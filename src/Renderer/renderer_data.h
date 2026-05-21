#pragma once
#include "RoutingGraph/street_edge.h"
#include "raylib.h"
#include <atomic>
#include <mutex>
#include <vector>

struct Circle {
  Vector2 center;
  float radius;
  Color color;
};
struct Edge {
  Vector2 start;
  Vector2 end;
  Color color;
  float thickness;
};

// Raw geographic primitives — no projection applied yet
struct GeoPoint {
  GeoPoint(double lat_a, double lon_a, Color color_a, float radius_a)
      : lat(lat_a), lon(lon_a), color(color_a), radius(radius_a) {}
  double lat, lon;
  Color color;
  float radius;
};
struct GeoEdge {
  double lat1, lon1;
  double lat2, lon2;
  HighwayType type;
  float thickness;
};

class RendererData {
public:
  RendererData() = default;
  ~RendererData() = default;

  void add_point(double lat, double lon);
  void add_edge(double lat1, double lon1, double lat2, double lon2,
                HighwayType type);

  // Reference origin for projection (set on first point added)
  double ref_lat = 0.0;
  double ref_lon = 0.0;
  bool ref_set = false;

  std::vector<GeoPoint> points;
  std::vector<GeoEdge> edges;
  std::vector<GeoPoint> stops;
  std::mutex data_mtx;
  std::atomic<bool> loading_done{false};
  bool render_nodes = false;
  bool highway_colors = false;
  bool render_stops = false;
};
