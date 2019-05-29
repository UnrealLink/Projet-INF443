#include "fish.hpp"
#include "vcl/core/opengl/opengl.hpp"


using namespace vcl;

fish::fish() : mesh_drawable(), mesh_length(0.f), embossMinMap(0.f, 0.f, 0.f), embossMaxMap(1.f, 1.f, 1.f)
{

}

fish::fish(const vcl::mesh& data) : mesh_drawable(data), mesh_length(0.f), embossMinMap(0.f, 0.f, 0.f), embossMaxMap(1.f, 1.f, 1.f)
{
    for(vec3 v : data.position)
    {
        if(v.z > mesh_length)
            mesh_length = v.z;
    }
}

/** Display the mesh with and send the uniform parameters */
void fish::draw(GLuint shader, const vcl::camera_scene& camera)
{
    opengl_debug();
    GLint current_shader = 0; opengl_debug();
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();

    if(shader!=GLuint(current_shader))
        glUseProgram(shader); opengl_debug();

    uniform(shader, "mesh_length", mesh_length);
    uniform(shader, "embossMinMap", embossMinMap);
    uniform(shader, "embossMaxMap", embossMaxMap);

    float step = mesh_length/float(ONDULANTLEN);
    vec3 p[ONDULANTLEN+4];
    vec3 last, cur;
    float cuml = -2*step, t = -2*step;
    for(int i = 0; i < ONDULANTLEN+4; i++)
    {
        while (cuml < step*(i-2)) //Pour conserver la taille des objets, on calcule la longueur le long de la courbe
        {
            t += STEPPRE;
            cur = func(t);
            cuml += norm(last-cur);
            last = cur;
        }
        p[i] = cur;
    }
    const GLint location = glGetUniformLocation(shader, "axe"); opengl_debug();
    glUniform3fv(location, ONDULANTLEN+4, (float *) &p[0]); opengl_debug();

    mesh_drawable::draw(shader, camera);
}

fishexp::fishexp() : fish (), ampl(0.f), start(0.f)
{

}

fishexp::fishexp(const vcl::mesh& data) : fish (data), ampl(0.f), start(0.f)
{

}

vec3 fishexp::func(float z)
{
    if( z/mesh_length < start)
        return vec3(0.f, 0.f, z);
    return vec3(ampl*(z/mesh_length - start)*std::exp(4*(z-mesh_length)), 0.f, z);
}












