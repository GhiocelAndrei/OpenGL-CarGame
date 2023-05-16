#version 330

// Input
// TODO(student): Get vertex attributes from each location

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 position1;

// Output
// TODO(student): Output values to fragment shader
    out vec3 color;
    out vec3 position;
    out vec3 normal;
    out vec2 texture;

void main()
{
    color = v_color;
    position = v_position;
    texture = tex_coord;
    normal = v_normal;
    // TODO(student): Send output to fragment shader
    vec3 posWorld = (Model * vec4(v_position, 1.0)).xyz;
    float scaleFactor = 0.02;
    posWorld.y = posWorld.y - length(position1 - posWorld) * length(position1 - posWorld) * scaleFactor;

    gl_Position = Projection * View * vec4(posWorld, 1.0);
    // TODO(student): Compute gl_Position

}
