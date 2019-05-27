#include "vec3.hpp"

#include <iostream>
#include <cassert>

namespace vcl {

vec3::vec()
    :x(0),y(0),z(0)
{}

vec3::vec(float x_arg, float y_arg, float z_arg)
    :x(x_arg),y(y_arg),z(z_arg)
{}

const float& vec3::operator[](std::size_t index) const
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        std::cerr<<"Error: Try to access vec3["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}
float& vec3::operator[](std::size_t index)
{
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        std::cerr<<"Error: Try to access vec3["<<index<<"]"<<std::endl;
        assert(false);
    }
	abort();
}

bool vec3::operator<(const vec<3> &right) const
{
        if (x < right.x)
            return true;
        else if (x > right.x)
            return false;

        if (y < right.y)
            return true;
        else if (y > right.y)
            return false;

        if (z < right.z)
            return true;
        else if (z > right.z)
            return false;

        return false;
     }

vec3 cross(const vec3& a,const vec3& b)
{
    return    { a.y*b.z-a.z*b.y,
                a.z*b.x-a.x*b.z,
                a.x*b.y-a.y*b.x };
}

}
