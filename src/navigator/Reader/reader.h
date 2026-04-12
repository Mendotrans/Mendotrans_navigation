#pragma once
#include "osmium/handler/node_locations_for_ways.hpp"
#include "osmium/index/map/sparse_mem_array.hpp"
#include "osmium/io/file.hpp"
#include "osmium/io/reader.hpp"
#include "osmium/visitor.hpp"

template <typename Handler> class OSMReader {
public:
  OSMReader(osmium::io::File filepath, Handler handler) : m_osm_path(filepath), m_handler(handler) {}

  ~OSMReader() = default;

  void apply_reader() {
    using index_type =
        osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type,
                                           osmium::Location>;

    using location_handler_type =
        osmium::handler::NodeLocationsForWays<index_type>;

    index_type index;
    location_handler_type location_handler{index};

    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;

    osmium::io::Reader reader{m_osm_path, otypes};

    osmium::apply(reader, location_handler, m_handler);
  }

private:
  Handler m_handler;
  osmium::io::File m_osm_path;
};
