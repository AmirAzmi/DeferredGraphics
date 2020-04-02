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

uniform sampler2D ucolor;
uniform sampler2D BrightColor;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
  vec2 tex_offset = 1.0 / textureSize(BrightColor, 0); // gets size of single texel
  vec3 result = texture(BrightColor, texture_coordinates).rgb * weight[0]; // current fragment's contribution

  if(horizontal)
  {
    for(int i = 1; i < 5; ++i)
    {
      result += texture(BrightColor, texture_coordinates + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(BrightColor, texture_coordinates - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
  }
  else
  {
    for(int i = 1; i < 5; ++i)
    {
      result += texture(BrightColor, texture_coordinates + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
      result += texture(BrightColor, texture_coordinates - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
  }

  color = vec4(result, 1.0f);

}