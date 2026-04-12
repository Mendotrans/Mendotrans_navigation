#pragma once

#include "osmium/handler.hpp"
#include "routing_graph.h"

class GraphBuilder : public osmium::handler::Handler {
public:
  GraphBuilder(Routing_Graph *rgraph);
  void way(const osmium::Way &way);

  void node(const osmium::Node &node);

private:
  Routing_Graph *m_routing_graph;
};
