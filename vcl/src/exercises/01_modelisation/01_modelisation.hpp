#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include <random>
#include <chrono>

#ifdef INF443_01_MODELISATION

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe;
};

using index4 = std::array<unsigned int, 4>;

struct tree
{
    vcl::mesh_drawable tronc, feuillage;
    void put(vcl::vec3 v);
    void draw(GLuint shader, const vcl::camera_scene &camera);
};

struct quadMesh
{
    std::vector<vcl::vec3> position;
    std::vector<index4> connectivity;

    vcl::mesh toMesh();
    static quadMesh createPlan(vcl::vec3 pos = {0.f,0.f,0.f}, vcl::vec3 u = {1.f, 0.f, 0.f}, vcl::vec3 v = {0.f, 1.f, 0.f});
};

struct particle_element
{
    vcl::vec3 p; // Position
    vcl::vec3 v; // Speed
};

struct scene_exercise : base_scene_exercise
{

    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void setForm(float yt, float yq, float zMax);

    void set_gui();

    // visual representation of a surface
    vcl::mesh_drawable terrain;
    GLuint texture_terrain;

    vcl::mesh_drawable cube;
    std::vector<vcl::vec3> tree_position;
    tree arbre;

    vcl::mesh_drawable billboard;
    GLuint texture_herbe, texture_fleur;
    std::vector<vcl::vec3> herbe_position, fleur_position;

    vcl::mesh_drawable skybox;
    GLuint texture_skybox;

    gui_scene_structure gui_scene;

    vcl::mesh_drawable_hierarchy hierarchy;
    vcl::timer_interval timer;
    const float r_body = 0.05f, separation = 1.f;
    float last_draw = 0.f;
    vcl::vec2 pos = {0.f,0.f};
    static const int Nb_corps = 15;
    particle_element parts[Nb_corps];
    float L0;

};

#endif












