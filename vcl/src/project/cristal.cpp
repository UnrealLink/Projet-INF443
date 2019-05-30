#include "cristal.hpp"

mesh create_cristal_branch(float length, float radius, mat3 rot){
    mesh branch;

    for (int i=0; i<5; i++){
        branch.position.push_back(rot*vec3(radius*std::cos(std::atan(1)*8*i/5), radius*std::sin(std::atan(1)*8*i/5), 0));
        branch.texture_uv.push_back(vec2(i*radius, 0));
        branch.position.push_back(rot*vec3(radius*std::cos(std::atan(1)*8*i/5), radius*std::sin(std::atan(1)*8*i/5), length*4/5));
        branch.texture_uv.push_back(vec2(i*radius, length*4/5));
    }
    branch.position.push_back(rot*vec3(0, 0, length));
    branch.texture_uv.push_back(vec2(2.5*radius, length));
    

    for (int i=0; i<5; i++){
        const index3 triangle_1 = {i*2, i*2+1, (i+1)%5*2};
        const index3 triangle_2 = {i*2+1, (i+1)%5*2, (i+1)%5*2+1};
        const index3 triangle_3 = {i*2+1, (i+1)%5*2+1, branch.position.size()-1};
        branch.connectivity.push_back(triangle_1);
        branch.connectivity.push_back(triangle_2);
        branch.connectivity.push_back(triangle_3);
    }

    return branch;
}


mesh create_cristal() {
    mesh cristal;
    for (int i=0; i<rand()%5+4; i++) {
        mat3 rotx(1, 0, 0, 0, std::cos((float)i), -std::sin((float)i), 0, std::sin((float)i), std::cos((float)i));
        mat3 roty(std::cos((float)i), 0, std::sin((float)i), 0, 1, 0, -std::sin((float)i), 0, std::cos((float)i));
        mat3 rotz(std::cos((float)i), -std::sin((float)i), 0, std::sin((float)i), std::cos((float)i), 0, 0, 0, 1);
        cristal.push_back(create_cristal_branch((i%2)/2+1, 0.1, rotx*roty*rotz));
    }

    return cristal;
}
