#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 world_position;
out vec3 world_normal_position;
out vec2 texture_coordinates;

uniform mat4 object_to_world;
uniform mat4 view_matrix;
uniform mat4 perspective_matrix;
uniform mat4 normal_matrix;

void main()
{
  world_position = vec3(object_to_world * vec4(aPos, 1.0f));
  world_normal_position = vec3(normal_matrix * vec4(aNormal, 0.0f)); 
  texture_coordinates = aTexCoords;

  gl_Position = perspective_matrix * view_matrix * vec4(world_position, 1.0f);
}