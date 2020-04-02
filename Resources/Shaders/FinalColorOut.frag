#version 330 core

layout (location = 0) out vec4 color;

in vec2 texture_coordinates;

uniform sampler2D ucolor;

void main()
{
   color = vec4(texture(ucolor, texture_coordinates).rgb, 1.0f);
}