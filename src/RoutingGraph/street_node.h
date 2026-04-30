#pragma once

#include "osmium/osm/location.hpp"
#include "osmium/osm/types.hpp"

struct StreetNode {
  osmium::object_id_type ID;
  osmium::Location Coords;
};
