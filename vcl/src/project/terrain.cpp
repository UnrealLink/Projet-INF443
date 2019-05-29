#include "terrain.hpp"

// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluateTerrain_z(float u, float v)
{
    const std::vector<vec2> pi = {vec2(0,0),vec2(0.5,0.5),vec2(0.2,0.7),vec2(0.8,0.7)};
    const std::vector<float> hi = {3,-1.5,1,2}, sigmai={0.5,0.15,0.2,0.2};
    vec2 p = {u, v};

    float z = 0;

    for(size_t i = 0; i < pi.size(); i++)
        z += hi[i]*std::exp( -dot(p-pi[i], p-pi[i])/sigmai[i]/sigmai[i] );

    return z + (perlin(3*u, 3*v, 7, 0.4)-0.5f)*0.8f;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluateTerrain(float u, float v, float terrain_size)
{
    const float x = terrain_size*(u-0.5f);
    const float y = terrain_size*(v-0.5f);
    const float z = evaluateTerrain_z(u,v);

    return {x,y,z};
}

vec2 evaluateTerrainTexture(float u, float v, int repeat)
{
    const float x = repeat*u;
    const float y = repeat*v;

    return {x, y};
}

// Generate terrain meshmesh
mesh createTerrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 600;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            terrain.position[kv+N*ku] = evaluateTerrain(u,v, 20.f);
            terrain.texture_uv[kv+N*ku] = evaluateTerrainTexture(u, v, 10);
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}
