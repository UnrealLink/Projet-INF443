
#include "01_modelisation.hpp"


#ifdef INF443_01_MODELISATION

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;




float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
vec2 evaluate_terrain_texture(float u, float v);


mesh create_terrain();
mesh create_cylinder(double radius, float height);
mesh create_cone(double radius, float height, float z_offset);
mesh create_tree_foliage(double radius, float height, float z_offset, float ecart);
tree create_tree(double r_tronc, float h_tronc, double r_feuille, float h_feuille);
std::vector<vcl::vec3> update_tree_position(size_t n_trees, float min_dist);
bool is_treePos_valid(const std::vector<vec3> &trees, const vec3 &tree, float min_dist);

mesh create_skybox(float size);

void randomize(std::vector<vec3> &vertices, float randomness);
unsigned int addVertices(std::vector<vec3> &vertices, vec3 v, float pre);
std::vector<index3> QuadToTri(const std::vector<index4> &quads);
void subdivideQuads(quadMesh &m);

vec3 spring_force(const vec3& pi, const vec3& pj, float L0, float K);
vec3 damping(const vec3& vi, float mu);
void avance(particle_element* parts, const float m, const float K, const float L0, const float mu, const float dt);
void setFormDyn(particle_element* parts, vcl::mesh_drawable_hierarchy &hierarchy);
const float terrain_size = 20.f;


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    terrain = create_terrain();
    terrain.uniform_parameter.color = {1.f, 1.f, 1.f};
    terrain.uniform_parameter.shading = {0.2f, 1.f, 0.f}; // non-specular terrain material
    texture_terrain = texture_gpu(image_load_png("data/gravel-stone.png"));

    arbre = create_tree(0.1, 1.3f, 0.6, 0.7);
    tree_position = update_tree_position(30, 1.5f);

    mesh m;
    m.position = { {-0.15f, 0.f, 0.f}, {0.15f, 0.f, 0.f}, {0.15f, 0.f, 0.5f}, {-0.15f, 0.f, 0.5f}};
    m.connectivity = {{0, 1, 2}, {0, 2, 3}};
    m.texture_uv = {{0.f, 1.f}, {1.f, 1.f}, {1.f, 0.f}, {0.f, 0.f}};
    billboard = m;
    billboard.uniform_parameter.shading = {0.3f,0.8f,0};

    texture_herbe = texture_gpu(image_load_png("data/billboard_grass.png"));
    texture_fleur = texture_gpu(image_load_png("data/billboard_redflowers.png"));
    herbe_position = update_tree_position(40, 2.f);
    fleur_position = update_tree_position(5, 2.f);

    skybox = create_skybox(30);
    skybox.uniform_parameter.shading = {1, 0, 0};
    texture_skybox = texture_gpu(image_load_png("data/skybox.png"));

    quadMesh qcube = quadMesh::createPlan();
    subdivideQuads(qcube);
    subdivideQuads(qcube);
    subdivideQuads(qcube);
    cube = qcube.toMesh();

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    //setup chenille
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



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    glBindTexture(GL_TEXTURE_2D, texture_terrain);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_MIRRORED_REPEAT);
    terrain.draw(shaders["underwater"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    for(vec3 v: tree_position)
    {
        arbre.put(v - vec3(0.f, 0.f, 0.04f));
        arbre.draw(shaders["underwater"], scene.camera);
    }

    //draw chenille
    const float t = (timer.t - timer.t_min)/timer.t_max;
    const float ut = std::exp(-40*(t-0.5f)*(t-0.5f));

    setForm(pos.x, pos.y + separation*r_body*Nb_corps, 2.5f*r_body*ut);

    //deplacement global
    if(t < 0.5f && t > last_draw)
    {
        pos.y -= 0.7f*(t - last_draw);
    }
    else if(t > last_draw)
    {
        pos.x -= 0.7f*(t - last_draw);
        pos.y += 0.7f*(t - last_draw);
    }
    hierarchy.draw(shaders["underwater"], scene.camera);
    last_draw = t;

    //setFormDyn(parts, hierarchy);

    //cube.draw(shaders["wireframe"], scene.camera);

    //draw skybox
    glBindTexture(GL_TEXTURE_2D, texture_skybox);
    skybox.uniform_parameter.translation = scene.camera.translation;
    skybox.draw(shaders["mesh"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        terrain.draw(shaders["wireframe"], scene.camera);
    }

    //draw billboards
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);
    glBindTexture(GL_TEXTURE_2D, texture_herbe);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    mat3 Id = mat3::identity(), R = rotation_from_axis_angle_mat3({0, 0, 1}, 3.14/2.);
    for(vec3 v: herbe_position)
    {
        billboard.uniform_parameter.translation = v - vec3(0.f, 0.f, 0.02f);
        billboard.uniform_parameter.rotation = Id;
        billboard.draw(shaders["underwater"], scene.camera);
        billboard.uniform_parameter.rotation = R;
        billboard.draw(shaders["underwater"], scene.camera);
    }
    glBindTexture(GL_TEXTURE_2D, texture_fleur);
    for(vec3 v: fleur_position)
    {
        billboard.uniform_parameter.translation = v - vec3(0.f, 0.f, 0.02f);
        billboard.uniform_parameter.rotation = Id;
        billboard.draw(shaders["underwater"], scene.camera);
        billboard.uniform_parameter.rotation = R;
        billboard.draw(shaders["underwater"], scene.camera);
    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);
}



// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
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
vec3 evaluate_terrain(float u, float v)
{
    const float x = terrain_size*(u-0.5f);
    const float y = terrain_size*(v-0.5f);
    const float z = evaluate_terrain_z(u,v);

    return {x,y,z};
}

vec2 evaluate_terrain_texture(float u, float v)
{
    const float x = 10*u;
    const float y = 10*v;

    return {x, y};
}

// Generate terrain meshmesh
mesh create_terrain()
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
            terrain.position[kv+N*ku] = evaluate_terrain(u,v);
            terrain.texture_uv[kv+N*ku] = evaluate_terrain_texture(u, v);
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

mesh create_cylinder(double radius, float height)
{
    mesh cylindre;
    const size_t N = 20;
    cylindre.position.resize(2*N);
    for(size_t i = 0; i < 2*N; i+=2)
    {
        cylindre.position[i] = vec3(radius*std::cos(M_PI*i/N), radius*std::sin(M_PI*i/N), 0);
        cylindre.position[i+1] = vec3(radius*std::cos(M_PI*i/N), radius*std::sin(M_PI*i/N), height);
    }
    for(unsigned int i = 0; i < 2*N-2; i+= 2)
    {
        const index3 triangle_1 = {i, i+1, i+2};
        const index3 triangle_2 = {i+1, i+2, i+3};

        cylindre.connectivity.push_back(triangle_1);
        cylindre.connectivity.push_back(triangle_2);
    }
    cylindre.connectivity.push_back(index3{0,2*N-2, 2*N-1});
    cylindre.connectivity.push_back(index3{0,1, 2*N-1});

    return cylindre;
}

mesh create_cone(double radius, float height, float z_offset)
{
    mesh cone;
    const size_t N = 20;
    cone.position.resize(N+2);
    cone.position[0] = vec3(0, 0, z_offset);
    cone.position[1] = vec3(0, 0, z_offset+ height);
    for(size_t i = 0; i < N; i++)
    {
        cone.position[i+2] = vec3(radius*std::cos(2*M_PI*i/N), radius*std::sin(2*M_PI*i/N), z_offset);

    }
    for(unsigned int i = 2; i < N+1; i++)
    {
        const index3 triangle_1 = {0, i, i+1};
        const index3 triangle_2 = {1, i, i+1};

        cone.connectivity.push_back(triangle_1);
        cone.connectivity.push_back(triangle_2);
    }
    cone.connectivity.push_back(index3{0, N+1, 2});
    cone.connectivity.push_back(index3{1, N+1, 2});

    return cone;
}

mesh create_tree_foliage(double radius, float height, float z_offset, float ecart)
{
    mesh m = create_cone(radius, height, z_offset);
    m.push_back( create_cone(radius, height, z_offset + ecart) );
    m.push_back( create_cone(radius, height, z_offset + 2*ecart) );

    return m;
}

tree create_tree(double r_tronc, float h_tronc, double r_feuille, float h_feuille)
{
    tree arbre;
    arbre.tronc = create_cylinder(r_tronc, h_tronc);
    arbre.tronc.uniform_parameter.shading = {0.f, 1.f, 0.f};
    arbre.tronc.uniform_parameter.color = {139/255, 108/255, 66/255};

    arbre.feuillage = create_tree_foliage(r_feuille, h_feuille, static_cast<double>(h_tronc)*0.95, static_cast<double>(h_feuille)*0.6);
    arbre.tronc.uniform_parameter.shading = {0.f, 1.f, 0.f};
    arbre.feuillage.uniform_parameter.color = {0.4f,0.65f,0.3f};

    return arbre;
}

void tree::put(vec3 v)
{
    tronc.uniform_parameter.translation = v;
    feuillage.uniform_parameter.translation = v;
}

void tree::draw(GLuint shader, const vcl::camera_scene &camera)
{
    tronc.draw(shader, camera);
    feuillage.draw(shader, camera);
}

void scene_exercise::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);

    const float time_scale_min = 0.1f;
    const float time_scale_max = 3.0f;
    ImGui::SliderFloat("Time scale", &timer.scale, time_scale_min, time_scale_max);
}

