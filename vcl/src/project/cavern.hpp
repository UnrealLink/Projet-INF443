#ifndef CAVERN_HPP
#define CAVERN_HPP

#include "../vcl/vcl.hpp"
#include "../helper_scene.hpp"
#include "cristal.hpp"

#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace vcl;

struct Chunk{
    bool initialized = false;
    vec3 center;
    float size;
    vec3 cristal_pos;
    mesh_drawable_hierarchy terrain;
};

Chunk createChunk(vec3 origin, int nb_cubes, float cube_size, float f(vec3));

struct ChunkIndex{
    int i;
    int j;
    int k;
};

float perlin3D(vec3 p);
float isovalue(vec3 p);

struct Cavern {
    std::vector<Chunk> chunks;
    vec3 origin;
    int nb_chunks;
    int nb_cubes;
    float cube_size;
    ChunkIndex current_idx;
    float (*f)(vec3);
    drawable_uniform uniform_parameter;
    void draw(GLuint shader, const camera_scene& camera, float distance);
    void addChunk(ChunkIndex idx);
    std::vector<ChunkIndex> getChunksAround(vec3 p, float distance);

};

Cavern createCavern(vec3 origin, int nb_chunks, int nb_cubes, float cube_size, float f(vec3)=isovalue);

#endif // CAVERN_HPP
