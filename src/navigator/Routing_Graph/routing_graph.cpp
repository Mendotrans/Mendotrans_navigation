#include "routing_graph.h"

const std::unordered_map<graaf::vertex_id_t, Street_graph_node> &
RoutingGraph::get_vertex_map() const {
  return m_navigationGraph.get_vertices();
}

const std::unordered_map<graaf::edge_id_t, float, graaf::edge_id_hash> &
RoutingGraph::get_edges() const {
  return m_navigationGraph.get_edges();
}

void RoutingGraph::add_vertex(osmium::object_id_type osm_id,
                              Street_graph_node node) {
  graaf::vertex_id_t generated_graaf_id = m_navigationGraph.add_vertex(node);

  m_osm_to_graaf_id[osm_id] = generated_graaf_id;
}

void RoutingGraph::add_edge(osmium::object_id_type lhs_osm,
                            osmium::object_id_type rhs_osm, float weight) {
  auto it_lhs = m_osm_to_graaf_id.find(lhs_osm);
  auto it_rhs = m_osm_to_graaf_id.find(rhs_osm);

  if (it_lhs != m_osm_to_graaf_id.end() && it_rhs != m_osm_to_graaf_id.end()) {
    m_navigationGraph.add_edge(it_lhs->second, it_rhs->second, weight);
  }
}
