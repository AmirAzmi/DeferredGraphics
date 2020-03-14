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

#pragma once

#include "Behavior.h" // Behavior


//
// This behavior rotates the entity in a circle.
//
class RotationBehavior : public Behavior
{
public:
  void Update() override;
};
