#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "../vcl/core/math/math.hpp"
#include "../vcl/core/opengl/uniform/uniform.hpp"


/** this header is used to define the typed used by the underwater shader and send them, as uniform parameter, to the shader*/

struct light
{
    vcl::vec3 position; //donne dans le repere du monde
    vcl::vec3 color;
    float radius; //rayon de la zone lumineuse, dans laquelle il n'y a pas de decroissance a la source
    float rangeDecr;  //vitesse de decroissance de la portee or rayon
    float strength;
};

struct spot
{
    vcl::vec3 position;
    vcl::vec3 dir;
    vcl::vec3 color;
    float inner;
    float outter;
    float strength;
    float radius;
};

void uniform(GLuint shader, const std::string& name, const light& value);
void uniform(GLuint shader, const std::string& name, const spot& value);

const light switchedOffLight = light{vcl::vec3(0.f, 0.f, 0.f), vcl::vec3(1.f, 1.f, 1.f), 1.f, 0.f, 1.f};
const spot switchedOffSpot = spot{vcl::vec3(0.f, 0.f, 0.f), vcl::vec3(0.f, 0.f, 1.f), vcl::vec3(1.f, 1.f, 1.f), 0.f, 0.f, 0.f};



#endif // LIGHT_HPP
