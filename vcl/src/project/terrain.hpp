#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "../vcl/vcl.hpp"
#include "../helper_scene.hpp"

float evaluateTerrain_z(float u, float v);
vec3 evaluateTerrain(float u, float v, float terrain_size);
vec2 evaluateTerrainTexture(float u, float v, int repeat);
mesh createTerrain();

#endif // TERRAIN_HPP
