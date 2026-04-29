#pragma once

#include "raylib.h"
enum class HighwayType {
  motorway,
  trunk,
  primary,
  secondary,
  tertiary,
  unclassified,
  residential
};

struct StreetEdge {
  StreetEdge(float weight, HighwayType type, Color color = GRAY)
      : weight(weight), type(type) {}
  float weight;
  HighwayType type;
};
