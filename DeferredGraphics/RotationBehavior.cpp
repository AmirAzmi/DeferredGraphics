/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: RotationBehavior.h
Purpose: Scene containing what needs to be updated/drawn
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: February 16th , 2020
--------------------------------------------------------*/

#include "RotationBehavior.h"

#include <glm/glm/vec3.hpp>
#include "Entity.h"
#include "Input.h"
#include "Imgui/imgui.h"

void RotationBehavior::Update(float delta_time)
{
  owner->currentPosition += delta_time * position_speed;
  owner->angle += delta_time * rotation_speed;
  owner->position.x = cosf(owner->currentPosition) * 4.0f;
  owner->position.z = sinf(owner->currentPosition) * 4.0f;

}

void RotationBehavior::inspect()
{
  ImGui::DragFloat("Rotation Speed: ",&rotation_speed);
  ImGui::DragFloat("Position Speed: ",&position_speed);
}
