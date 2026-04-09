#pragma once

#include <osmium/handler.hpp>

class Read_handler : public osmium::handler::Handler {
public:
  void way(const osmium::Way &way);

  void node(const osmium::Node &node);
};
