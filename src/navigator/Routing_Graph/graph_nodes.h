#pragma once

#include "osmium/osm/location.hpp"
#include "osmium/osm/types.hpp"
#include <cstdint>

struct Street_graph_node{
	osmium::object_id_type ID;
	osmium::Location Coords;
};
