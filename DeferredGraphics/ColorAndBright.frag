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

void main()
{
  color = texture(ucolor, texture_coordinates);
}