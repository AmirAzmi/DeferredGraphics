/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: gBuvver.vert
Purpose: passing texture from the gbuffer to the output data of the fragment shader
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 4) in ivec4 BoneIDs;
layout (location = 5) in vec4 Weights;

out vec3 world_position;
out vec3 world_normal_position;
out vec2 texture_coordinates;

const int MAX_BONES = 64;

uniform mat4 object_to_world;
uniform mat4 view_matrix;
uniform mat4 perspective_matrix;
uniform mat4 normal_matrix;
uniform mat4 final_bone_output[MAX_BONES];

void main()
{
    mat4 BoneTransform = final_bone_output[BoneIDs[0]] * Weights[0];
    BoneTransform += final_bone_output[BoneIDs[1]] * Weights[1];
    BoneTransform += final_bone_output[BoneIDs[2]] * Weights[2];
    BoneTransform += final_bone_output[BoneIDs[3]] * Weights[3];

  world_position = vec3(object_to_world * BoneTransform * vec4(aPos, 1.0f));
  world_normal_position = vec3(normal_matrix * vec4(aNormal, 0.0f)); 
  texture_coordinates = aTexCoords;

  gl_Position = perspective_matrix * view_matrix * vec4(world_position, 1.0f);
}