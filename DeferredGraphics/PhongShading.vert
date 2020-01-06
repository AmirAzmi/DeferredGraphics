#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 world_position;
out vec3 world_normal_position;

uniform mat4 object_to_world;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 normal_matrix;

void main()
{
  world_position = vec3(object_to_world * vec4(aPos, 1.0f));

  world_normal_position = vec3(normal_matrix * vec4(aNormal, 1.0f));    


  gl_Position = projection_matrix * view_matrix * vec4(world_position, 1.0f);
}