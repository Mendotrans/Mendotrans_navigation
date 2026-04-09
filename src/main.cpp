#include "navigator/reader.h"
#include "osmium/osm/location.hpp"
#include <iostream>

int main() {
  OSM_Reader reader(osmium::io::File("/home/ironowl/Downloads/map(1).osm"));
	reader.apply_reader();
}
