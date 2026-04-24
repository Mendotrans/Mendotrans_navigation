#pragma once

#include "raylib.h"
#include "renderer_info.h"
#undef PI

class GraphRenderer {
public:
  GraphRenderer(RendererInfo *render_data);
  ~GraphRenderer();

  void init(int width, int height, const char *title,
            const float default_view_x, const float default_view_y,
            const int target_fps);
  void update();
  void shutdown();

  void manage_movement();

  void begin_render(int width, int height, const char *title,
                    const float default_view_x, const float default_view_y,
                    const int target_fps);

private:
  Camera2D m_camera = {0};
  Vector2 m_viewing_point = {0};

  RendererInfo *mp_renderer_info;
};
