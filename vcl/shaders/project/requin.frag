#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 mesh_position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

uniform sampler2D texture_sampler;

out vec4 FragColor;

uniform vec3 camera_position;
uniform vec3 color; // object color
uniform float ambiant  = 0.2;
uniform float diffuse  = 0.8;
uniform float specular = 0.5;

vec3 light = vec3(camera_position.x, camera_position.y, 3.0f);

uniform vec3 blanc = vec3(1.f, 1.f, 1.f);
uniform vec3 bleu = vec3(0.f, 0.f, 1.f);


void main()
{
    vec3 n = normalize(fragment.normal.xyz);
    vec3 u = normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(fragment.position.xyz-camera_position);


    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), 128.0);


    vec3 white = vec3(1.0);
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    if(fragment.mesh_position.y - fragment.mesh_position.z*0.12f > -0.08f)
        color_texture = vec4(bleu, 1.f);
    else
        color_texture = vec4(blanc, 1.f);


    vec3 c = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb + specular_value*white;


    FragColor = vec4(c, color_texture.a*fragment.color.a);
}

