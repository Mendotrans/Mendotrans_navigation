#pragma once

#include "raylib.h"
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

class RendererData {
public:
  RendererData() = default;
  ~RendererData() = default;
  void add_point(float x, float y);
  void add_edge(float x1, float y1, float x2, float y2);

  std::vector<Circle> points;
  std::vector<Edge> edges;
};
