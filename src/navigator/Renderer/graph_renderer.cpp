#include "graph_renderer.h"
#include "raylib.h"
#include <cmath>

constexpr int MOVING_SPEED = 5;

static Rectangle zoom_curl(Camera2D &camera) {
  // Inside your update() or a helper function
  float width = GetScreenWidth() / camera.zoom;
  float height = GetScreenHeight() / camera.zoom;

  return {camera.target.x - (camera.offset.x / camera.zoom),
          camera.target.y - (camera.offset.y / camera.zoom), width, height};
}

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
  manage_movement();
  BeginDrawing();
  ClearBackground(RAYWHITE);
  BeginMode2D(m_camera);

  Rectangle world_view = zoom_curl(m_camera);

  for (const Edge &e : m_edge_list) {
    if (CheckCollisionPointRec(e.start, world_view) ||
        CheckCollisionPointRec(e.end, world_view)) {
      DrawLineEx(e.start, e.end, e.thickness, e.color);
    }
  }

  for (const Circle &a : m_point_list) {
    if (CheckCollisionPointRec(a.center, world_view)) {
      if (a.radius * m_camera.zoom > 0.5f) {
        DrawCircle(a.center.x, a.center.y, a.radius, a.color);
      }
    }
  }

  EndMode2D();
  DrawFPS(10, 10);
  EndDrawing();
}

void GraphRenderer::manage_movement() {
  float dt = GetFrameTime();

  float currentSpeed = (MOVING_SPEED * 100.0f) / m_camera.zoom;

  if (IsKeyDown(KEY_A))
    m_viewing_point.x -= currentSpeed * dt;
  if (IsKeyDown(KEY_D))
    m_viewing_point.x += currentSpeed * dt;
  if (IsKeyDown(KEY_W))
    m_viewing_point.y -= currentSpeed * dt;
  if (IsKeyDown(KEY_S))
    m_viewing_point.y += currentSpeed * dt;

  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_camera);

    float scaleFactor = 1.0f + (0.15f * fabsf(wheel));
    if (wheel > 0)
      m_camera.zoom *= scaleFactor;
    else
      m_camera.zoom /= scaleFactor;

    if (m_camera.zoom > 100.0f)
      m_camera.zoom = 100.0f;
    if (m_camera.zoom < 0.01f)
      m_camera.zoom = 0.01f;

    Vector2 mouseWorldPosAfter =
        GetScreenToWorld2D(GetMousePosition(), m_camera);

    m_viewing_point.x += (mouseWorldPos.x - mouseWorldPosAfter.x);
    m_viewing_point.y += (mouseWorldPos.y - mouseWorldPosAfter.y);
  }

  m_camera.target = m_viewing_point;
}

void GraphRenderer::shutdown() { CloseWindow(); }

void GraphRenderer::add_point(float x, float y) {
  m_point_list.push_back({{x, y}, 2.0f, RED});
}

void GraphRenderer::add_edge(float x1, float y1, float x2, float y2) {
  m_edge_list.push_back({{x1, y1}, {x2, y2}, DARKGRAY, 1.0f});
}
