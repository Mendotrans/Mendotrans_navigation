#include "graph_renderer.h"
#include "raylib.h"


GraphRenderer::GraphRenderer(){}
GraphRenderer::~GraphRenderer(){}

void GraphRenderer::init(int width, int height, const char *title,
                         const int target_fps) {

  InitWindow(width, height, title);
  SetTargetFPS(target_fps);
}
void GraphRenderer::update() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (Circle a : m_point_list) {
    DrawCircle(a.center.x, a.center.y, a.radius, a.color);
  }
  EndDrawing();
}
void GraphRenderer::shutdown() { CloseWindow(); }

void GraphRenderer::add_point(int x, int y) {
  m_point_list.push_back(
      (Circle){{static_cast<float>(x), static_cast<float>(y)}, 3, RED});
}
