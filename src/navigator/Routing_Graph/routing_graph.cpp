#include "routing_graph.h"

RoutingGraph::RoutingGraph() {
  m_navigaitonGraph = graaf::directed_graph<Street_graph_node, float>();
}
RoutingGraph::~RoutingGraph() {}

const std::unordered_map<graaf::vertex_id_t, Street_graph_node> &
RoutingGraph::get_vertexArray() {
  return m_navigaitonGraph.get_vertices();
}

void RoutingGraph::add_vertex(Street_graph_node node) {
  m_navigaitonGraph.add_vertex(node);
}
