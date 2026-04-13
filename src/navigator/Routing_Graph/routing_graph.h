#pragma once

#include "graaflib/graph.h"
#include "graph_nodes.h"

class RoutingGraph {
public:
  RoutingGraph();
  ~RoutingGraph();

  void add_vertex(Street_graph_node node);

  const std::unordered_map<graaf::vertex_id_t, Street_graph_node> &
  get_vertexArray();

private:
  graaf::directed_graph<Street_graph_node, float> m_navigaitonGraph;
};
