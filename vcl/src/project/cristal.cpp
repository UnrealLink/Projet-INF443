#include "cristal.hpp"

mesh create_cristal_branch(float length, float radius){
    mesh branch;

    for (int i=0; i<5; i++){
        branch.position.push_back(vec3(radius*std::cos(std::atan(1)*4*i/5), radius*std::sin(std::atan(1)*4*i/5), 0));
        branch.texture_uv.push_back(vec2(i*radius, 0));
        branch.position.push_back(vec3(radius*std::cos(std::atan(1)*4*i/5), radius*std::sin(std::atan(1)*4*i/5), length*4/5));
        branch.texture_uv.push_back(vec2(i*radius, length*4/5));
    branch.position.push_back(vec3(0, 0, length));
    branch.texture_uv.push_back(vec2(2.5*radius, length));
    }

    for (int i=0; i<5; i++){
        const index3 triangle_1 = {i*2, i*2+1, (i+1)%5*2};
        const index3 triangle_2 = {i*2, i*2+1, (i+1)%5*2+1};
        const index3 triangle_3 = {i*2, (i+1)%5*2, branch.position.size()-1};
        branch.connectivity.push_back(triangle_1);
        branch.connectivity.push_back(triangle_2);
        branch.connectivity.push_back(triangle_3);
    }

    return branch;
}


mesh create_cristal() {
    mesh cristal;
    for (int i=0; i<5; i++) {
        std::cos(perlin(i))
    }

    return cristal;
}
