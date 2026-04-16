#include "graph_renderer.h"
#include "raylib.h"
#include <cmath>
#include <iostream>

GraphRenderer::GraphRenderer() {}
GraphRenderer::~GraphRenderer() {}

void GraphRenderer::init(int width, int height, const char *title,
                         const float default_view_x, const float default_view_y,
                         const int target_fps) {
  InitWindow(width, height, title);
  SetTargetFPS(target_fps);
  m_camera.target = {default_view_x, default_view_y};
  m_camera.offset = {width / 2.0f, height / 2.0f};
  m_camera.zoom = 1.0f;
  m_camera.rotation = 0.0f;
}
void GraphRenderer::update() {
  // HACK: This should be a diferent system
  if (IsKeyDown(KEY_RIGHT))
    m_viewing_point.x += 4;
  else if (IsKeyDown(KEY_LEFT))
    m_viewing_point.x -= 4;

  m_camera.target = {m_viewing_point.x + 20, m_viewing_point.y + 20};

  if (IsKeyDown(KEY_A))
    m_viewing_point.x--;
  if (IsKeyDown(KEY_D))
    m_viewing_point.x++;
  if (IsKeyDown(KEY_W))
    m_viewing_point.y--;
  if (IsKeyDown(KEY_S))
    m_viewing_point.y++;

  m_camera.zoom = expf(logf(m_camera.zoom) + GetMouseWheelMove() * 0.1f);
  if (m_camera.zoom > 10.0f)
    m_camera.zoom = 10.0f;
  else if (m_camera.zoom < 0.001f)
    m_camera.zoom = 0.01f;

  BeginDrawing();
  ClearBackground(RAYWHITE);

  BeginMode2D(m_camera);
  for (Circle a : m_point_list) {
    std::cout << "Drawing Circle on: " << a.center.x << ' ' << a.center.y << '\n';
    DrawCircle(a.center.x, a.center.y, a.radius, a.color);
  }
  EndMode2D();

  EndDrawing();
}
void GraphRenderer::shutdown() { CloseWindow(); }

void GraphRenderer::add_point(int x, int y) {
  m_point_list.push_back(
      (Circle){{static_cast<float>(x), static_cast<float>(y)}, 3, RED});
}
