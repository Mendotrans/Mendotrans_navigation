#pragma once

#include "raylib.h"
struct Circle {
	Circle (Vector2 center, float radius, Color color) : center(center), radius(radius), color(color) {}
  Vector2 center;
  float radius;
  Color color;
};

#include <vector>
class GraphRenderer {
public:
  GraphRenderer();
  ~GraphRenderer();

  void init(int width, int height, const char *title,
            const float default_view_x = 0.0, const float default_view_y = 0.0,
            const int target_fps = 60);
  void update();
  void shutdown();

  void add_point(int x, int y);

private:
  std::vector<Circle> m_point_list;

  Camera2D m_camera;
	Vector2 m_viewing_point; // HACK: Duplicated memory
};
