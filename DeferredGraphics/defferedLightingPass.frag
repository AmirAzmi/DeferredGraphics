#version 430 core

in vec2 texture_coordinates;

out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light
{
  vec3 ambientColor; //12 bytes 0 % 16
  float constant;         //4 bytes 12 % 4

  vec3 diffuseColor; //12 bytes 16 % 16
  float linear;           //4 bytes 28 % 4

  vec3 specularColor;//12 bytes 32 % 16
  float specularIntensity;//4 bytes 44 % 4

  float quadratic;        //4 bytes 48 % 4
  vec3 padding;      //padding bytes for next array element

  //attenuation factors
  //-------------------------------
  //Total bytes: 48 bytes however actual total is 64 bytes due to aligning on 16 byte boundary
};

layout (std430, binding = 0) buffer shader_data
{
  Light lights[16];
  int numberOfLights;
};

void main()
{
 //retrieve data from gbuffer
 vec3 world_position = texture(gPosition, texture_coordinates).rgb;
 vec3 normal_world_position = texture(gNormal, texture_coordinates).rgb;
 vec3 diffuse_color = texture(gAlbedoSpec, texture_coordinates).rgb;
 float specular_intensity = texture(gAlbedoSpec, texture_coordinates).a;
  
}
