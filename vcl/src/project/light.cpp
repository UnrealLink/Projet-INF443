#include "light.hpp"

using namespace vcl;

void uniform(GLuint shader, const std::string& name, const light& value)
{
    uniform(shader, name + ".position", value.position);
    uniform(shader, name + ".color", value.color);
    uniform(shader, name + ".radius", value.radius);
    uniform(shader, name + ".strength", value.strength);
}

void uniform(GLuint shader, const std::string& name, const spot& value)
{
    uniform(shader, name + ".position", value.position);
    uniform(shader, name + ".dir", value.dir);
    uniform(shader, name + ".color", value.color);
    uniform(shader, name + ".inner", value.inner);
    uniform(shader, name + ".outter", value.outter);
    uniform(shader, name + ".strength", value.strength);
    uniform(shader, name + ".radius", value.radius);
}
