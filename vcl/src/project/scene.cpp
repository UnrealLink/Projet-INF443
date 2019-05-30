#include "scene.hpp"

using namespace vcl;

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_project::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    /*
    terrain = createTerrain();
    terrain.uniform_parameter.color = {1.f, 1.f, 1.f};
    terrain.uniform_parameter.shading = {0.13f, 1.f, 0.f}; // non-specular terrain material
    texture_terrain = texture_gpu(image_load_png("data/gravel-stone.png"));
    */

    cavern = createCavern(vec3(0, 0, 0), 50, 10, 1, isovalue);
    cavern.uniform_parameter.color = {1.f, 1.f, 1.f};
    cavern.uniform_parameter.shading = {0.13f, 1.f, 0.f}; // non-specular terrain material
    texture_cavern = texture_gpu(image_load_png("data/rock.png"));
    distance_display_cavern = 40.;
    float embossStrength = 0.2;

    // Create murene
    mur = load_murene("data/murene.obj");
    mur.uniform_parameter.translation = {0.f,0.f,0.f};
    mur.uniform_parameter.shading = {0.13f, 0.7f, 0.3f};
    mur.uniform_parameter.color = {55/255.f, 55/255.f, 0/255.f};
    mur.embossMinMap = vec3(1.f- embossStrength, 1.f- embossStrength, 1.f- embossStrength);
    mur.embossMaxMap = vec3(1.f+ embossStrength, 1.f+ embossStrength, 1.f+ embossStrength);

    // Create requin
    requin = load_requin("data/requin.obj", 1.f);
    requin.start = 0.5;
    requin.uniform_parameter.shading = {0.6f, 0.5f, 0.f};
    requin.uniform_parameter.translation = {0.f,0.4f,0.f};
    requin.uniform_parameter.color = {1.f, 1.f, 1.f};
    requin.uniform_parameter.rotation = rotation_from_axis_angle_mat3({1.0f, .0f, .0f}, 3.14159f/2.f);
    embossStrength = 0.1;
    requin.embossMinMap = vec3(1.f- embossStrength, 1.f- embossStrength, 1.f- embossStrength);
    requin.embossMaxMap = vec3(1.f+ embossStrength, 1.f+ embossStrength, 1.f+ embossStrength);
    //glEnable(GL_TEXTURE_3D);
    //texture_perlin = createPerlin3DTexture();
    texture_perlin = texture_gpu(image_load_png("data/perlin.png"));

    // Create waterbox
    waterbox = create_waterbox();
    waterbox.uniform_parameter.shading = {1,0,0};
    waterbox.uniform_parameter.rotation = rotation_from_axis_angle_mat3({1,0,0},-3.014f/2.0f);
    texture_waterbox = texture_gpu(image_load_png("data/waterbox.png"));

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_fps;
    scene.camera.scale = 0.f;
    scene.camera.apply_rotation(0,0,0,1.2f);
    scene.camera.apply_translation_orthogonal_to_screen_plane(-5.f);

    // Setup timer for ondulations
    timer.t_min = -10.f;
    timer.t_max = 10.f;
    timer.scale = 10.f;
}

/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_project::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();
    set_lights(shaders["underwater"], scene);
    set_lights(shaders["requin"], scene);

    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    /*
    glBindTexture(GL_TEXTURE_2D, texture_terrain);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_MIRRORED_REPEAT);
    glUseProgram(shaders["underwater"]);
    uniform(shaders["underwater"], "embossMinMap", vec3(0.f, 0.f, 0.f));
    uniform(shaders["underwater"], "embossMaxMap", vec3(1.f, 1.f, 1.f));
    terrain.draw(shaders["underwater"], scene.camera);
    */
   /*
    glBindTexture(GL_TEXTURE_2D, texture_cavern);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,   GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,   GL_REPEAT);
    */
    glUseProgram(shaders["underwater"]);
    cavern.draw(shaders["underwater"], scene, texture_cavern, scene.texture_white, distance_display_cavern);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Display waterbox
    if (gui_scene.waterbox) {
        glBindTexture(GL_TEXTURE_2D, texture_waterbox);
        waterbox.uniform_parameter.scaling = 150.0f;
        waterbox.uniform_parameter.translation = scene.camera.camera_position() + vec3(0,0,-50.0f);
        waterbox.draw(shaders["underwater"], scene.camera);
        glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    }

    // Display murene
    mur.ampl = timer.t;
    glBindTexture(GL_TEXTURE_2D, texture_perlin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);
    //mur.draw(shaders["underwater"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Display requin
    requin.ampl = timer.t/10.f;
    glUseProgram(shaders["requin"]);
    /*
    glBindTexture(GL_TEXTURE_3D, texture_perlin); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,  GL_REPEAT); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,  GL_REPEAT); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R,  GL_REPEAT); opengl_debug();
    */
    glBindTexture(GL_TEXTURE_2D, texture_perlin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);
    
    // compute requin movement
    pos_requin = requin.uniform_parameter.translation;
    grad_requin.x = (isovalue(pos_requin + h*vec3(1, 0, 0)) - isovalue(pos_requin - h*vec3(1, 0, 0))) / (2*h);
    grad_requin.y = (isovalue(pos_requin + h*vec3(0, 1, 0)) - isovalue(pos_requin - h*vec3(0, 1, 0))) / (2*h);
    grad_requin.z = (isovalue(pos_requin + h*vec3(0, 0, 1)) - isovalue(pos_requin - h*vec3(0, 0, 1))) / (2*h);
    speed = normalize(speed + a*grad_requin);
    requin.uniform_parameter.translation += h*speed;
    /*
    mat3 rotx(1, 0, 0, 0, std::cos((float)i), -std::sin((float)i), 0, std::sin((float)i), std::cos((float)i));
    mat3 roty(std::cos((float)i), 0, std::sin((float)i), 0, 1, 0, -std::sin((float)i), 0, std::cos((float)i));
    mat3 rotz(std::cos((float)i), -std::sin((float)i), 0, std::sin((float)i), std::cos((float)i), 0, 0, 0, 1);
    */
    requin.draw(shaders["requin"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Display wireframe
    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
        glPolygonOffset( 1.0, 1.0 );
        //terrain.draw(shaders["wireframe"], scene.camera);
        cavern.draw(shaders["wireframe"], scene, texture_cavern, scene.texture_white, distance_display_cavern);
        //mur.draw(shaders["wireframe"], scene.camera);
        requin.draw(shaders["wireframe"], scene.camera);

    }

}

void scene_project::set_lights(GLuint shader, scene_structure& scene)
{
    glUseProgram(shader); opengl_debug();

    vec3 dir = scene.camera.orientation*vec3(0.f, 0.f, -1.f);
    light mainLight = light{ 0.2f*dir + scene.camera.camera_position() , vec3(1.f, 1.f, 1.f), 0.f, 10.f};
    light cristal[10];
    cristal[0] = light{vec3(5.f, 0.f, 2.f), vec3(0.8f, 0.8f, 0.8f), 1.f, 1.f};
    for(int i = 1; i < 10; i++)
    {
        cristal[i] = switchedOffLight;
    }

    spot spotlight = spot{scene.camera.camera_position() , dir, vec3(1.f, 1.f, 1.f), 0.99f, 0.98f, 0.f, 50.f};
    //spotlight = switchedOffSpot;

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
    ImGui::Checkbox("Waterbox", &gui_scene.waterbox);
}

void scene_project::mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods)
{

}

void scene_project::mouse_move(scene_structure& scene, GLFWwindow* window)
{

}
