#include "scene.hpp"

using namespace vcl;

//mesh createMurene();


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_project::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    cavern = createCavern(vec3(0, 0, 0), 5, 20, 1.);
    cavern.uniform_parameter.color = {1.f, 1.f, 1.f};
    cavern.uniform_parameter.shading = {0.2f, 1.f, 0.f}; // non-specular terrain material
    texture_cavern = texture_gpu(image_load_png("data/gravel-stone.png"));
    distance_display_cavern = 200.;

    murene = mesh_load_file_obj("data/murene.obj");
    murene.uniform_parameter.translation = {0.f,0.f,0.f};
    murene.uniform_parameter.shading = {0.6f, 0.5f, 0.f}; // non-specular terrain material


    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    timer.t_min = -10.f;
    timer.t_max = 20.f;
    timer.scale = 1.f;
}

/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_project::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();
    set_lights(shaders["underwater"], scene);

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    glBindTexture(GL_TEXTURE_2D, texture_cavern);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_MIRRORED_REPEAT);
    cavern.draw(shaders["underwater"], scene.camera, distance_display_cavern);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    //murene.ampl = timer.t;
    //murene.draw(shaders["mesh"], scene.camera);

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        cavern.draw(shaders["wireframe"], scene.camera, distance_display_cavern);
        //murene.draw(shaders["wireframe"], scene.camera);

    }

}

void scene_project::set_lights(GLuint shader, scene_structure& scene)
{
    glUseProgram(shader); opengl_debug();

    vec3 dir = scene.camera.orientation*vec3(0.f, 0.f, -1.f);
    light mainLight = light{ 0.2f*dir + scene.camera.camera_position() , vec3(1.f, 1.f, 1.f), 10.f, 1.f, 1.f};
    light cristal[10];
    cristal[0] = light{vec3(5.f, 0.f, 2.f), vec3(0.8f, 0.8f, 0.8f), 1.f, 1.f, 1.f};
    for(int i = 1; i < 10; i++)
    {
        cristal[i] = switchedOffLight;
    }

    spot spotlight = spot{scene.camera.camera_position() , dir, vec3(1.f, 1.f, 1.f), 0.99f, 0.98f, 1.f, 50.f};

    uniform(shader, "mainLight", mainLight); opengl_debug();
    for(int i = 0; i < 10; i++)
    {
        uniform(shader, "sideLight[" + std::to_string(i) + "]", cristal[i]);  opengl_debug();
    }
    uniform(shader, "spotlight", spotlight);  opengl_debug();
}

void scene_project::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}

void scene_project::mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods)
{

}

void scene_project::mouse_move(scene_structure& scene, GLFWwindow* window)
{

}

mesh createMurene()
{
    mesh murene;
    const size_t N = 15;
    const float R = 0.2f;
    const float L = 3.f;
    murene.position.resize(N*4);
    murene.connectivity.resize(N*8 + 4);

    murene.connectivity[0] = index3{0, 1, 2};
    murene.connectivity[1] = index3{0, 3, 2};

    for(unsigned int i = 0; i < N; i++)
    {
        murene.position[i*4]   = vec3(R, R, i*L/float(N-1));
        murene.position[i*4+1] = vec3(R, -R, i*L/float(N-1));
        murene.position[i*4+2] = vec3(-R, -R, i*L/float(N-1));
        murene.position[i*4+3] = vec3(-R, R, i*L/float(N-1));
        if(i >= 1)
        {
            for(unsigned int j = 0; j < 4; j++)
            {
                murene.connectivity[2+i*8+2*j  ] = index3{4*i+j-4,     4*i+(j+1)%4-4 , 4*i+j};
                murene.connectivity[2+i*8+2*j+1] = index3{4*i+(j+1)%4, 4*i+(j+1)%4-4,    4*i+j};
            }
        }
    }
    murene.connectivity[N*8+2] = index3{4*N-4, 4*N-3, 4*N-2};
    murene.connectivity[N*8+3] = index3{4*N-4, 4*N-2, 4*N-1};

    return murene;
}
