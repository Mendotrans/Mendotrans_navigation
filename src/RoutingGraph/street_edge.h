#pragma once

enum class HighwayType {
  motorway,
  motorway_link,
  trunk,
  trunk_link,
  primary,
  primary_link,
  secondary,
  secondary_link,
  tertiary,
  tertiary_link,
  residential,
  residential_link,
  unclassified,
};

struct StreetEdge {
  StreetEdge(float weight, HighwayType type) : weight(weight), type(type) {}
  float weight;
  HighwayType type;
};
