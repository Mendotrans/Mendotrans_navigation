#pragma once

#include "Renderer/renderer_data.h"
#include "graaflib/graph.h"
#include "osmium/osm/types.hpp"
#include "street_edge.h"
#include "street_node.h"
#include <unordered_map>

class RoutingGraph {
public:
  RoutingGraph() = default;
  ~RoutingGraph() = default;

  void add_vertex(osmium::object_id_type osm_id, StreetNode node);

  void add_edge(osmium::object_id_type lhs_osm, osmium::object_id_type rhs_osm,
                float weight, HighwayType type = HighwayType::unclassified);

  const std::unordered_map<graaf::vertex_id_t, StreetNode> &
  get_vertex_map() const;

  const std::unordered_map<graaf::edge_id_t, StreetEdge, graaf::edge_id_hash> &
  get_edges() const;

private:
  graaf::directed_graph<StreetNode, StreetEdge> m_navigationGraph;

  std::unordered_map<osmium::object_id_type, graaf::vertex_id_t>
      m_osm_to_graaf_id;
};

void LoadOSMData(std::string filename, RoutingGraph *graph,
                 RendererData *renderer_data);
