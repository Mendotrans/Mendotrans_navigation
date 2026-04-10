#include "navigator/reader.h"
#include "osmium/osm/location.hpp"
#include <iostream>
#include "raylib.h"

int main0() {
  OSM_Reader reader(osmium::io::File("/home/ironowl/Downloads/map(1).osm"));
	reader.apply_reader();
	return 0;
}


int main(void)
{
    InitWindow(1000, 1000, "raylib example - basic window");
		SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window!", 0, 0, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
