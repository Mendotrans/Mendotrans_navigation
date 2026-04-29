#include "graph_builder.h"
#include "osmium/osm/node.hpp"
#include "osmium/osm/way.hpp"
#include "raylib.h"
#include "street_edge.h"
#include <cstring>

GraphBuilder::GraphBuilder(RoutingGraph *rgraph) : m_routing_graph(rgraph) {}

void GraphBuilder::node(const osmium::Node &node) {
  m_routing_graph->add_vertex(node.id(), {node.id(), node.location()});
}

void GraphBuilder::way(const osmium::Way &way) {
  const char *highway = way.tags().get_value_by_key("highway");
  if (!highway) {
    return; // Not a road, skip it.
  }

  HighwayType type = HighwayType::unclassified;
  if (strcmp(highway, "motorway") == 0) {
    type = HighwayType::motorway;
  } else if (strcmp(highway, "trunk") == 0) {
    type = HighwayType::trunk;
  } else if (strcmp(highway, "primary") == 0) {
    type = HighwayType::primary;
  } else if (strcmp(highway, "secondary") == 0) {
    type = HighwayType::secondary;
  } else if (strcmp(highway, "tertiary") == 0) {
    type = HighwayType::tertiary;
  } else if (strcmp(highway, "residential") == 0) {
    type = HighwayType::residential;
  }

  bool is_oneway = false;
  const char *oneway = way.tags().get_value_by_key("oneway");
  if (oneway &&
      (std::strcmp(oneway, "yes") == 0 || std::strcmp(oneway, "1") == 0)) {
    is_oneway = true;
  }

  const auto &nodes = way.nodes();
  if (nodes.size() < 2) {
    return; // A valid edge requires at least two nodes
  }

  for (size_t i = 0; i < nodes.size() - 1; ++i) {
    const auto &n1 = nodes[i];
    const auto &n2 = nodes[i + 1];

    double distance = 0.0;
    try {
      distance =
          osmium::geom::haversine::distance(n1.location(), n2.location());
    } catch (const osmium::invalid_location &) {
      // If location data is missing, we can't route over it.
      continue;
    }

    // Add the forward edge (n1 -> n2)
    m_routing_graph->add_edge(n1.ref(), n2.ref(), distance, type);

    // If it's NOT a one-way street, add the reverse edge (n2 -> n1)
    if (!is_oneway) {
      m_routing_graph->add_edge(n2.ref(), n1.ref(), distance, type);
    }
  }
}
