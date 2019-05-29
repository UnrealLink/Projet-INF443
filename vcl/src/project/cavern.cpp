#include "cavern.hpp"

void Cavern::draw(GLuint shader, const camera_scene& camera, float distance){

    // set uniform parameters
    opengl_debug();
    GLint current_shader = 0; opengl_debug();
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();

    if(shader!=GLuint(current_shader))
        glUseProgram(shader); opengl_debug();



    uniform(shader, "rotation", uniform_parameter.rotation); opengl_debug();
    uniform(shader, "translation", uniform_parameter.translation); opengl_debug();
    uniform(shader, "color", uniform_parameter.color); opengl_debug();
    uniform(shader, "scaling", uniform_parameter.scaling); opengl_debug();
    uniform(shader, "scaling_axis", uniform_parameter.scaling_axis); opengl_debug();

    uniform(shader,"perspective",camera.perspective.matrix()); opengl_debug();
    uniform(shader,"view",camera.view_matrix()); opengl_debug();
    uniform(shader,"camera_position",camera.camera_position()); opengl_debug();

    uniform(shader, "ambiant", uniform_parameter.shading.ambiant); opengl_debug();
    uniform(shader, "diffuse", uniform_parameter.shading.diffuse); opengl_debug();
    uniform(shader, "specular", uniform_parameter.shading.specular); opengl_debug();

    // camera orientation
    vec3 dir = camera.orientation*vec3(0.f, 0.f, -1.f);
    // choosing which chunks to draw
    for (int i=0; i<chunks.size(); i++){
        /*if (norm((chunks[i].center-camera.camera_position())) < distance && dot(dir, chunks[i].center-camera.camera_position()) > 0) {
            vcl::draw(chunks[i].terrain);
        }*/
        vcl::draw(chunks[i].terrain);
    }
}

Chunk Cavern::createChunk(vec3 origin, int chunk_size, float cube_size, float f(vec3), float isolevel){
    Chunk chunk;
    chunk.terrain = (mesh_gpu)create_chunk(origin, chunk_size, cube_size, f, isolevel);;
    chunk.center = origin + vec3(chunk_size*cube_size/2, chunk_size*cube_size/2, chunk_size*cube_size/2);
    chunk.size = chunk_size*cube_size;
    return chunk;
}

Cavern createCavern(vec3 origin, int nb_chunks, int chunk_size, float cube_size, float f(vec3), float isolevel){
    Cavern cavern;
    Chunk chunk;
    for (int i=0; i<nb_chunks; i++){
        for (int j=0; j<nb_chunks; j++){
            for (int k=0; k<nb_chunks; k++){
                chunk = cavern.createChunk(origin+vec3(i*chunk_size*cube_size, j*chunk_size*cube_size, k*chunk_size*cube_size), chunk_size, cube_size, f, isolevel);
                cavern.chunks.push_back(chunk);
            }
        }
    }
    return cavern;
}

float perlin3D(vec3 p){
    if (norm(p) < 50 && p.z < 0) {
        return perlin(0.05f*p.x, 0.05f*p.y, 0.05f*p.z, 3, 0.1f);
    } else {
        return 0.4;
    }
}
