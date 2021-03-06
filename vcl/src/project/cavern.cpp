#include "cavern.hpp"

void Cavern::draw(GLuint shader, const scene_structure& scene, GLuint texture_cavern, GLuint texture_cristal, float distance){

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

    mat3 real_rot = uniform_parameter.rotation;
    vec3 real_trans = uniform_parameter.translation;

    // camera orientation
    // vec3 dir = normalize(camera.orientation*vec3(0.f, 0.f, -1.f));
    // choosing which chunks to draw
    /*
    ChunkIndex current_idx;
    float chunk_size = nb_cubes*cube_size;
    current_idx.i = (int)(camera.camera_position().x-origin.x)/chunk_size;
    current_idx.j = (int)(camera.camera_position().y-origin.y)/chunk_size;
    current_idx.k = (int)(camera.camera_position().z-origin.z)/chunk_size;
    if (current_idx.i != current_idx.i || current_idx.j != current_idx.j || current_idx.k != current_idx.k){
        chunks = std::vector<Chunk>(nb_chunks*nb_chunks*nb_chunks);
    }
    */
    std::vector<ChunkIndex> idxs = getChunksAround(scene.camera.camera_position(), distance);
    Chunk current_chunk;
    for (ChunkIndex idx : idxs){
        if (chunks[idx.i*nb_chunks*nb_chunks+idx.j*nb_chunks+idx.k].initialized){
            current_chunk = chunks[idx.i*nb_chunks*nb_chunks+idx.j*nb_chunks+idx.k];
            glBindTexture(GL_TEXTURE_2D, texture_cavern);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,   GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,   GL_REPEAT);
            current_chunk.terrain.mesh_visual("cavern").draw(shader, scene.camera);
            glBindTexture(GL_TEXTURE_2D, scene.texture_white);
            if (current_chunk.cristal){
                glBindTexture(GL_TEXTURE_2D, texture_cristal);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,   GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,   GL_REPEAT);
                current_chunk.terrain.mesh_visual("cristal").draw(shader, scene.camera);
            }
            glBindTexture(GL_TEXTURE_2D, scene.texture_white);
        } else {
            addChunk(idx);Chunk current_chunk = chunks[idx.i*nb_chunks*nb_chunks+idx.j*nb_chunks+idx.k];
            current_chunk = chunks[idx.i*nb_chunks*nb_chunks+idx.j*nb_chunks+idx.k];
            glBindTexture(GL_TEXTURE_2D, texture_cavern);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,   GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,   GL_REPEAT);
            current_chunk.terrain.mesh_visual("cavern").draw(shader, scene.camera);
            glBindTexture(GL_TEXTURE_2D, scene.texture_white);
            if (current_chunk.cristal){
                glBindTexture(GL_TEXTURE_2D, texture_cristal);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,   GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,   GL_REPEAT);
                current_chunk.terrain.mesh_visual("cristal").draw(shader, scene.camera);
            }
            glBindTexture(GL_TEXTURE_2D, scene.texture_white);
        }
    }
}

std::vector<ChunkIndex> Cavern::getChunksAround(vec3 p, float distance){
    std::vector<ChunkIndex> idxs;
    ChunkIndex idx;
    float chunk_size = nb_cubes*cube_size;
    idx.i = (int)(p.x-origin.x)/chunk_size;
    idx.j = (int)(p.y-origin.y)/chunk_size;
    idx.k = (int)(p.z-origin.z)/chunk_size;
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
    srand(time(0));
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
    //creating terrain
    chunk.initialized = true;
    mesh terrain = create_chunk(origin, nb_cubes, cube_size, f, 0.);
    chunk.terrain.add_element(terrain, "cavern", "root");
    //adding cristal
    if ((terrain.connectivity.size() > 0) && (rand()%3 == 0)) {
        index3 idx = terrain.connectivity[rand()%terrain.connectivity.size()];
        const vec3& p0 = terrain.position[idx[0]];
        const vec3& p1 = terrain.position[idx[1]];
        const vec3& p2 = terrain.position[idx[2]];
        chunk.cristal_pos = (p0 + p1 + p2)/3;
        const vec3& p10 = normalize(p1-p0);
        const vec3& p20 = normalize(p2-p0);
        const vec3& n = normalize( cross(p10,p20) );
        const vec3& p20bis = normalize(cross(p10,n));
        /*mat3 rot(p10.x, p10.y, p10.z,
                -p20bis.x, -p20bis.y, -p20bis.z,
                n.x,   n.y,   n.z);*/
        mat3 rot(p10.x, -p20bis.x, n.x,
                 p10.y, -p20bis.y, n.y,
                 p10.z, -p20bis.z, n.z);
        chunk.terrain.add_element(create_cristal(), "cristal", "cavern", chunk.cristal_pos, rot);
        chunk.terrain.mesh_visual("cristal").uniform_parameter.rotation = chunk.terrain.get_rotation_global("cristal");
        chunk.terrain.mesh_visual("cristal").uniform_parameter.translation = chunk.terrain.get_translation_global("cristal");
        chunk.terrain.mesh_visual("cristal").uniform_parameter.color = {0.f, 0.f, 1.f};
        chunk.cristal = true;
    }
    //
    chunk.center = origin + vec3(nb_cubes*cube_size/2, nb_cubes*cube_size/2, nb_cubes*cube_size/2);
    chunk.size = nb_cubes*cube_size;
    return chunk;
}

float perlin3D(vec3 p){
    return perlin(0.05f*p.x, 0.05f*p.y, 0.05f*p.z, 3, 0.1f);
}

float isovalue(vec3 p){
    return perlin(0.05f*p.x, 0.05f*p.y, 0.05f*p.z, 5, 0.1f) - 0.5;
}
