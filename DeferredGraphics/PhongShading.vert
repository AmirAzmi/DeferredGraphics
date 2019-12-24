#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 fagment_position;
out vec3 normal_direction;

uniform mat4 object_to_world;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 normal_matrix;

void main()
{
  fagment_position = vec3(object_to_world * vec4(aPos, 1.0));
  normal_direction = normal_matrix * aNormal;    

  gl_Position = projection_matrix * view_matrix * vec4(fagment_position, 1.0);
}