#pragma once

#include "graaflib/graph.h"
#include "graaflib/types.h"
#include "graph_nodes.h"
#include <vector>

class Routing_Graph {
public:
  Routing_Graph();
  ~Routing_Graph();

  void add_vertex(Street_graph_node node);

  std::vector<graaf::vertex_id_t> &get_vertexArray();

private:
  graaf::directed_graph<Street_graph_node, float> m_navigaitonGraph;
  std::vector<graaf::vertex_id_t> m_vertexArray;
};
