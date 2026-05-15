#include "routing_graph.h"
#include "Reader/reader.h"
#include "Renderer/graph_renderer.h"
#include "Renderer/renderer_data.h"
#include "RoutingGraph/graph_builder.h"

const std::unordered_map<graaf::vertex_id_t, StreetNode> &
RoutingGraph::get_vertex_map() const {
  return m_navigationGraph.get_vertices();
}

const std::unordered_map<graaf::edge_id_t, StreetEdge, graaf::edge_id_hash> &
RoutingGraph::get_edges() const {
  return m_navigationGraph.get_edges();
}

void RoutingGraph::add_vertex(osmium::object_id_type osm_id, StreetNode node) {
  graaf::vertex_id_t generated_graaf_id = m_navigationGraph.add_vertex(node);

  m_osm_to_graaf_id[osm_id] = generated_graaf_id;
}

void RoutingGraph::add_edge(osmium::object_id_type lhs_osm,
                            osmium::object_id_type rhs_osm, float weight,
                            HighwayType type) {
  auto it_lhs = m_osm_to_graaf_id.find(lhs_osm);
  auto it_rhs = m_osm_to_graaf_id.find(rhs_osm);

  if (it_lhs != m_osm_to_graaf_id.end() && it_rhs != m_osm_to_graaf_id.end()) {
    m_navigationGraph.add_edge(it_lhs->second, it_rhs->second,
                               StreetEdge(weight, type));
  }
}

void LoadOSMData(std::string filename, RoutingGraph *graph,
                 RendererData *renderer_data) {
  OSMReader<GraphBuilder> reader(osmium::io::File(filename.c_str()),
                                 GraphBuilder(graph));
  reader.apply_reader();

  auto vertices = graph->get_vertex_map();
  if (vertices.empty())
    return;

  double ref_lat = vertices.begin()->second.Coords.lat();
  double ref_lon = vertices.begin()->second.Coords.lon();

  if (renderer_data->render_nodes) {
    for (const auto &a : vertices) {
      Vector2 pos = latLonToWorld(a.second.Coords.lat(), a.second.Coords.lon(),
                                  ref_lat, ref_lon);
      renderer_data->add_point(pos.x, pos.y);
    }
  }

  for (const auto &[edge_id, edge] : graph->get_edges()) {
    const auto &nodeA = graph->get_vertex_map().at(edge_id.first);
    const auto &nodeB = graph->get_vertex_map().at(edge_id.second);

    Vector2 posA =
        latLonToWorld(nodeA.Coords.lat(), nodeA.Coords.lon(), ref_lat, ref_lon);
    Vector2 posB =
        latLonToWorld(nodeB.Coords.lat(), nodeB.Coords.lon(), ref_lat, ref_lon);

    renderer_data->add_edge(posA.x, posA.y, posB.x, posB.y, edge.type);
  }
}
