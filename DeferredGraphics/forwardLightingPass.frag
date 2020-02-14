/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: forwardLightingPass.frag
Purpose: Fragment shader for forward lighting currently does diffuse just for testing purposes however phong is in there
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#version 440 core

struct Light
{
  vec3 ambientColor; //12 bytes 0 % 16
  float radius;         //4 bytes 12 % 4
  vec3 diffuseColor; //12 bytes 16 % 16
  float linear;           //4 bytes 28 % 4
  vec3 specularColor;//12 bytes 32 % 16
  float specularExponent;//4 bytes 44 % 4
  vec3 position;      //padding bytes for next array element
  float quadratic;        //4 bytes 48 % 4
};

layout (std430, binding = 0) buffer shader_data
{
  Light lights[1000];
  int numberOfLights;
};

in vec3 normal_world_position;
in vec3 world_position;

uniform vec3 view_position;
uniform vec4 diffuse_color;

out vec4 color;

void main()
{
  vec3 normalized_normal_world_position = normalize(normal_world_position);
  float ambientStrength = 0.1f;
  vec3 result = vec3(diffuse_color.xyz);

  //for all lights  
  for(int i = 0; i <  numberOfLights; ++i)
  {

    Light light = lights[i];   

    // ambient
    vec3 ambient = ambientStrength * light.diffuseColor;
  	
    // diffuse 
    vec3 normalized_normal_world_position = normalize(normal_world_position);
    vec3 light_direction = normalize(light.position - world_position);
    float diff = max(dot(normalized_normal_world_position, light_direction), 0.0f);
    vec3 diffuse = diff * vec3(diffuse_color.xyz) * light.diffuseColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 view_direction = normalize(view_position - world_position);
    vec3 reflection_vector = 2 * dot(normalized_normal_world_position, light_direction) * normalized_normal_world_position - light_direction; //reflection vector  
    float spec = pow(max(dot(reflection_vector, view_direction), 0.0f), 32);
    vec3 specular = specularStrength * spec * light.diffuseColor;  

    result +=  (diffuse + specular + ambient);
  }

  color = vec4(result, 1.0); 
}
