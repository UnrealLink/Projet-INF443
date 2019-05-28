#ifndef LOADER_HPP
#define LOADER_HPP

#include "../vcl/core/core.hpp"

#include <string>

vcl::mesh load_murene(const std::string& filename);
vcl::mesh load_requin(const std::string& filename);
void bbox(const vcl::mesh& m, vcl::vec3& min, vcl::vec3& max);

#endif // LOADER_HPP
