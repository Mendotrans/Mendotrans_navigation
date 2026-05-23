#include "renderer_data.h"
#include "raylib.h"

void RendererData::add_point(double lat, double lon) {
  std::lock_guard<std::mutex> lock(data_mtx);
  if (!ref_set) {
    ref_lat = lat;
    ref_lon = lon;
    ref_set = true;
  }
  points.push_back({lat, lon, RED, 2.0f});
}

void RendererData::add_edge(double lat1, double lon1, double lat2, double lon2,
                            HighwayType type) {
  float thickness = 1.0f;
  switch (type) {
  case HighwayType::motorway:
  case HighwayType::motorway_link:
    thickness = 3.0f;
    break;
  case HighwayType::trunk:
  case HighwayType::trunk_link:
    thickness = 3.0f;
    break;
  case HighwayType::secondary:
  case HighwayType::secondary_link:
    thickness = 2.0f;
    break;
  default:
    thickness = 1.0f;
    break;
  }

  std::lock_guard<std::mutex> lock(data_mtx);
  if (!ref_set) {
    ref_lat = lat1;
    ref_lon = lon1;
    ref_set = true;
  }
  edges.push_back({lat1, lon1, lat2, lon2, type, thickness});
}
