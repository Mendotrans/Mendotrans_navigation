
#include "routing_graph.h"
#include "graaflib/graph.h"

Routing_Graph::Routing_Graph() {
  m_navigaitonGraph = graaf::directed_graph<Street_graph_node, float>();
}
Routing_Graph::~Routing_Graph() {}

std::vector<graaf::vertex_id_t>& Routing_Graph::get_vertexArray() {
  return m_vertexArray;
}

void Routing_Graph::add_vertex(Street_graph_node node) {
  m_vertexArray.push_back(m_navigaitonGraph.add_vertex(node));
}
