#include "navigator/Reader/reader.h"
#include "navigator/Renderer/graph_renderer.h"
#include "navigator/Routing_Graph/graph_builder.h"
#include "navigator/Routing_Graph/routing_graph.h"
#include "raylib.h"
#include <cstdint>
#include <iostream>
#include <limits>

constexpr uint32_t WIDTH = 1000;
constexpr uint32_t HEIGHT = 1000;

Vector2 latLonToWorld(double lat, double lon, double ref_lat, double ref_lon) {
  constexpr double SCALE = 100000.0;

  float x = static_cast<float>((lon - ref_lon) * SCALE);
  float y = static_cast<float>(-(lat - ref_lat) * SCALE);

  return {x, y};
}

int main() {
  GraphRenderer graph_renderer;
  graph_renderer.init(WIDTH, HEIGHT, "Graph Renderer", 0, 0, 60);

  RoutingGraph routing_graph;
  OSMReader<GraphBuilder> reader(
      osmium::io::File("/home/ironowl/Downloads/map(1).osm"),
      GraphBuilder(&routing_graph));
  reader.apply_reader();

  auto vertices = routing_graph.get_vertexArray();
  if (vertices.empty())
    return 0;

  double ref_lat = vertices.begin()->second.Coords.lat();
  double ref_lon = vertices.begin()->second.Coords.lon();

  // 1. Add all points (Nodes)
  for (const auto &a : vertices) {
    double lat = a.second.Coords.lat();
    double lon = a.second.Coords.lon();

    Vector2 pos = latLonToWorld(lat, lon, ref_lat, ref_lon);
    graph_renderer.add_point(pos.x, pos.y);
  }

  // 2. Add all relations (Edges)
  // *NOTE*: The exact method here depends on your 'graaf' routing_graph API.
  // Replace `get_edges()` and the loop structure with however your library
  // fetches adjacency/edges.
  /*
  for (const auto& edge : routing_graph.get_edges()) {
      auto nodeA = routing_graph.get_vertex(edge.source_id);
      auto nodeB = routing_graph.get_vertex(edge.target_id);

      Vector2 posA = latLonToWorld(nodeA.Coords.lat(), nodeA.Coords.lon(),
  ref_lat, ref_lon); Vector2 posB = latLonToWorld(nodeB.Coords.lat(),
  nodeB.Coords.lon(), ref_lat, ref_lon);

      graph_renderer.add_edge(posA.x, posA.y, posB.x, posB.y);
  }
  */

  while (!WindowShouldClose()) {
    graph_renderer.update();
  }

  graph_renderer.shutdown();
  return 0;
}
