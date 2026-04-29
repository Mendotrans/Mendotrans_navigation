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
  switch (type) {
  case HighwayType::motorway:
    color = RED;
    break;
  case HighwayType::trunk:
    color = ORANGE;
    break;
  case HighwayType::primary:
    color = GOLD;
    break;
  case HighwayType::secondary:
    color = YELLOW;
    break;
  case HighwayType::tertiary:
    color = BEIGE;
    break;
  case HighwayType::unclassified:
    color = GRAY;
    break;
  case HighwayType::residential:
    color = BROWN;
    break;
  }
  std::lock_guard<std::mutex> lock(data_mtx);
  edges.push_back({{x1, y1}, {x2, y2}, color, 1.0f});
}
