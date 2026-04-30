#include "renderer_data.h"
#include "../Routing_Graph/street_edge.h"
#include "raylib.h"

void RendererData::add_point(float x, float y) {
  std::lock_guard<std::mutex> lock(data_mtx);
  points.push_back({{x, y}, 2.0f, RED});
}
void RendererData::add_edge(float x1, float y1, float x2, float y2,
                            HighwayType type) {

  Color color;
  float thickness = 1.0f;
  switch (type) {
  case HighwayType::motorway:
  case HighwayType::motorway_link:
    color = RED;
    thickness = 3.0f;
    break;
  case HighwayType::trunk:
  case HighwayType::trunk_link:
    color = ORANGE;
    thickness = 3.0f;
    break;
  case HighwayType::primary:
  case HighwayType::primary_link:
    color = GOLD;
    break;
  case HighwayType::secondary:
  case HighwayType::secondary_link:
    color = YELLOW;
    thickness = 2.0f;
    break;
  case HighwayType::tertiary:
  case HighwayType::tertiary_link:
    color = BEIGE;
    break;
  case HighwayType::unclassified:
    color = GRAY;
    break;
  case HighwayType::residential:
  case HighwayType::residential_link:
    color = BROWN;
    break;
    break;
  }
  std::lock_guard<std::mutex> lock(data_mtx);
  edges.push_back(
      {{x1, y1}, {x2, y2}, highway_colors ? color : GRAY, thickness});
}
