#ifndef CAVERN_HPP
#define CAVERN_HPP

#include "../vcl/vcl.hpp"
#include "../helper_scene.hpp"

#include <string>
#include <map>
#include <vector>

struct Chunk{
    vec3 center;
    float size;
    vcl::mesh_gpu terrain;
};

float perlin3D(vec3 p);

struct Cavern {

    std::vector<Chunk> chunks;
    drawable_uniform uniform_parameter;
    void draw(GLuint shader, const camera_scene& camera, float distance);
    Chunk createChunk(vec3 origin, int chunk_size, float cube_size, float f(vec3), float isolevel);

};

Cavern createCavern(vec3 origin, int nb_chunks, int chunk_size, float cube_size, float f(vec3)=perlin3D, float isolevel=0.5);

#endif // CAVERN_HPP
