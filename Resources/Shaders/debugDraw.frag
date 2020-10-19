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

out vec4 color;
in vec4 fragment_color;
uniform float level;
uniform bool octree;

vec4 setColor(float color_value)
{
  if(color_value == 0)
  {
    return vec4(0.0, 0.0, 0.0, 1.0);
  }

   if(color_value == 1)
  {
    return vec4(1.0, 1.0, 1.0, 1.0);
  }

  if(color_value == 2)
  {
     return vec4(1.0, 0.0, 0.0, 1.0);
  }

  if(color_value == 3)
  {
     return vec4(0.0, 1.0, 0.0, 1.0);
  }

  if(color_value == 4)
  {
     return vec4(0.0, 0.0, 1.0, 1.0);
  }

  if(color_value == 5)
  {
     return vec4(1.0, 1.0, 0.0, 1.0);
  }

  if(color_value == 6)
  {
     return vec4(1.0, 0.0, 1.0, 1.0);
  }

  if(color_value == 7)
  {
     return vec4(0.0, 0.0, 1.0, 1.0);
  }
}

void main()
{
  if(octree == true)
  {
    color = setColor(level); 
  }
  else
  {
  
      color = fragment_color;
  }
}
