#include "reader.h"
#include "osmium/handler/node_locations_for_ways.hpp"
#include "osmium/index/map/sparse_mem_array.hpp"
#include "osmium/io/reader.hpp"
#include "osmium/visitor.hpp"
#include "reader_handler.h"

OSM_Reader::OSM_Reader(osmium::io::File filepath) {
  m_handler = Read_handler();
  m_osm_path = filepath;
}
OSM_Reader::~OSM_Reader() {}

void OSM_Reader::apply_reader() {
  using index_type =
      osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type,
                                         osmium::Location>;

  using location_handler_type =
      osmium::handler::NodeLocationsForWays<index_type>;

	index_type index;
  location_handler_type location_handler{index};

  auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;

  auto reader = osmium::io::Reader{m_osm_path, otypes};

  osmium::apply(reader, location_handler, m_handler);
}
