#include "Renderer/graph_renderer.h"
#include "RoutingGraph/routing_graph.h"
#include <cstdint>
#include <iostream>
#include <thread>

constexpr uint32_t WIDTH = 1920;
constexpr uint32_t HEIGHT = 1200;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: mendotrans-router <filepath> [Options]\n"
              << "Options:\n"
              << "-render-nodes\t Show OSM nodes\n"
              << "-highway-colors\t Use colors for highway types\n";
    return 1;
  }
  std::string filename = argv[1];
  std::cout << "Called with path: " << filename << '\n';

  RoutingGraph routing_graph;
  RendererData renderer_data;

  for (int i = 2; i < argc; ++i) {
    std::string option = argv[i];
    if (option == "-render-nodes") {
      renderer_data.render_nodes = true;
      continue;
    } else if (option == "-highway-colors") {
      renderer_data.highway_colors = true;
      continue;
    }

    std::cout << "Invalid option: " << argv[i] << '\n';
    return 1;
  }

  std::thread loader_thread([&]() {
    LoadOSMData(filename, &routing_graph, &renderer_data);
    renderer_data.loading_done = true;
  });
  loader_thread.detach();

  GraphRendererArgs renderer_args = {.width = WIDTH,
                                     .height = HEIGHT,
                                     .title = "Graph Renderer",
                                     .default_view_x = 0,
                                     .default_view_y = 0,
                                     .target_fps = 60};
  GraphRenderer graph_renderer;

  graph_renderer.init(&renderer_args);
  graph_renderer.begin_render(&renderer_data);
  return 0;
}
