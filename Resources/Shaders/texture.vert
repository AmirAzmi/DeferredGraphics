#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;

out vec2 texture_coordinates;
out vec3 ourColor;


uniform mat4 view_matrix;
uniform mat4 perspective_matrix;
uniform mat4 object_to_world;
void main()
{
  gl_Position = perspective_matrix * view_matrix * object_to_world * vec4(aPos, 1.0);
  texture_coordinates = aTexCoords;
  ourColor = aColor;
}