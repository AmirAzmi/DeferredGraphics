/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: gBuffer.frag
Purpose:managing geometry so we can pass it into the deffered renderer
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 world_position;
in vec3 world_normal_position;
in vec2 texture_coordinates;

uniform vec4 diffuse_color;
uniform float specular_intensity;

out vec3 color;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = world_position;

    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(world_normal_position);

    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = diffuse_color.rgb;

    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = specular_intensity;

}
