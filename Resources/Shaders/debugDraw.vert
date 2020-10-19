/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: forwardLightingPass.vert
Purpose: vertex shader for forward lighting
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#version 440 core
layout (location = 0) in vec3 aPos;

out vec3 world_position;
out vec4 fragment_color; 
uniform mat4 view_matrix;
uniform mat4 perspective_matrix;
uniform int last_index;

void main()
{
  world_position = vec3(vec4(aPos, 1.0f));

  if(gl_VertexID < 5 || gl_VertexID == last_index - 1)
  {
    fragment_color = vec4(0.0f, 0.0f, 0.0f, 1.0f); 
  }
  else
  {
    fragment_color = vec4(1.0f, 1.0f, 1.0f, 1.0f); 
  }

  gl_Position = perspective_matrix * view_matrix * vec4(world_position, 1.0f);
}