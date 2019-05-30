#ifndef CRISTAL_HPP
#define CRISTAL_HPP

#include "../vcl/vcl.hpp"
#include "../helper_scene.hpp"

#include <cmath>
#include <cstdlib>

using namespace vcl;

mesh create_cristal();
mesh create_cristal_branch(float length, float radius, mat3 rot=vcl::mat3::identity());

#endif // CRISTAL_HPP
