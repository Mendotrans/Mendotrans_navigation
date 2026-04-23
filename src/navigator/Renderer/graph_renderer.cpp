#include "graph_renderer.h"
#include "raylib.h"
#include <cmath>

constexpr int MOVING_SPEED = 5;

GraphRenderer::GraphRenderer() {}
GraphRenderer::~GraphRenderer() {}

void GraphRenderer::init(int width, int height, const char *title,
                         const float default_view_x, const float default_view_y,
                         const int target_fps) {
  InitWindow(width, height, title);
  SetTargetFPS(target_fps);

  m_viewing_point = {default_view_x, default_view_y};

  m_camera.target = {default_view_x, default_view_y};
  m_camera.offset = {width / 2.0f, height / 2.0f};
  m_camera.zoom = 1.0f;
  m_camera.rotation = 0.0f;
}

void GraphRenderer::update() {
  // HACK: This should be a diferent system
  m_camera.target = m_viewing_point;

  if (IsKeyDown(KEY_A))
    m_viewing_point.x = m_viewing_point.x - MOVING_SPEED;
  if (IsKeyDown(KEY_D))
    m_viewing_point.x = m_viewing_point.x + MOVING_SPEED;
  if (IsKeyDown(KEY_W))
    m_viewing_point.y = m_viewing_point.y - MOVING_SPEED;
  if (IsKeyDown(KEY_S))
    m_viewing_point.y = m_viewing_point.y + MOVING_SPEED;

  m_camera.zoom = expf(logf(m_camera.zoom) + GetMouseWheelMove() * 0.1f);
  if (m_camera.zoom > 10.0f)
    m_camera.zoom = 100.0f;
  else if (m_camera.zoom < 0.001f)
    m_camera.zoom = 0.01f;

  BeginDrawing();
  ClearBackground(RAYWHITE);

  BeginMode2D(m_camera);

  for (const Edge &e : m_edge_list) {
    DrawLineEx(e.start, e.end, e.thickness, e.color);
  }

  for (const Circle &a : m_point_list) {
    DrawCircle(a.center.x, a.center.y, a.radius, a.color);
  }
  EndMode2D();

  EndDrawing();
}

void GraphRenderer::shutdown() { CloseWindow(); }

void GraphRenderer::add_point(float x, float y) {
  m_point_list.push_back({{x, y}, 2.0f, RED});
}

void GraphRenderer::add_edge(float x1, float y1, float x2, float y2) {
  m_edge_list.push_back({{x1, y1}, {x2, y2}, DARKGRAY, 1.0f});
}
