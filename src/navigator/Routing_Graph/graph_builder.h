#pragma once

#include "osmium/handler.hpp"
#include "routing_graph.h"
#undef PI
#include <osmium/geom/haversine.hpp>

class GraphBuilder : public osmium::handler::Handler {
public:
  GraphBuilder(RoutingGraph *rgraph);

  void node(const osmium::Node &node);
  void way(const osmium::Way &way);

private:
  RoutingGraph *m_routing_graph;
};
