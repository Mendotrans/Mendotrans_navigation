#pragma once

#include <string>

namespace mendotran {

struct Stop {
  int stop_id;
  std::string type;
  double lat;
  double lon;
  std::string code;
  std::string location;
};

struct Service {
  int service_id;
  std::string type;
  int group_id;
  std::string code;
  std::string name;
  std::string color;
  std::string mode;
};

struct Group {
  std::string group_id;
  std::string name;
};

struct ApiConfig {
  std::string base_url;  // e.g. "owa.visionblo.com"
  std::string base_path; // e.g. "/api/mendoza"
  std::string token;
  std::string xss_search;
  std::string xss_arrivals;
  std::string xss_service;
  bool use_https = true;
};

} // namespace mendotran
