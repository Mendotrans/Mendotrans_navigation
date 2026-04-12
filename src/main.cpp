#include "navigator/Reader/reader.h"
#include "navigator/Routing_Graph/graph_builder.h"
#include "navigator/Routing_Graph/routing_graph.h"
#include "raylib.h"

int main() {
  Routing_Graph routing_graph;
  OSMReader<GraphBuilder> reader(
      osmium::io::File("/home/ironowl/Downloads/map(1).osm"),
      GraphBuilder(&routing_graph));
  reader.apply_reader();
  return 0;
}

int main0(void) {
  InitWindow(1000, 1000, "raylib example - basic window");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 0, 0, 20, LIGHTGRAY);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
