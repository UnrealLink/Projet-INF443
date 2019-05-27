#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"

#ifdef INF443_EXAMPLE_MASS_SPRING

const int Nb_particles = 15;

struct particle_element
{
    vcl::vec3 p; // Position
    vcl::vec3 v; // Speed
};

struct scene_exercise : base_scene_exercise
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    particle_element parts[Nb_particles];
    float L0;


    vcl::mesh_drawable sphere;      // Visual display of particles
    vcl::segments_drawable borders; // Visual display of borders
    vcl::segment_drawable_immediate_mode segment_drawer;

    vcl::timer_event timer;
};






#endif
