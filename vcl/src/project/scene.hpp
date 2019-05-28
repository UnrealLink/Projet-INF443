#ifndef SCENE_HPP
#define SCENE_HPP

#include "../vcl/vcl.hpp"
#include "../helper_scene.hpp"
#include "light.hpp"
#include "fish.hpp"
#include "cavern.hpp"
#include "loader.hpp"

#include <string>
#include <map>
#include <vector>

struct gui_scene_structure
{
    bool wireframe;
    bool skybox = true;
};


struct scene_project
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

    void set_lights(GLuint shader, scene_structure& scene);

    void set_gui();
    void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    void mouse_move(scene_structure& scene, GLFWwindow* window);

    // visual representation of a surface
    vcl::mesh_drawable terrain;
    Cavern cavern;
    GLuint texture_cavern, texture_terrain;
    float distance_display_cavern;

    fishexp mur;
    fishexp requin;

    vcl::mesh_drawable skybox;
    GLuint texture_skybox;
    void display_skybox(std::map<std::string,GLuint>& shaders, scene_structure& scene);

    gui_scene_structure gui_scene;

    vcl::timer_interval_reversing timer;
};

typedef struct scene_project scene_project;

#endif // SCENE_HPP
