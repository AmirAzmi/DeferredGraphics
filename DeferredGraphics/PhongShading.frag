#version 440 core

/*
uniform vec4 light_position;
uniform vec4 eye_position;
uniform vec3 light_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float specular_exponent;
uniform vec3 ambient_color;
*/

in vec3 world_position;
in vec3 world_normal;

// Output data
out vec4 color;

void main()
{
  /*
	vec3 L = normalize(vec3(light_position - world_position));
  vec3 m = normalize(vec3(world_normal));
  vec3 v = normalize(vec3(eye_position - world_position));
  vec3 Rl = normalize(vec3(2 *dot(m,L) * m - L));
  */

  // phong illumination model     
  /*color = 
  (vec4(diffuse_color.xyz, 1.0) * vec4(ambient_color.xyz, 1.0)) + //ambient factor
  (vec4(diffuse_color.xyz, 1.0) * max(dot(m,L), 0.0) * vec4(light_color.xyz, 1.0)) +  // diffuse factor
  (vec4(specular_color.xyz, 1.0) * max(pow(dot(Rl,v), specular_exponent), 0.0) * vec4(light_color.xyz, 1.0)); //specular factor
  */

  color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
