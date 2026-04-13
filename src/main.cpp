#include "navigator/Reader/reader.h"
#include "navigator/Renderer/graph_renderer.h"
#include "navigator/Routing_Graph/graph_builder.h"
#include "navigator/Routing_Graph/routing_graph.h"
#include "raylib.h"
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <iostream>

constexpr uint32_t WIDTH = 1000;
constexpr uint32_t HEIGH = 1000;

int main() {
  GraphRenderer graph_renderer;
  graph_renderer.init(1000, 1000, "Graph Renderer");

  RoutingGraph routing_graph;
  OSMReader<GraphBuilder> reader(
      osmium::io::File("/home/ironowl/Downloads/map(1).osm"),
      GraphBuilder(&routing_graph));
  reader.apply_reader();

  for (std::pair<graaf::vertex_id_t, Street_graph_node> a :
       routing_graph.get_vertexArray()) {
    graph_renderer.add_point((std::abs(a.second.Coords.lat() / WIDTH)),
                             std::abs(a.second.Coords.lon() / HEIGH));
    std::cout << "Adding Point :" << a.first << ' ' << a.second.Coords.lat()
              << ' ' << a.second.Coords.lon() << '\n';
  }

  while (!WindowShouldClose()) {
    graph_renderer.update();
  }
  graph_renderer.shutdown();
  return 0;
}
