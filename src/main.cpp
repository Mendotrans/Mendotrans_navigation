#include "PublicTransportSystem/mendotran_types.h"
#include "PublicTransportSystem/public_transport_system.h"
#include "Renderer/graph_renderer.h"
#include "Renderer/renderer_data.h"
#include "RoutingGraph/routing_graph.h"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

int main(int argc, char *argv[]) {
  // Application Args -------------------------------------------------------
  if (argc < 2) {
    std::cerr << "Usage: mendotrans-router <filepath> [Options]\n"
              << "Options:\n"

              << "-render-nodes\t Show OSM nodes\n"

              << "-highway-colors\t Use colors for highway types\n"

              << "-fetch-data\t Creates and populates public transport db, "
                 "errors when it finds one\n"

              << "-render-stops\t Renders stops, errors when no db" << '\n';
    return 1;
  }
  std::string filename = argv[1];
  std::cout << "Called with path: " << filename << '\n';

  RoutingGraph routing_graph;
  RendererData renderer_data;
  bool fetch_data = false;

  for (int i = 2; i < argc; ++i) {
    std::string option = argv[i];
    if (option == "-render-nodes") {
      renderer_data.render_nodes = true;
      continue;
    } else if (option == "-highway-colors") {
      renderer_data.highway_colors = true;
      continue;
    } else if (option == "-fetch-data") {
      fetch_data = true;
      continue;
    } else if (option == "-render-stops") {
      renderer_data.render_stops = true;
      continue;
    }

    std::cout << "Invalid option: " << argv[i] << '\n';
    return 1;
  }
  // -----------------------------------------------------------------------

  // Fetching public transport data ----------------------------------------
  //
  mendotran::ApiConfig conf{
      .base_url = "owa.visionblo.com",
      .base_path = "/api/mendoza",
      .token = "OQkGfHEQqWRO9zXRQgJb",
      .xss_search = "cacdea08b541276c47572b40",
      .xss_arrivals = "cacdea08b541276c47572b40",
      .xss_service = "cacdea08b541276c47572b40",
      .use_https = true,
  };

  if (fetch_data) {
    std::cout << "Fetching public transport data" << '\n';

    mendotran::PublicTransportSystem public_transport_system(
        "mendotran_data.db", conf);

    public_transport_system.init_static_data();

    public_transport_system.fetch_all_service_details(
        std::chrono::milliseconds(1));
    return 0;
  }

  std::thread loader_thread([&]() {
    LoadOSMData(filename, &routing_graph, &renderer_data);

    if (renderer_data.render_stops) {
      mendotran::PublicTransportSystem public_transport_system(
          "mendotran_data.db", conf);

      {
        std::lock_guard<std::mutex> lock(renderer_data.data_mtx);

        for (const mendotran::Stop &stop :
             public_transport_system.get_all_stops()) {
          renderer_data.stops.push_back(GeoPoint(stop.lat, stop.lon, GREEN, 5));
        }
      }
    }

    renderer_data.loading_done = true;
  });
  loader_thread.detach();

  GraphRendererArgs renderer_args = {
      .width = WIDTH,
      .height = HEIGHT,
      .title = "Graph Renderer",
      .default_view_x = 0,
      .default_view_y = 0,
      .target_fps = 60,
      .zoom_fade = true,
  };
  GraphRenderer graph_renderer;

  graph_renderer.init(&renderer_args);
  graph_renderer.begin_render(&renderer_data);
  return 0;
}
