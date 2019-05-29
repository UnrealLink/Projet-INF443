#pragma once

#include "../../core/core.hpp"
#include <map>

using namespace vcl;


struct Cube{
    vec3 p [8];
    float val[8];
};

mesh create_chunk(vec3 origin, const int nb_cubes, const float cube_size, float f(vec3), float isolevel);
int compute_cube(Cube cube, float isolevel, mesh &chunk, std::map<vec3, int> &coordsToIdx, int &last, int edgeTable[256], int triTable[256][16]);
vec3 interpolate(float isolevel, vec3 p1, vec3 p2, float val1, float val2);
vec2 evaluateTexture(vec3 p);
