#include "navigator/Reader/reader.h"
#include "navigator/Renderer/graph_renderer.h"
#include "navigator/Renderer/renderer_data.h"
#include "navigator/Routing_Graph/graph_builder.h"
#include "navigator/Routing_Graph/routing_graph.h"
#include "raylib.h"
#include <cstdint>
#include <iostream>

constexpr uint32_t WIDTH = 1920;
constexpr uint32_t HEIGHT = 1200;

Vector2 latLonToWorld(double lat, double lon, double ref_lat, double ref_lon) {
  constexpr double SCALE = 100000.0;

  float x = static_cast<float>((lon - ref_lon) * SCALE);
  float y = static_cast<float>(-(lat - ref_lat) * SCALE);

  return {x, y};
}

void LoadOSMData(std::string filename, RoutingGraph *graph,
                 RendererData *renderer_data) {
  OSMReader<GraphBuilder> reader(osmium::io::File(filename.c_str()),
                                 GraphBuilder(graph));
  reader.apply_reader();

  auto vertices = graph->get_vertex_map();
  if (vertices.empty())
    return;

  double ref_lat = vertices.begin()->second.Coords.lat();
  double ref_lon = vertices.begin()->second.Coords.lon();

  if (renderer_data->render_nodes) {
    for (const auto &a : vertices) {
      Vector2 pos = latLonToWorld(a.second.Coords.lat(), a.second.Coords.lon(),
                                  ref_lat, ref_lon);
      renderer_data->add_point(pos.x, pos.y);
    }
  }

  for (const auto &[edge_id, edge] : graph->get_edges()) {
    const auto &nodeA = graph->get_vertex_map().at(edge_id.first);
    const auto &nodeB = graph->get_vertex_map().at(edge_id.second);

    Vector2 posA =
        latLonToWorld(nodeA.Coords.lat(), nodeA.Coords.lon(), ref_lat, ref_lon);
    Vector2 posB =
        latLonToWorld(nodeB.Coords.lat(), nodeB.Coords.lon(), ref_lat, ref_lon);

    renderer_data->add_edge(posA.x, posA.y, posB.x, posB.y, edge.type);
  }
}

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
