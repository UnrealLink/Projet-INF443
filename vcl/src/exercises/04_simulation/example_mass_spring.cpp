
#include "example_mass_spring.hpp"


#ifdef INF443_EXAMPLE_MASS_SPRING

using namespace vcl;


static void set_gui(timer_basic& timer);

void avance(particle_element* p, const float m, const float K, const float mu, const float dt);

/** Compute spring force applied on particle pi from particle pj */
vec3 spring_force(const vec3& pi, const vec3& pj, float L0, float K)
{
    return K*(norm(pi - pj) - L0)*normalize(pj - pi);
}

/** Compute damping force applied on particle pi from particle pj */
vec3 damping(const vec3& vi, float mu)
{
    return -mu*vi;
}

void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    // Initial position and speed of particles
    // ******************************************* //
    for(size_t i = 0; i < Nb_particles; i++)
    {
        parts[i].p = {i*1.f/Nb_particles, 0.f, 0.f};
        parts[i].v = {0.f, 0.f, 0.f};
    }

    L0 = 0.5f/Nb_particles; // Rest length

    // Display elements
    // ******************************************* //
    segment_drawer.init();
    segment_drawer.uniform_parameter.color = {0,0,1};

    sphere = mesh_primitive_sphere();
    sphere.uniform_parameter.scaling = 0.05f;


    std::vector<vec3> borders_segments = {{-1,-1,-1},{1,-1,-1}, {1,-1,-1},{1,1,-1}, {1,1,-1},{-1,1,-1}, {-1,1,-1},{-1,-1,-1},
                                          {-1,-1,1} ,{1,-1,1},  {1,-1,1}, {1,1,1},  {1,1,1}, {-1,1,1},  {-1,1,1}, {-1,-1,1},
                                          {-1,-1,-1},{-1,-1,1}, {1,-1,-1},{1,-1,1}, {1,1,-1},{1,1,1},   {-1,1,-1},{-1,1,1}};
    borders = borders_segments;
    borders.uniform_parameter.color = {0,0,0};

}

void avance(particle_element* parts, const float m, const float K, const float L0, const float mu, const float dt)
{
    const vec3 g   = {0,-9.81f,0}; // gravity

    for(int i = 1; i < Nb_particles; i++)
    {
        const vec3 f_weight  = m*g; // TO DO: correct this force value
        const vec3 f_spring  = spring_force(parts[i].p, parts[i-1].p, L0, K);
        const vec3 f_damping = damping(parts[i].v, mu); // TO DO: correct this force value
        vec3 F = f_spring+f_weight+f_damping;
        if(i < Nb_particles - 1)
            F += spring_force(parts[i].p, parts[i+1].p, L0, K);
        if(i < Nb_particles -2)
            F += spring_force(parts[i].p, parts[i+2].p, 2*L0, K/5.f);
        if(i > 2)
            F += spring_force(parts[i].p, parts[i-2].p, 2*L0, K/5.f);

        vec3& p = parts[i].p; // position of particle
        vec3& v = parts[i].v; // speed of particle

        v = v + dt*F/m;
        p = p + dt*v;
    }
}



void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui(timer);


    // Simulation time step (dt)
    float dt = timer.scale*0.0001f;

    // Simulation parameters
    const float m  = 0.005f;        // particle mass
    const float K  = 12.0f;         // spring stiffness
    const float mu = 0.01f;       // damping coefficient

    for(int i = 0; i < 100; i++)
        avance(parts, m, K, L0, mu, dt);


    // Display of the result
    for(int i = 0; i < Nb_particles; i++)
    {
        sphere.uniform_parameter.translation = parts[i].p;
        sphere.uniform_parameter.color = {1*i/(float)Nb_particles, 1 - i/(float)Nb_particles, 0};
        sphere.draw(shaders["mesh"], scene.camera);
        if(i < Nb_particles - 1)
        {
            segment_drawer.uniform_parameter.p1 = parts[i].p;
            segment_drawer.uniform_parameter.p2 = parts[i+1].p;
            segment_drawer.draw(shaders["segment_im"],scene.camera);

        }
    }

    borders.draw(shaders["curve"], scene.camera);
}


/** Part specific GUI drawing */
static void set_gui(timer_basic& timer)
{
    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 2.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    // Start and stop animation
    if (ImGui::Button("Stop"))
        timer.stop();
    if (ImGui::Button("Start"))
        timer.start();

}



#endif
