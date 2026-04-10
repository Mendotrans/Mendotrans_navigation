#pragma once
#include "osmium/io/file.hpp"
#include "osmium/io/reader.hpp"
#include "reader_handler.h"

class OSM_Reader {
public:
  OSM_Reader(osmium::io::File filepath);
  ~OSM_Reader();

  void apply_reader();

private:
  Read_handler m_handler;
  osmium::io::File m_osm_path;

};
