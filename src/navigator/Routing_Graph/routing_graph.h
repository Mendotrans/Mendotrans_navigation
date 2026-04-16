#pragma once

#include "graaflib/graph.h"
#include "graph_nodes.h"
#include "osmium/osm/types.hpp"
#include <unordered_map>

class RoutingGraph {
public:
  RoutingGraph() = default;
  ~RoutingGraph() = default;

  void add_vertex(osmium::object_id_type osm_id, Street_graph_node node);

  void add_edge(osmium::object_id_type lhs, osmium::object_id_type rhs,
                float weight);

  const std::unordered_map<graaf::vertex_id_t, Street_graph_node> &
  get_vertex_map() const;

  const std::unordered_map<graaf::edge_id_t, float, graaf::edge_id_hash> &
  get_edges() const;

private:
  graaf::directed_graph<Street_graph_node, float> m_navigationGraph;

  std::unordered_map<osmium::object_id_type, graaf::vertex_id_t>
      m_osm_to_graaf_id;
};
