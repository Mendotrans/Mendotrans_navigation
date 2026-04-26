#include "renderer_data.h"

void RendererData::add_point(float x, float y) {
  points.push_back({{x, y}, 2.0f, RED});
}
void RendererData::add_edge(float x1, float y1, float x2, float y2) {
  edges.push_back({{x1, y1}, {x2, y2}, DARKGRAY, 1.0f});
}
