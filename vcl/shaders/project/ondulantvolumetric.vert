#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texture_uv;

out struct fragment_data
{
    vec4 position;
    vec4 mesh_position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

// model transformation
uniform vec3 translation = vec3(0.0, 0.0, 0.0);                      // user defined translation
uniform mat3 rotation = mat3(1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0); // user defined rotation
uniform float scaling = 1.0;                                         // user defined scaling
uniform vec3 scaling_axis = vec3(1.0,1.0,1.0);                       // user defined scaling

// view transform
uniform mat4 view;
// perspective matrix
uniform mat4 perspective;

//spline description
uniform vec3 axe[54];
uniform float mesh_length;
uniform float mu = 0.5f;

vec3 spline_pos(in vec3 v1, in vec3 v2, in vec3 v3, in vec3 v4, in float t);
vec3 spline_dev(in vec3 v1, in vec3 v2, in vec3 v3, in vec3 v4, in float t);
mat3 rotation_from_axis_angle_mat3(in vec3 axis, in float angle);
mat3 rotation_between_vector_mat3(in vec3 a, in vec3 b);

void main()
{
    // scaling matrix
    mat4 S = mat4(scaling*scaling_axis.x,0.0,0.0,0.0, 0.0,scaling*scaling_axis.y,0.0,0.0, 0.0,0.0,scaling*scaling_axis.z,0.0, 0.0,0.0,0.0,1.0);
    // 4x4 rotation matrix
    mat4 R = mat4(rotation);
    // 4D translation
    vec4 T = vec4(translation,0.0);

    float t = fract(50*position.z/mesh_length);
    int i = int(floor(50*position.z/mesh_length)) + 2;
    vec3 p = spline_pos(axe[i-1], axe[i], axe[i+1], axe[i+2], t);
    vec3 d = spline_dev(axe[i-1], axe[i], axe[i+1], axe[i+2], t);

    mat3 fresnel = rotation_between_vector_mat3( normalize(d), vec3(0., 0., 1.f) );
    p = p + fresnel*vec3(position.xy, 0.f);

    fragment.normal = R*mat4(fresnel)*normal;
    vec4 position_transformed = R*S*vec4(p, position.w) + T;

    fragment.color = color;
    fragment.texture_uv = texture_uv;

    fragment.position = position_transformed;
    gl_Position = perspective * view * position_transformed;
}

vec3 spline_pos(in vec3 v1, in vec3 v2, in vec3 v3, in vec3 v4, in float t)
{
    vec3 d1 = mu*(v3 - v1);
    vec3 d2 = mu*(v4 - v2);
    return (2.*t*t*t - 3.*t*t + 1.)*v2 + t*(t*t - 2.*t +1.)*d1 + t*t*(-2.f*t+3.)*v3 + t*t*(t-1.)*d2;
}

vec3 spline_dev(in vec3 v1, in vec3 v2, in vec3 v3, in vec3 v4, in float t)
{
    vec3 d1 = mu*(v3 - v1);
    vec3 d2 = mu*(v4 - v2);
    return 6.*t*(t - 1.)*(v2-v3) + (6.*t*(t - 1.) + 1.)*d1 + t*(3.*t-2.)*d2;
}

mat3 rotation_from_axis_angle_mat3(in vec3 axis, in float angle)
{
    vec3 u = normalize(axis);
    float x = u.x;
    float y = u.y;
    float z = u.z;
    float c = cos(angle);
    float s = sin(angle);

    return mat3(c+x*x*(1-c)  , x*y*(1-c)-z*s, x*z*(1-c)+y*s,
                y*x*(1-c)+z*s, c+y*y*(1-c)  , y*z*(1-c)-x*s,
                z*x*(1-c)-y*s, z*y*(1-c)+x*s, c+z*z*(1-c)   );
}

mat3 rotation_between_vector_mat3(in vec3 a, in vec3 b)
{
    vec3 u0 = normalize(a);
    vec3 u1 = normalize(b);

    if( length(u0-u1)<1e-4f )
        return mat3(1.,0.,0.,
                    0.,1.,0.,
                    0.,0.,1.);

    float d = dot(u0,u1);
    float angle = acos( d );

    vec3 axis = cross(u0,u1);
    float L_axis = length(axis);
    if( L_axis>1e-6f)
        axis /= L_axis;
    else
        axis = u0;

    return rotation_from_axis_angle_mat3(axis,angle);
}
