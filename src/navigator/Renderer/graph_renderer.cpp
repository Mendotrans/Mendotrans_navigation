#include "graph_renderer.h"
#include "raylib.h"
#include "renderer_data.h"
#include <cmath>

constexpr int MOVING_SPEED = 5;

static Rectangle zoom_curl(Camera2D &camera) {
  float width = GetScreenWidth() / camera.zoom;
  float height = GetScreenHeight() / camera.zoom;

  return {camera.target.x - (camera.offset.x / camera.zoom),
          camera.target.y - (camera.offset.y / camera.zoom), width, height};
}

GraphRenderer::GraphRenderer() {}
GraphRenderer::~GraphRenderer() {}

void GraphRenderer::init(GraphRendererArgs *args) {
  InitWindow(args->width, args->height, args->title);
  SetTargetFPS(args->target_fps);

  m_viewing_point = {args->default_view_x, args->default_view_y};

  m_camera.target = {args->default_view_x, args->default_view_y};
  m_camera.offset = {args->width / 2.0f, args->height / 2.0f};
  m_camera.zoom = 1.0f;
  m_camera.rotation = 0.0f;
}

void GraphRenderer::update() {
  manage_movement();
  BeginDrawing();
  ClearBackground(RAYWHITE);
  BeginMode2D(m_camera);

  Rectangle world_view = zoom_curl(m_camera);

  for (const Edge &e : mp_renderer_data->edges) {
    if (CheckCollisionPointRec(e.start, world_view) ||
        CheckCollisionPointRec(e.end, world_view)) {
      DrawLineEx(e.start, e.end, e.thickness, e.color);
    }
  }

  for (const Circle &a : mp_renderer_data->points) {
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

void GraphRenderer::begin_render(RendererData *render_data) {
  mp_renderer_data = render_data;

  while (!WindowShouldClose()) {
    update();
  }

  shutdown();
}
