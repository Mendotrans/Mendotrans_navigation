#pragma once

#include "graaflib/graph.h"
#include "graph_nodes.h"

class Routing_Graph {
public:
  Routing_Graph();
  ~Routing_Graph();

  void add_vertex(Street_graph_node node);

private:
  graaf::directed_graph<Street_graph_node, float> m_navigaitonGraph;
};
