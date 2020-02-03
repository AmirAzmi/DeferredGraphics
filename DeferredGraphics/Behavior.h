/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Behavior.h
Purpose: Scene containing what needs to be updated/drawn
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#pragma once
class Behavior
{
  virtual void OnFrameBegin();
  virtual void Update() = 0;
  virtual void OnFrameEnd();

  template<typename T, typename ...Ts>
  T add();

  template<typename T>
  T remove();
};

template<typename T, typename ...Ts>
inline T Behavior::add()
{
  return T();
}

template<typename T>
inline T Behavior::remove()
{
  return T();
}
