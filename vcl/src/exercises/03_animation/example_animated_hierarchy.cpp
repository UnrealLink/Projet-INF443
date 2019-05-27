
#include "example_animated_hierarchy.hpp"


#ifdef INF443_EXAMPLE_ANIMATED_HIERARCHY


using namespace vcl;



void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    mesh_drawable body = mesh_primitive_sphere(r_body,{0,0,0},40,40);
    body.uniform_parameter.color = {132/255.f, 233/255.f, 99/255.f};

    mesh_drawable eye = mesh_primitive_sphere(r_body/3.f,{0,0,0},20,20);
    eye.uniform_parameter.color = {0,0,0};

    mesh_drawable pate = mesh_primitive_sphere(r_body/6.f);
    pate.uniform_parameter.color = {0,0,0};

    hierarchy.add_element(body, "head", "root");
    hierarchy.add_element(pate, "pate head left", "head", {r_body*0.4f, 0, -r_body*0.85f});
    hierarchy.add_element(pate, "pate head right", "head", {-r_body*0.4f, 0, -r_body*0.85f});

    for(size_t i = 1; i < Nb_corps; i++)
    {
        std::string s("body"), p("pate");
        hierarchy.add_element(body, s + std::to_string(i), "head");
        hierarchy.add_element(pate, p + std::to_string(i) + " left", s + std::to_string(i), {r_body*0.4f, 0, -r_body*0.85f});
        hierarchy.add_element(pate, p + std::to_string(i) + " right", s + std::to_string(i), {-r_body*0.4f, 0, -r_body*0.85f});
    }

    hierarchy.add_element(eye, "eye left", "head", {r_body*0.35f, -r_body*0.60f, r_body*0.3f});
    hierarchy.add_element(eye, "eye right", "head", {-r_body*0.35f, -r_body*0.60f, r_body*0.3f});

    timer.t_min = 0.f;
    timer.t_max = 1.f;
    timer.scale = 0.1f;
}


void scene_exercise::setForm(float yt, float yq, float zMax)
{
    hierarchy.translation("head") =  {0, yt, 0};
    for(size_t i = 1; i < Nb_corps; i++)
    {
        std::string s("body");
        s = s + std::to_string(i);
        hierarchy.translation(s) = {0, yq*i/(Nb_corps - 1.f) , zMax*static_cast<float>(std::sin(M_PI*i/(Nb_corps - 1.)))};
        hierarchy.rotation(s) = rotation_between_vector_mat3({0, 1, 0}, {0, 1, zMax*static_cast<float>(std::cos(M_PI*i/(Nb_corps - 1.)))});
    }
}

void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();

    const float t = (timer.t - timer.t_min)/timer.t_max;
    const float ut = std::exp(-40*(t-0.5f)*(t-0.5f));

    setForm(pos.x, pos.y + separation*r_body*Nb_corps, 4*r_body*ut);
    //setForm(-2, 2, 4);

    //deplacement global
    if(t < 0.5f && t > last_draw)
    {
        pos.y -= t - last_draw;
    }
    else if(t > last_draw)
    {
        pos.x -= t - last_draw;
        pos.y += t - last_draw;
    }
    //drawing
    hierarchy.draw(shaders["mesh"], scene.camera);
    if(gui_scene.wireframe)
        hierarchy.draw(shaders["wireframe"], scene.camera);

    last_draw = t;
}

void scene_exercise::set_gui()
{
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);

    const float time_scale_min = 0.1f;
    const float time_scale_max = 3.0f;
    ImGui::SliderFloat("Time scale", &timer.scale, time_scale_min, time_scale_max);
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);

}




#endif

