#include "osmium/osm/location.hpp"
#include <iostream>

#include <osmium/handler.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/visitor.hpp>

#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>

class MyHandler : public osmium::handler::Handler {
public:
  void way(const osmium::Way &way) {
    std::cout << "way " << way.id() << '\n';
    for (const osmium::Tag &t : way.tags()) {
      std::cout << '\t' << t.key() << "=" << t.value() << '\n';
    }
    std::cout << "way nodes\n";
    for (auto a : way.nodes()) {
      try {
        std::cout << '\t' << a.lat() << ' ' << a.lon() << ' ' << a.ref()
                  << '\n';
      } catch (osmium::invalid_location e) {
        std::cout << '\t' << "Invalid" << '\n';
      }
    }
  }

  void node(const osmium::Node &node) {
    std::cout << "node " << node.id() << '\n';
  }
};

int main() {
  using index_type =
      osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type,
                                         osmium::Location>;

  using location_handler_type =
      osmium::handler::NodeLocationsForWays<index_type>;

  index_type index;
  location_handler_type location_handler{index};

  auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
  osmium::io::Reader reader{"/home/ironowl/Downloads/map(1).osm", otypes};

  MyHandler handler;

  osmium::apply(reader, location_handler, handler);

  reader.close();
}
