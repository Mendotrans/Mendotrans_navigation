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
    InitWindow(800, 450, "raylib example - basic window");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
