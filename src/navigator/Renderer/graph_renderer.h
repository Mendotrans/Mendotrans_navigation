#pragma once

#include "raylib.h"
#undef PI
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

class GraphRenderer {
public:
  GraphRenderer();
  ~GraphRenderer();

  void init(int width, int height, const char *title,
            const float default_view_x, const float default_view_y,
            const int target_fps);
  void update();
  void shutdown();

  void add_point(float x, float y);
  void add_edge(float x1, float y1, float x2, float y2);

private:
  std::vector<Circle> m_point_list;
  std::vector<Edge> m_edge_list;
  Camera2D m_camera = {0};
  Vector2 m_viewing_point = {0};
};