std::vector<vcl::vec3> update_tree_position(size_t n_trees, float min_dist)
{
    std::uniform_real_distribution<float> distrib(0.0,1.0);
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::vector<vcl::vec3> pos = {};
    while(pos.size() < n_trees)
    {
        vec3 v = evaluate_terrain(distrib(generator), distrib(generator));
        if(is_treePos_valid(pos, v, min_dist))
        {
            pos.push_back(v);
        }
    }
    return pos;
}

bool is_treePos_valid(const std::vector<vec3> &trees, const vec3 &tree, float min_dist)
{
    for(vec3 v: trees)
    {
        vec3 u = v - tree;
        u.z = 0;
        if( norm(u) <= min_dist)
            return false;
    }
    return true;
}

mesh quadMesh::toMesh()
{
    mesh m;
    m.position = position;
    m.connectivity = QuadToTri(connectivity);
    return m;
}

quadMesh quadMesh::createPlan(vcl::vec3 pos, vcl::vec3 u, vcl::vec3 v)
{
    quadMesh m;
    m.position = {};
    m.position.push_back(pos);
    m.position.push_back(pos + u);
    m.position.push_back(pos + v);
    m.position.push_back(pos + u + v);

    m.connectivity = {};
    index4 quad = {0, 1, 3, 2};
    m.connectivity.push_back(quad);

    return m;
}

