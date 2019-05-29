#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 mesh_position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

struct light
{
    vec3 position; //donne dans le repere du monde
    vec3 color;
    float radius; //rayon de la zone lumineuse, dans laquelle il n'y a pas de decroissance a la source
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

uniform sampler3D texture_sampler;

out vec4 FragColor;

uniform vec3 camera_position = vec3(0.f);
uniform vec3 color; // object color
uniform float ambiant  = 0.2f;
uniform float diffuse  = 0.8f;
uniform float specular = 0.5f;

uniform light mainLight;
uniform light sideLight[10];
uniform spot spotlight;

uniform vec3 color_fond = vec3(0.08f, 0.08f, 0.2f);
uniform vec3 attenuationColor = vec3(1.f, 0.4f, 0.2f);
uniform vec3 embossMinMap = vec3(0.f);
uniform vec3 embossMaxMap = vec3(1.f);


uniform vec3 blanc = vec3(1.f, 1.f, 1.f);
uniform vec3 bleu = vec3(.1f, 0.1f, 42.f/255.f);


void lightEclairage(in light l, in fragment_data frag, inout vec3 diffuse_ecl, inout vec3 specular_ecl);
void spotEclairage(in spot l, in fragment_data frag, inout vec3 diffuse_ecl);

void main()
{
    vec4 color_texture = texture(texture_sampler, fragment.mesh_position.xyz);
    vec4 sidetexture;
    if(fragment.mesh_position.y + fragment.mesh_position.z*0.25f  + abs(fragment.mesh_position.x)*0.07 + color_texture.r*0.01 > 0.016f)
        sidetexture = vec4(bleu, 1.f);
    else
        sidetexture = vec4(blanc, 1.f);

    color_texture.rgb = sidetexture.rgb*mix(embossMinMap, embossMaxMap, color_texture.rgb);


    vec3 diffuse_ecl = vec3(0.f), specular_ecl = vec3(0.f);
    lightEclairage(mainLight, fragment, diffuse_ecl, specular_ecl);
    for(int i = 0; i < 10; i++)
        lightEclairage(sideLight[i], fragment, diffuse_ecl, specular_ecl);

   spotEclairage(spotlight, fragment, diffuse_ecl);

    vec3 diffuse_color = clamp(ambiant + diffuse_ecl*diffuse, 0.f, 1.f)*color.rgb*fragment.color.rgb*color_texture.rgb;
    vec3 specular_color = clamp( specular_ecl, 0.f, 1.f)*vec3(1.f)*specular;

    vec3 cf = mix(color_fond, diffuse_color, clamp(diffuse_ecl, 0.f, 1.f) );
    cf += mix(vec3(0.f), specular_color, clamp(diffuse_ecl, 0.f, 1.f));
    FragColor = vec4(cf, color_texture.a*fragment.color.a);
}

void lightEclairage(in light l, in fragment_data frag, inout vec3 diffuse_ecl, inout vec3 specular_ecl)
{
    vec3 n = normalize(frag.normal.xyz);
    vec3 u = normalize(l.position-frag.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(frag.position.xyz-camera_position);

    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), 128.0);

    float dist = distance(frag.position.xyz, camera_position) + distance(l.position, frag.position.xyz);
    vec3 attenuation = clamp(exp(-attenuationColor*(dist - l.radius)/l.strength), 0.f, 1.f);

    diffuse_ecl += diffuse_value*l.color*attenuation;
    specular_ecl += diffuse_value*l.color*attenuation;
}

void spotEclairage(in spot l, in fragment_data frag, inout vec3 diffuse_ecl)
{
    vec3 n = normalize(frag.normal.xyz);
    vec3 u = normalize(l.position-frag.position.xyz);

    float diffuse_value  = diffuse * clamp(dot(u,n), 0.f, 1.f)*smoothstep(l.outter, l.inner, dot(-u,l.dir));

    float dist = distance(frag.position.xyz, camera_position) + distance(l.position, frag.position.xyz);
    vec3 attenuation = clamp(exp(-attenuationColor*(dist - l.radius)/l.strength), 0.f, 1.f);

    diffuse_ecl += diffuse_value*l.color*attenuation;
}
