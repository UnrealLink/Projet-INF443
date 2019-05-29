#include "scene.hpp"

using namespace vcl;

float evaluateTerrain_z(float u, float v);
vec3 evaluateTerrain(float u, float v, float terrain_size);
vec2 evaluateTerrainTexture(float u, float v, int repeat);
mesh createTerrain();
mesh createMurene();
mesh create_waterbox();
GLuint createPerlin3DTexture();

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_project::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    terrain = createTerrain();
    terrain.uniform_parameter.color = {1.f, 1.f, 1.f};
    terrain.uniform_parameter.shading = {0.13f, 1.f, 0.f}; // non-specular terrain material
    texture_terrain = texture_gpu(image_load_png("data/gravel-stone.png"));

    cavern = createCavern(vec3(50, 50, 0), 5, 20, -1, perlin3D, 0.5);
    cavern.uniform_parameter.color = {1.f, 1.f, 1.f};
    cavern.uniform_parameter.shading = {0.13f, 1.f, 0.f}; // non-specular terrain material
    texture_cavern = texture_gpu(image_load_png("data/rock.png"));
    distance_display_cavern = 500.;
    float embossStrength = 0.2;


    mur = load_murene("data/murene.obj");
    mur.uniform_parameter.translation = {0.f,0.f,0.f};
    mur.uniform_parameter.shading = {0.13f, 0.7f, 0.3f};
    mur.uniform_parameter.color = {55/255.f, 55/255.f, 0/255.f};
    mur.embossMinMap = vec3(1.f- embossStrength, 1.f- embossStrength, 1.f- embossStrength);
    mur.embossMaxMap = vec3(1.f+ embossStrength, 1.f+ embossStrength, 1.f+ embossStrength);

    requin = load_requin("data/requin.obj", 50.f);
    requin.start = 0.5;
    requin.uniform_parameter.shading = {0.6f, 0.5f, 0.f};
    requin.uniform_parameter.translation = {0.f,0.4f,0.f};
    requin.uniform_parameter.color = {1.f, 1.f, 1.f};
    requin.uniform_parameter.rotation = rotation_from_axis_angle_mat3({1.0f, .0f, .0f}, 3.14159f/2.f);
    embossStrength = 0.1;
    requin.embossMinMap = vec3(1.f- embossStrength, 1.f- embossStrength, 1.f- embossStrength);
    requin.embossMaxMap = vec3(1.f+ embossStrength, 1.f+ embossStrength, 1.f+ embossStrength);
    glEnable(GL_TEXTURE_3D);
    texture_perlin = createPerlin3DTexture();

    waterbox = create_waterbox();
    waterbox.uniform_parameter.shading = {1,0,0};
    waterbox.uniform_parameter.rotation = rotation_from_axis_angle_mat3({1,0,0},-3.014f/2.0f);
    texture_waterbox = texture_gpu(image_load_png("data/waterbox.png"));

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_fps;
    scene.camera.scale = 0.f;
    scene.camera.apply_rotation(0,0,0,1.2f);
    scene.camera.apply_translation_orthogonal_to_screen_plane(-5.f);

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

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    glBindTexture(GL_TEXTURE_2D, texture_terrain);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_MIRRORED_REPEAT);
    glUseProgram(shaders["underwater"]);
    uniform(shaders["underwater"], "embossMinMap", vec3(0.f, 0.f, 0.f));
    uniform(shaders["underwater"], "embossMaxMap", vec3(1.f, 1.f, 1.f));
    terrain.draw(shaders["underwater"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, texture_cavern);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,   GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,   GL_REPEAT);
    cavern.draw(shaders["mesh"], scene.camera, distance_display_cavern);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    display_waterbox(shaders, scene);

    mur.ampl = timer.t;
    glBindTexture(GL_TEXTURE_2D, texture_perlin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);
    //mur.draw(shaders["underwater"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    requin.ampl = timer.t/10.f;
    glUseProgram(shaders["requin"]);
    glBindTexture(GL_TEXTURE_3D, texture_perlin); opengl_debug();
    std::cout << "bint" << std::endl;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,  GL_REPEAT); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,  GL_REPEAT); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R,  GL_REPEAT); opengl_debug();
    requin.draw(shaders["requin"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);


    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        terrain.draw(shaders["wireframe"], scene.camera);
        mur.draw(shaders["wireframe"], scene.camera);
        cavern.draw(shaders["wireframe"], scene.camera, distance_display_cavern);

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
    ImGui::Checkbox("waterbox", &gui_scene.waterbox);
}

void scene_project::mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods)
{

}

void scene_project::mouse_move(scene_structure& scene, GLFWwindow* window)
{

}

// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluateTerrain_z(float u, float v)
{
    const std::vector<vec2> pi = {vec2(0,0),vec2(0.5,0.5),vec2(0.2,0.7),vec2(0.8,0.7)};
    const std::vector<float> hi = {3,-1.5,1,2}, sigmai={0.5,0.15,0.2,0.2};
    vec2 p = {u, v};

    float z = 0;

    for(size_t i = 0; i < pi.size(); i++)
        z += hi[i]*std::exp( -dot(p-pi[i], p-pi[i])/sigmai[i]/sigmai[i] );

    return z + (perlin(3*u, 3*v, 7, 0.4)-0.5f)*0.8f;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluateTerrain(float u, float v, float terrain_size)
{
    const float x = terrain_size*(u-0.5f);
    const float y = terrain_size*(v-0.5f);
    const float z = evaluateTerrain_z(u,v);

    return {x,y,z};
}

vec2 evaluateTerrainTexture(float u, float v, int repeat)
{
    const float x = repeat*u;
    const float y = repeat*v;

    return {x, y};
}

// Generate terrain meshmesh
mesh createTerrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 600;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            terrain.position[kv+N*ku] = evaluateTerrain(u,v, 20.f);
            terrain.texture_uv[kv+N*ku] = evaluateTerrainTexture(u, v, 10);
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
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

GLuint createPerlin3DTexture()
{
    GLuint id = 0;
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_3D,id);

    const int n = 16;
    GLfloat data[n*n*n*4];
    for (size_t i = 0; i < n*n*n*4; i = i+4)
    {
        float u = (i%n)/float(n);
        float v = ((i/n)%n)/float(n);
        float w = i/(n*n);
        data[i] = perlin(u,v,w);
        data[i+1] = perlin(w, u, v);
        data[i+2] = perlin(v, w, u);
        data[i+3] = 1.f;
    }

    // Send texture on GPU
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, n, n, n, 0, GL_RGBA,  GL_FLOAT, &data[0]); opengl_debug();
    glGenerateMipmap(GL_TEXTURE_3D); opengl_debug();

    // Set default texture behavior
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT); opengl_debug();

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); opengl_debug();

    glBindTexture(GL_TEXTURE_3D,0);

    std::cout << "created" << std::endl;
    return id;
}

void scene_project::display_waterbox(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    if(gui_scene.waterbox)
    {
        glBindTexture(GL_TEXTURE_2D,texture_waterbox);
        waterbox.uniform_parameter.scaling = 150.0f;
        waterbox.uniform_parameter.translation = scene.camera.camera_position() + vec3(0,0,-50.0f);
        waterbox.draw(shaders["underwater"], scene.camera);
        glBindTexture(GL_TEXTURE_2D,scene.texture_white);
    }
}

vcl::mesh create_waterbox()
{
    const vec3 p000 = {-1,-1,-1};
    const vec3 p001 = {-1,-1, 1};
    const vec3 p010 = {-1, 1,-1};
    const vec3 p011 = {-1, 1, 1};
    const vec3 p100 = { 1,-1,-1};
    const vec3 p101 = { 1,-1, 1};
    const vec3 p110 = { 1, 1,-1};
    const vec3 p111 = { 1, 1, 1};

    mesh waterbox;

    waterbox.position = {
        p000, p100, p110, p010,
        p010, p110, p111, p011,
        p100, p110, p111, p101,
        p000, p001, p010, p011,
        p001, p101, p111, p011,
        p000, p100, p101, p001
    };


    waterbox.connectivity = {
        {0,1,2}, {0,2,3}, {4,5,6}, {4,6,7},
        {8,11,10}, {8,10,9}, {17,16,19}, {17,19,18},
        {23,22,21}, {23,21,20}, {13,12,14}, {13,14,15}
    };

    const float e = 1e-3f;
    const float u0 = 0.0f;
    const float u1 = 0.25f+e;
    const float u2 = 0.5f-e;
    const float u3 = 0.75f-e;
    const float u4 = 1.0f;
    const float v0 = 0.0f;
    const float v1 = 1.0f/3.0f+e;
    const float v2 = 2.0f/3.0f-e;
    const float v3 = 1.0f;
    waterbox.texture_uv = {
        {u1,v1}, {u2,v1}, {u2,v2}, {u1,v2},
        {u1,v2}, {u2,v2}, {u2,v3}, {u1,v3},
        {u2,v1}, {u2,v2}, {u3,v2}, {u3,v1},
        {u1,v1}, {u0,v1}, {u1,v2}, {u0,v2},
        {u4,v1}, {u3,v1}, {u3,v2}, {u4,v2},
        {u1,v1}, {u2,v1}, {u2,v0}, {u1,v0}
    };


    return waterbox;

}
