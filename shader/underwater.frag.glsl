#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

struct light
{
    vec3 position; //donne dans le repere du monde
    vec3 color;
    float radius; //rayon de la zone lumineuse, dans laquelle il n'y a pas de decroissance a la source
    float rangeDecr;  //vitesse de decroissance de la portee hors rayon
    float strength;
};

struct spot
{
    vec3 position;
    vec3 dir;
    vec3 color;
    float inner;
    float outter;
    float strength;
    float radius;
};

uniform sampler2D texture_sampler;

out vec4 FragColor;

uniform vec3 camera_position = vec3(0.f);
uniform vec3 color; // object color
uniform float ambiant  = 0.2f;
uniform float diffuse  = 0.8f;
uniform float specular = 0.5f;

uniform light mainLight;
uniform light sideLight[10];
uniform spot spotlight;

uniform vec3 attenuationColor = vec3(0.15f, 0.15f, 0.8f);

vec3 mainEclairage(in light l, in fragment_data frag, in vec4 color_texture);
vec3 sideEclairage(in light l, in fragment_data frag, in vec4 color_texture);
vec3 spotEclairage(in spot l, in fragment_data frag, in vec4 color_texture);

void main()
{
    vec4 color_texture = texture(texture_sampler, fragment.texture_uv);
    vec3 cf = mainEclairage(mainLight, fragment, color_texture);
    for(int i = 0; i < 10; i++)
        cf += sideEclairage(sideLight[i], fragment, color_texture);

    cf += spotEclairage(spotlight, fragment, color_texture);

    FragColor = vec4(cf, color_texture.a*fragment.color.a);
}

vec3 mainEclairage(in light l, in fragment_data frag, in vec4 color_texture)
{
    vec3 n = normalize(frag.normal.xyz);
    vec3 u = normalize(l.position-frag.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(frag.position.xyz-camera_position);

    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), 128.0);

    vec3 c = ((ambiant+diffuse_value)*color.rgb*frag.color.rgb*color_texture.rgb + specular_value)*l.color;
    return c*min(vec3(1.f), attenuationColor*l.strength/(distance(frag.position.xyz, camera_position) + max(distance(l.position, frag.position.xyz) - l.radius, 0)*l.rangeDecr ));
}

vec3 sideEclairage(in light l, in fragment_data frag, in vec4 color_texture) //doesnt add ambiant and specular
{
    vec3 n = normalize(frag.normal.xyz);
    vec3 u = normalize(l.position-frag.position.xyz);

    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);

    vec3 c = diffuse_value*color.rgb*frag.color.rgb*color_texture.rgb*l.color;
    return c*min(vec3(1.f), attenuationColor*l.strength/(distance(frag.position.xyz, camera_position) + max(distance(l.position, frag.position.xyz) - l.radius, 0)*l.rangeDecr ));
}

vec3 spotEclairage(in spot l, in fragment_data frag, in vec4 color_texture)
{
    vec3 n = normalize(frag.normal.xyz);
    vec3 u = normalize(l.position-frag.position.xyz);

    float diffuse_value  = diffuse * clamp(dot(u,n), 0.f, 1.f) *smoothstep(l.outter, l.inner, dot(-u,l.dir));

    vec3 c = diffuse_value*color.rgb*frag.color.rgb*color_texture.rgb*l.color;
    return c*min(vec3(1.f), attenuationColor*l.strength/max(distance(frag.position.xyz, camera_position) + distance(l.position, frag.position.xyz) - l.radius*2 , 1.f));
}






