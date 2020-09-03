#version 440 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 texture_coordinates;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, texture_coordinates);
}