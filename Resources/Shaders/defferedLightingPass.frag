/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: defferedLightPass.frag
Purpose: Fragment shader for deffered lighting
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#version 430 core

in vec2 texture_coordinates;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 view_position;

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

  //attenuation factors
  //-------------------------------
  //Total bytes: 48 bytes however actual total is 64 bytes due to aligning on 16 byte boundary
};

layout (std430, binding = 0) buffer shader_data
{
  Light lights[1000];
  int numberOfLights;
};


//uncharted tone mapping function
/*vec3 Uncharted2Tonemap(vec3 x)
{
  float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}*/

void main()
{
  //retrieve data from gbuffer
  vec3 world_position = texture(gPosition, texture_coordinates).rgb;
  vec3 normal_world_position = texture(gNormal, texture_coordinates).rgb;
  vec3 diffuse_color = texture(gAlbedoSpec, texture_coordinates).rgb;
  float specular_intensity = texture(gAlbedoSpec, texture_coordinates).a;
  
  const float gamma = 2.2;
  vec3 lighting = vec3(0.0f) * 0.6f; //(.6 here is the constant ambient value)
  vec3 lighting_no_spec = vec3(0.0f) * 0.6f; //(.6 here is the constant ambient value)
  vec3 normalized_normal_world_position = normalize(normal_world_position);
  vec3 mapped = vec3(1.0f);

  //for all lights
  for(int i = 0; i <  numberOfLights; ++i)
  {
    Light light = lights[i];     

    // diffuse
    vec3 light_direction = normalize(light.position - world_position);
    vec3 diffuse = max(dot(normalized_normal_world_position, light_direction), 0.0) * diffuse_color * light.diffuseColor;

    //specular
    vec3 view_direction = normalize(view_position - world_position);
    vec3 reflection_vector = 2 * dot(normalized_normal_world_position, light_direction) * normalized_normal_world_position - light_direction; //reflection vector  
    float spec = pow(max(dot(reflection_vector, view_direction), 0.0), light.specularExponent);
    vec3 specular = light.specularColor * spec * specular_intensity;

    //attenuation
    float dist = length(light.position - world_position); //distance from light source to fragment
    float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;
    lighting_no_spec += diffuse;
  }


  //doing the tone mapping and gamma correction after lighting calculations is interesting and correct
  //exposure tone mapping
  /*if(exposure_tone_mapping)
  {
    mapped = vec3(1.0) - exp(-diffuse_color * exposure);
  }

  if(uncharted_tone_mapping)
  {
     mapped = Uncharted2Tonemap(diffuse_color);
  }

  if(gamma_correction)
  {
    lighting = (pow(lighting * mapped, vec3(1.0/gamma)));
  }*/

  //this needs to be done somewhere else
  //bloom color extraction after lighting values

  //0.2126, 0.7152, 0.0722
  float brightness = dot(lighting_no_spec.rgb, vec3(0.2126, 0.7152, 0.0722));
  vec4 brightColor;
  if(brightness > 1.0)
  {
    brightColor = vec4(lighting_no_spec.rgb, 1.0);
  }
  else
  {
    brightColor = vec4(0.0, 0.0, 0.0, 1.0);
  }

  BrightColor = brightColor;

  color = vec4(lighting, 1.0) * vec4(mapped.xyz, 1.0f);
}

