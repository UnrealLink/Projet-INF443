#include "loader.hpp"

using namespace vcl;


mesh load_murene(const std::string& filename)
{
    mesh m = mesh_load_file_obj(filename);
    vec3 v1, v2;
    bbox(m, v1, v2);
    for(vec3& v : m.position)
    {
        v.z = -(v.z - v2.z);
        m.texture_uv.push_back(vec2(v.x + v.y, v.z));
    }
    for(index3& c: m.connectivity)
    {
        c = index3{c[0], c[2], c[1]};
    }
    return m;
}

vcl::mesh load_requin(const std::string& filename)
{
    mesh m = mesh_load_file_obj(filename);
    vec3 v1, v2;
    bbox(m, v1, v2);
    for(vec3& v : m.position)
    {
        v.z = -(v.z - v2.z);
        m.texture_uv.push_back(vec2(v.x + v.y, v.z));
    }
    for(index3& c: m.connectivity)
    {
        c = index3{c[0], c[2], c[1]};
    }
    return m;
}


void bbox(const vcl::mesh& m, vcl::vec3& min, vcl::vec3& max)
{
    min = max = m.position[0];
    for(vec3 v : m.position)
    {
        if(v.x < min.x)
            min.x = v.x;
        if(v.y < min.y)
            min.y = v.y;
        if(v.z < min.z)
            min.z = v.z;
        if(v.x > max.x)
            max.x = v.x;
        if(v.y > max.y)
            max.y = v.y;
        if(v.z > max.z)
            max.z = v.z;
    }
}