void randomize(std::vector<vec3> &vertices, float randomness)
{
    std::normal_distribution<float> distrib(0.0,randomness);
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    for(size_t i = 0; i < vertices.size(); i++)
    {
        vertices[i] += vec3(distrib(generator), distrib(generator), distrib(generator));
    }
}

unsigned int addVertices(std::vector<vec3> &vertices, vec3 v, float pre)
{
    for(size_t i = 0; i < vertices.size(); i++)
        if(norm(v - vertices[i]) < pre)
            return i;
    vertices.push_back(v);
    return  vertices.size() - 1;
}

std::vector<index3> QuadToTri(const std::vector<index4> &quads)
{
    std::vector<index3> tris = {};
    for(index4 i : quads)
    {
        index3 tri1 = {i[0], i[1], i[2]};
        index3 tri2 = {i[0], i[2], i[3]};
        tris.push_back(tri1);
        tris.push_back(tri2);
    }
    return tris;
}

void subdivideQuads(quadMesh &m)
{
    std::vector<index4> con = {};
    for(index4 i: m.connectivity)
    {
        std::array<unsigned int, 9> v =
            {i[0],                                                                       addVertices(m.position, (m.position[i[0]] + m.position[i[1]])/2., 0.00001f),                                                                        i[1],
            addVertices(m.position, (m.position[i[0]] + m.position[i[3]])/2., 0.00001f), addVertices(m.position, (m.position[i[0]] + m.position[i[2]])/2., 0.00001f), addVertices(m.position, (m.position[i[1]] + m.position[i[2]])/2., 0.00001f),
             i[3],                                                                       addVertices(m.position, (m.position[i[3]] + m.position[i[2]])/2., 0.00001f),                                                                        i[2]};
        index4 quad1 = {v[0], v[1], v[4], v[3]};
        index4 quad2 = {v[1], v[2], v[5], v[4]};
        index4 quad3 = {v[3], v[4], v[7], v[6]};
        index4 quad4 = {v[4], v[5], v[8], v[7]};
        con.push_back(quad1);
        con.push_back(quad2);
        con.push_back(quad3);
        con.push_back(quad4);
    }
    m.connectivity = con;
}

