#ifndef FISH_HPP
#define FISH_HPP

#include "../vcl/core/drawable/mesh_drawable/mesh_drawable.hpp"
#include "vcl/external_lib/headers/glad.hpp"

#define ONDULANTLEN 50
#define STEPPRE 0.001f

struct fish : public vcl::mesh_drawable
{
    float mu;

    fish();
    fish(const vcl::mesh& data);

    /** Display the mesh with and send the uniform parameters */
    void draw(GLuint shader, const vcl::camera_scene& camera);

    /** gizes the position of the point 0,0,t */
    virtual vcl::vec3 func(float z) = 0;


protected:
    float mesh_length;
};

struct fishexp : public fish
{
    fishexp();
    fishexp(const vcl::mesh& data);
    virtual vcl::vec3 func(float z);

    float ampl, start;
};


#endif // FISH_HPP
