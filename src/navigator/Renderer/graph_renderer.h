#pragma once

#include "raylib.h"
#include "renderer_data.h"
#undef PI

struct GraphRendererArgs {
  int width;
  int height;
  const char *title;
  const float default_view_x;
  const float default_view_y;
  const int target_fps;
};

class GraphRenderer {
public:
  GraphRenderer();
  ~GraphRenderer();

  void init(GraphRendererArgs *args);
  void update();
  void shutdown();

  void manage_movement();

  void begin_render(RendererData *render_data);

private:
  Camera2D m_camera = {0};
  Vector2 m_viewing_point = {0};

  RendererData *mp_renderer_data;
  GraphRendererArgs *mp_renderer_args;
};
