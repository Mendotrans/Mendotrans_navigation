
#include "routing_graph.h"
#include "graaflib/graph.h"

Routing_Graph::Routing_Graph() {
  m_navigaitonGraph = graaf::directed_graph<Street_graph_node, float>();
}
Routing_Graph::~Routing_Graph() {}

void Routing_Graph::add_vertex(Street_graph_node node) {
  m_navigaitonGraph.add_vertex(node);
}
