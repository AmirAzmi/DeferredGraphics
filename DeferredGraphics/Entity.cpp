/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Entity.cpp
Purpose: Entity constructior
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#include "Entity.h"

#include "Behavior.h"

Entity::Entity(std::string name, Scene& scene):name(name), scene(scene), axisOfRotation(0,0,1), scale(1,1,1), angle(0.0f)
{
}

void Entity::onFrameBegin(float delta_time)
{
  for (BehaviorPtr behavior : behaviors)
  {
    behavior->OnFrameBegin(delta_time);
  }
}

void Entity::update(float delta_time)
{
  for (BehaviorPtr behavior : behaviors)
  {
    behavior->Update(delta_time);
  }
}

void Entity::onFrameEnd(float delta_time)
{
  for (BehaviorPtr behavior : behaviors)
  {
    behavior->OnFrameEnd(delta_time);
  }
}

Entity::~Entity()
{
  for (BehaviorPtr behavior : behaviors)
  {
    delete behavior;
  }

  behaviors.clear();
}

BehaviorPtr Entity::addBehaviorImpl(BehaviorPtr behavior)
{
  behavior->owner = this;
  behaviors.push_back(behavior);
  return behavior;
}
