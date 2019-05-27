#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include <string>
#include <math.h>

#ifdef INF443_EXAMPLE_ANIMATED_HIERARCHY


struct gui_scene_structure
{
    bool wireframe   = false;
};

struct scene_exercise : base_scene_exercise
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    void setForm(float yq, float yt, float zMax);

    vcl::mesh_drawable_hierarchy hierarchy;

    vcl::mesh_drawable ground;

    gui_scene_structure gui_scene;

    vcl::timer_interval timer;
    const size_t Nb_corps = 15;
    const float r_body = 0.15f, separation = 1.f;
    float last_draw = 0.f;
    vcl::vec2 pos = {0.f,0.f};
};

#endif


