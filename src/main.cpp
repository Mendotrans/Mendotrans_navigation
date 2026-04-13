#include "navigator/Reader/reader.h"
#include "navigator/Renderer/graph_renderer.h"
#include "navigator/Routing_Graph/graph_builder.h"
#include "navigator/Routing_Graph/routing_graph.h"
#include "raylib.h"
#include <iostream>

int main() {
  // Routing_Graph routing_graph;
  // OSMReader<GraphBuilder> reader(
  //     osmium::io::File("/home/ironowl/Downloads/map(1).osm"),
  //     GraphBuilder(&routing_graph));
  // reader.apply_reader();
  //
  // for (auto a : routing_graph.get_vertexArray()) {
  //   std::cout << a << '\n';
  // }

  GraphRenderer graph_renderer;
  graph_renderer.init(1000, 1000, "Graph Renderer");
  graph_renderer.add_point(500, 500);
  while (!WindowShouldClose()) {
    graph_renderer.update();
  }
  graph_renderer.shutdown();
  return 0;
}
