#pragma once

#include "raylib.h"
#include "renderer_data.h"
#undef PI

Vector2 latLonToWorld(double lat, double lon, double ref_lat, double ref_lon);

struct GraphRendererArgs {
  int width = 1920;
  int height = 1080;
  const char *title = "Graph Renderer";
  const float default_view_x = 0;
  const float default_view_y = 0;
  const int target_fps = 60;
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
