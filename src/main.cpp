#include "navigator/Reader/reader.h"
#include "navigator/Renderer/graph_renderer.h"
#include "navigator/Routing_Graph/graph_builder.h"
#include "navigator/Routing_Graph/routing_graph.h"
#include "raylib.h"
#include <cstdint>

constexpr uint32_t WIDTH = 1000;
constexpr uint32_t HEIGH = 1000;

int main() {
  GraphRenderer graph_renderer;
  graph_renderer.init(WIDTH, HEIGH, "Graph Renderer", 500, 500, 60);

  RoutingGraph routing_graph;
  OSMReader<GraphBuilder> reader(
      osmium::io::File("/home/ironowl/Downloads/map(1).osm"),
      GraphBuilder(&routing_graph));
  reader.apply_reader();

  // for (std::pair<graaf::vertex_id_t, Street_graph_node> a :
  //      routing_graph.get_vertexArray()) {
  //   // graph_renderer.add_point(a.);
  //   std::cout << "Adding Point :" << a.first << ' ' << a.second.Coords.lat()
  //             << ' ' << a.second.Coords.lon() << '\n';
  // }
  graph_renderer.add_point(1, 1);

  while (!WindowShouldClose()) {
    graph_renderer.update();
  }
  graph_renderer.shutdown();
  return 0;
}
