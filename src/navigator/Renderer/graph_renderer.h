#pragma once

#include "raylib.h"
struct Circle {
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
            const int target_fps = 60);
  void update();
  void shutdown();

  void add_point(int x, int y);

private:
  std::vector<Circle> m_point_list;
};
