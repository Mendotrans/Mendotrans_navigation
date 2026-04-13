
#include "graph_builder.h"
#include "graph_nodes.h"
#include "osmium/osm/node.hpp"

GraphBuilder::GraphBuilder(RoutingGraph *rgraph) : m_routing_graph(rgraph) {}

void GraphBuilder::way(const osmium::Way &way) {}

void GraphBuilder::node(const osmium::Node &node) {
  m_routing_graph->add_vertex({node.id(), node.location()});
}
