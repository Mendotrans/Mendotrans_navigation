#include "reader_handler.h"

#include <osmium/handler.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/visitor.hpp>

#include <iostream>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>

void Read_handler::way(const osmium::Way &way) {
  std::cout << "way " << way.id() << '\n';
  for (const osmium::Tag &t : way.tags()) {
    std::cout << '\t' << t.key() << "=" << t.value() << '\n';
  }
  std::cout << "way nodes\n";
  for (auto a : way.nodes()) {
    try {
      std::cout << '\t' << a.lat() << ' ' << a.lon() << ' ' << a.ref() << '\n';
    } catch (osmium::invalid_location e) {
      std::cout << '\t' << "Invalid" << '\n';
    }
  }
}

void Read_handler::node(const osmium::Node &node) {
  std::cout << "node " << node.id() << node.location().lat() << ' ' << node.location().lon() <<  '\n';
}
