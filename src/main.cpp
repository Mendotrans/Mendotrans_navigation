#include "navigator/Reader/reader.h"
#include "navigator/Renderer/graph_renderer.h"
#include "navigator/Renderer/renderer_info.h"
#include "navigator/Routing_Graph/graph_builder.h"
#include "navigator/Routing_Graph/routing_graph.h"
#include "raylib.h"
#include <cstdint>
#include <iostream>

constexpr uint32_t WIDTH = 1920;
constexpr uint32_t HEIGHT = 1100;

Vector2 latLonToWorld(double lat, double lon, double ref_lat, double ref_lon) {
  constexpr double SCALE = 100000.0;

  float x = static_cast<float>((lon - ref_lon) * SCALE);
  float y = static_cast<float>(-(lat - ref_lat) * SCALE);

  return {x, y};
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: mendotrans-router <filepath>" << '\n';
    return 1;
  }
  std::string filename = argv[1];
  std::cout << "Called with path: " << filename << '\n';

  RoutingGraph routing_graph;
  OSMReader<GraphBuilder> reader(osmium::io::File(filename.c_str()),
                                 GraphBuilder(&routing_graph));
  reader.apply_reader();

  RendererInfo renderer_info;

  auto vertices = routing_graph.get_vertex_map();
  if (vertices.empty()) {
    std::cerr << "Empty Graph!" << '\n';
    return 0;
  }

  double ref_lat = vertices.begin()->second.Coords.lat();
  double ref_lon = vertices.begin()->second.Coords.lon();

  for (const auto &a : vertices) {
    double lat = a.second.Coords.lat();
    double lon = a.second.Coords.lon();

    Vector2 pos = latLonToWorld(lat, lon, ref_lat, ref_lon);
    renderer_info.add_point(pos.x, pos.y);
  }

  for (const auto &[edge_id, weight] : routing_graph.get_edges()) {
    const auto &nodeA = routing_graph.get_vertex_map().at(edge_id.first);
    const auto &nodeB = routing_graph.get_vertex_map().at(edge_id.second);

    Vector2 posA =
        latLonToWorld(nodeA.Coords.lat(), nodeA.Coords.lon(), ref_lat, ref_lon);
    Vector2 posB =
        latLonToWorld(nodeB.Coords.lat(), nodeB.Coords.lon(), ref_lat, ref_lon);

    renderer_info.add_edge(posA.x, posA.y, posB.x, posB.y);
  }

  GraphRenderer graph_renderer(&renderer_info);
  graph_renderer.begin_render(WIDTH, HEIGHT, "Map Renderer", 0, 0, 60);
  return 0;
}
