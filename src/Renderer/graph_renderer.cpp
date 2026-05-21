#include "graph_renderer.h"
#include "raylib.h"
#include "raymath.h"
#include "renderer_data.h"
#include <cassert>
#include <cmath>
#include <iostream>

constexpr int MOVING_SPEED = 5;

static Color highway_color(HighwayType type) {
  switch (type) {
  case HighwayType::motorway:
  case HighwayType::motorway_link:
    return RED;
  case HighwayType::trunk:
  case HighwayType::trunk_link:
    return ORANGE;
  case HighwayType::primary:
  case HighwayType::primary_link:
    return GOLD;
  case HighwayType::secondary:
  case HighwayType::secondary_link:
    return YELLOW;
  case HighwayType::tertiary:
  case HighwayType::tertiary_link:
    return BEIGE;
  case HighwayType::residential:
  case HighwayType::residential_link:
    return BROWN;
  case HighwayType::unclassified:
  default:
    return GRAY;
  }
}

static Rectangle zoom_rect(Camera2D &camera) {
  float w = GetScreenWidth() / camera.zoom;
  float h = GetScreenHeight() / camera.zoom;
  return {camera.target.x - (camera.offset.x / camera.zoom),
          camera.target.y - (camera.offset.y / camera.zoom), w, h};
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

Vector2 GraphRenderer::project(double lat, double lon) const {
  constexpr double SCALE = 100000.0;
  float x = static_cast<float>((lon - mp_renderer_data->ref_lon) * SCALE);
  float y = static_cast<float>(-(lat - mp_renderer_data->ref_lat) * SCALE);
  return {x, y};
}

void GraphRenderer::update() {
  manage_movement();
  BeginDrawing();
  ClearBackground(Color(0, 0, 0, 1));
  BeginMode2D(m_camera);

  Rectangle world_view = zoom_rect(m_camera);
  {
    std::lock_guard<std::mutex> lock(mp_renderer_data->data_mtx);

    for (const GeoEdge &e : mp_renderer_data->edges) {
      Vector2 a = project(e.lat1, e.lon1);
      Vector2 b = project(e.lat2, e.lon2);
      if (CheckCollisionPointRec(a, world_view) ||
          CheckCollisionPointRec(b, world_view)) {
        Color c =
            mp_renderer_data->highway_colors ? highway_color(e.type) : GRAY;
        DrawLineEx(a, b, e.thickness, c);
      }
    }

    if (mp_renderer_data->render_nodes) {
      for (const GeoPoint &p : mp_renderer_data->points) {
        Vector2 pos = project(p.lat, p.lon);
        if (CheckCollisionPointRec(pos, world_view)) {
          if (p.radius * m_camera.zoom > 0.5f) {
            // std::cout << "Rendering node: " << pos.x << ' ' << pos.y << '\n';
            DrawCircle(pos.x, pos.y, p.radius, p.color);
          }
        }
      }
    }

    if (mp_renderer_data->render_stops) {
      for (const GeoPoint &p : mp_renderer_data->stops) {
        Vector2 pos = project(p.lat, p.lon);
        std::cout << "Looping stop: " << pos.x << ' ' << pos.y << ' '
                  << p.radius << '\n';
        if (CheckCollisionPointRec(pos, world_view)) {
          if (p.radius * m_camera.zoom > 0.5f) {
            std::cout << "Drawing stop: " << pos.x << ' ' << pos.y << ' '
                      << p.radius << '\n';
            DrawCircle(pos.x, pos.y, p.radius, p.color);
          }
        }
      }
    }
  }

  EndMode2D();
  DrawFPS(10, 10);
  if (!mp_renderer_data->loading_done) {
    constexpr const char *label = "Loading OSM Data...";
    DrawText(label, GetScreenWidth() - MeasureText(label, 20) - 10, 10, 20,
             DARKGRAY);
  }
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
    m_camera.zoom = Clamp(m_camera.zoom, 0.01f, 100.0f);

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