mesh create_skybox(float size)
{
    const float r = 1.f; //offset evitant les problemes de raccord
    mesh m;
    m.position ={};
    m.position = {vec3(-1, -r, -1)*size, vec3(1, -r, -1)*size, vec3(1, -r, 1)*size, vec3(-1, -r, 1)*size, //devant
                  vec3(-1, r, -1)*size, vec3(1, r, -1)*size, vec3(1, r, 1)*size, vec3(-1, r, 1)*size, //derriere
                  vec3(-1, -1, r)*size, vec3(1, -1, r)*size, vec3(1, 1, r)*size, vec3(-1, 1, r)*size, //dessus
                 vec3(-1, -1, -r)*size, vec3(1, -1, -r)*size, vec3(1, 1, -r)*size, vec3(-1, 1, -r)*size, //dessous
                 vec3(-r, -1, -1)*size, vec3(-r, -1, 1)*size, vec3(-r, 1, 1)*size, vec3(-r, 1, -1)*size, //gauche
                 vec3(r, -1, -1)*size, vec3(r, -1, 1)*size, vec3(r, 1, 1)*size, vec3(r, 1, -1)*size,}; //droite

    m.connectivity = {{0, 1, 2}, {0, 2, 3}, //devant
                      {4, 5, 6}, {4, 6, 7}, //derriere
                      {8, 9, 10}, {8, 10, 11}, //dessus
                      {12, 13, 14}, {12, 14, 15}, //dessous
                      {16, 17, 18}, {16, 18, 19}, //gauche
                      {20, 21, 22}, {20, 22, 23}}; //droite
    float eps = 1.5e-3;
    float t0 = eps, t1 = 1/3.f + eps, t2 = 2/3.f - eps, t3 = 1 - eps, q1 = 0.25f + eps, q2 = 0.50f - eps, q3 = 0.75f;
    m.texture_uv = {{q1, t2}, {q2, t2}, {q2, t1}, {q1, t1}, //devant
                    {1, t2}, {q3, t2}, {q3, t1}, {1, t1}, //derriere
                    {q1, t1}, {q2, t1}, {q2, t0}, {q1, t0}, //dessus
                    {q1, t2}, {q2, t2}, {q2, t3}, {q1, t3}, //dessous
                    {q1, t2}, {q1, t1}, {0, t1}, {0, t2}, //gauche
                    {q2, t2}, {q2, t1}, {q3, t1}, {q3, t2}};//droite

    return m;
}

void scene_exercise::setForm(float yt, float yq, float zMax)
{
    hierarchy.translation("head") =  {0, yt, evaluate_terrain_z(0.5, yt/terrain_size + 0.5f) + r_body};
    for(size_t i = 1; i < Nb_corps; i++)
    {
        std::string s("body");
        s = s + std::to_string(i);
        hierarchy.translation(s) = {0, yq*i/(Nb_corps - 1.f) , zMax*static_cast<float>(std::sin(M_PI*i/(Nb_corps - 1.))) - evaluate_terrain_z(0.5, yt/terrain_size + 0.5f) + evaluate_terrain_z(0.5, yt/terrain_size + 0.5f + yq*i/(Nb_corps - 1.f)/terrain_size)};
        hierarchy.rotation(s) = rotation_between_vector_mat3({0, 1, 0}, {0, 1, zMax*static_cast<float>(std::cos(M_PI*i/(Nb_corps - 1.)))});
    }
}

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

void avance(particle_element* parts, const float m, const float K, const float L0, const float mu, const float dt)
{
    const vec3 g   = {0,-9.81f,0}; // gravity

    for(int i = 1; i < scene_exercise::Nb_corps; i++)
    {
        const vec3 f_weight  = m*g; // TO DO: correct this force value
        const vec3 f_spring  = spring_force(parts[i].p, parts[i-1].p, L0, K);
        const vec3 f_damping = damping(parts[i].v, mu); // TO DO: correct this force value
        vec3 F = f_spring+f_weight+f_damping;
        if(i < scene_exercise::Nb_corps - 1)
            F += spring_force(parts[i].p, parts[i+1].p, L0, K);
        if(i < scene_exercise::Nb_corps -2)
            F += spring_force(parts[i].p, parts[i+2].p, 2*L0, K/5.f);
        if(i > 2)
            F += spring_force(parts[i].p, parts[i-2].p, 2*L0, K/5.f);

        vec3& p = parts[i].p; // position of particle
        vec3& v = parts[i].v; // speed of particle

        v = v + dt*F/m;
        p = p + dt*v;
    }
}

void setFormDyn(particle_element* parts, vcl::mesh_drawable_hierarchy &hierarchy)
{
    hierarchy.translation("head") =  parts[0].p;
    for(size_t i = 1; i < scene_exercise::Nb_corps; i++)
    {
        std::string s("body");
        s = s + std::to_string(i);
        hierarchy.translation(s) = parts[i].p;
    }
}

#endif


















