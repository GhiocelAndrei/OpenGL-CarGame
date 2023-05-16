#version 330

// Input
// TODO(student): Get values from vertex shader

in vec3 color;
in vec3 normal;
in vec2 texture;
in vec3 position;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_texture;
layout(location = 2) out vec4 out_normal;

uniform vec3 object_color;
uniform int isTeren;

void main()
{
    // TODO(student): Write pixel out color
    if(isTeren==1)
    {
    out_color = vec4(object_color, 1);
    }
    else
    {
    out_color = vec4(color, 1);
    }
   // out_normal = vec4(normal, 1);
    //out_texture = vec4(texture, 1);
}