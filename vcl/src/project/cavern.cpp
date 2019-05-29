#include "cavern.hpp"

void Cavern::draw(GLuint shader, const camera_scene& camera, float distance){

    // set uniform parameters
    opengl_debug();
    GLint current_shader = 0; opengl_debug();
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();

    if(shader!=GLuint(current_shader))
        glUseProgram(shader); opengl_debug();

    //uniform vec3 embossMinMap = vec3(0.f);
    //uniform vec3 embossMaxMap = vec3(1.f);

    uniform(shader, "embossMinMap", vec3(0.f, 0.f, 0.f));
    uniform(shader, "embossMaxMap", vec3(1.f, 1.f, 1.f));

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
    // vec3 dir = normalize(camera.orientation*vec3(0.f, 0.f, -1.f));
    // choosing which chunks to draw
    /*
    ChunkIndex current_idx;
    float chunk_size = this->nb_cubes*this->cube_size;
    current_idx.i = (int)(camera.camera_position().x-this->origin.x)/chunk_size;
    current_idx.j = (int)(camera.camera_position().y-this->origin.y)/chunk_size;
    current_idx.k = (int)(camera.camera_position().z-this->origin.z)/chunk_size;
    if (current_idx.i != this->current_idx.i || current_idx.j != this->current_idx.j || current_idx.k != this->current_idx.k){
        this->chunks = std::vector<Chunk>(this->nb_chunks*this->nb_chunks*this->nb_chunks);
    }
    */
    std::vector<ChunkIndex> idxs = this->getChunksAround(camera.camera_position(), distance);
    for (ChunkIndex idx : idxs){
        if (chunks[idx.i*this->nb_chunks*this->nb_chunks+idx.j*this->nb_chunks+idx.k].initialized){
            vcl::draw(chunks[idx.i*this->nb_chunks*this->nb_chunks+idx.j*this->nb_chunks+idx.k].terrain);
        } else {
            this->addChunk(idx);
            vcl::draw(chunks[idx.i*this->nb_chunks*this->nb_chunks+idx.j*this->nb_chunks+idx.k].terrain);
        }
    }
}

std::vector<ChunkIndex> Cavern::getChunksAround(vec3 p, float distance){
    std::vector<ChunkIndex> idxs;
    ChunkIndex idx;
    float chunk_size = this->nb_cubes*this->cube_size;
    idx.i = (int)(p.x-this->origin.x)/chunk_size;
    idx.j = (int)(p.y-this->origin.y)/chunk_size;
    idx.k = (int)(p.z-this->origin.z)/chunk_size;
    for (int i2=0; i2<(int)distance/chunk_size; i2++){
        for (int j2=0; j2<(int)distance/chunk_size; j2++) {
            for (int k2=0; k2<(int)distance/chunk_size; k2++){
                idxs.push_back(ChunkIndex{idx.i+i2, idx.j+j2, idx.k+k2});
                idxs.push_back(ChunkIndex{idx.i+i2, idx.j+j2, idx.k-k2});
                idxs.push_back(ChunkIndex{idx.i+i2, idx.j-j2, idx.k+k2});
                idxs.push_back(ChunkIndex{idx.i+i2, idx.j-j2, idx.k-k2});
                idxs.push_back(ChunkIndex{idx.i-i2, idx.j+j2, idx.k+k2});
                idxs.push_back(ChunkIndex{idx.i-i2, idx.j+j2, idx.k-k2});
                idxs.push_back(ChunkIndex{idx.i-i2, idx.j-j2, idx.k+k2});
                idxs.push_back(ChunkIndex{idx.i-i2, idx.j-j2, idx.k-k2});
            }
        }
    }
    return idxs;
}

Cavern createCavern(vec3 origin, int nb_chunks, int nb_cubes, float cube_size, float f(vec3)){
    Cavern cavern;
    cavern.origin = origin-vec3(nb_chunks*nb_cubes*cube_size/2.f, nb_chunks*nb_cubes*cube_size/2.f, nb_chunks*nb_cubes*cube_size/2.f);
    cavern.nb_chunks = nb_chunks;
    cavern.nb_cubes = nb_cubes;
    cavern.cube_size = cube_size;
    cavern.chunks.resize(nb_chunks*nb_chunks*nb_chunks);
    cavern.f = f;
    ChunkIndex current_idx;
    float chunk_size = nb_cubes*cube_size;
    current_idx.i = (int)(origin.x-cavern.origin.x)/chunk_size;
    current_idx.j = (int)(origin.y-cavern.origin.y)/chunk_size;
    current_idx.k = (int)(origin.z-cavern.origin.z)/chunk_size;
    cavern.current_idx = current_idx;
    /*
    Chunk chunk;
    for (int i=0; i<nb_chunks; i++){
        for (int j=0; j<nb_chunks; j++){
            for (int k=0; k<nb_chunks; k++){
                chunk = cavern.createChunk(origin+vec3(i*nb_cubes*cube_size, j*nb_cubes*cube_size, k*nb_cubes*cube_size), nb_cubes, cube_size, f);
                cavern.chunks[i*nb_chunks*nb_chunks+j*nb_chunks+k] = chunk;
            }
        }
    }
    */
    return cavern;
}

void Cavern::addChunk(ChunkIndex idx){
    Chunk chunk;
    float chunk_size = nb_cubes*cube_size;
    chunk = createChunk(origin+vec3(idx.i*chunk_size, idx.j*chunk_size, idx.k*chunk_size), nb_cubes, cube_size, f);
    chunks[idx.i*nb_chunks*nb_chunks+idx.j*nb_chunks+idx.k] = chunk;
}

Chunk createChunk(vec3 origin, int nb_cubes, float cube_size, float f(vec3)){
    Chunk chunk;
    chunk.initialized = true;
    chunk.terrain = (mesh_gpu)create_chunk(origin, nb_cubes, cube_size, f, 0.);;
    chunk.center = origin + vec3(nb_cubes*cube_size/2, nb_cubes*cube_size/2, nb_cubes*cube_size/2);
    chunk.size = nb_cubes*cube_size;
    return chunk;
}

float perlin3D(vec3 p){
    return perlin(0.05f*p.x, 0.05f*p.y, 0.05f*p.z, 3, 0.1f);
}

float isovalue(vec3 p){
    return perlin(0.05f*p.x, 0.05f*p.y, 0.05f*p.z, 3, 0.1f) - 0.5;
}
