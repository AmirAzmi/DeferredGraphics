/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Entity.h
Purpose: Entity part of the ECS system
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>
#include <string>
#include <utility>
#include "EngineTypes.h"
#include "Scene.h"
#include "Quaternion.h"

class Behavior;

class Entity
{
public:
  Entity* parent;
  std::vector<Entity*> children;
  std::vector<BehaviorPtr> behaviors; // The list of behaviors attached to this entity.
  ComponentPointerLists pointers;
  Scene& scene;
  std::string name;
  glm::vec3 position;
  glm::vec3 axisOfRotation;
  glm::vec3 scale;
  glm::mat4 objectToWorld; //world space
  glm::mat4 localToWorld; //local space
  float angle;
  float currentPosition;

  glm::mat4 getObjectToWorld()
  {
    Quaternion q;
    q.setToRotatAboutAxis(glm::normalize(axisOfRotation), glm::radians(angle));
    q.normalized();

    localToWorld = glm::translate(position) * q.quaternionToMatrix() * glm::scale(scale);

    if (parent == nullptr)
    {
      objectToWorld = localToWorld;
    }
    else
    {
      objectToWorld = parent->objectToWorld * localToWorld;
    }

    return objectToWorld;

    //return glm::translate(position) * glm::rotate(glm::radians(angle), axisOfRotation) * glm::scale(scale);
  }

  Entity(std::string name, Scene& scene);

  template<typename T, typename ...Ts>
  T* add(Ts&&... args);

  template<typename T>
  T* get();

  template <typename T>
  void remove();

  template<typename T, typename ...Ts>
  T* addBehavior(Ts&&... args)
  {
    return static_cast<T*>(addBehaviorImpl(new T(args...)));
  }

  void onFrameBegin(float delta_time);
  void update(float delta_time);
  void onFrameEnd(float delta_time);

  ~Entity();

private:
  BehaviorPtr addBehaviorImpl(BehaviorPtr behavior);
};

//add components to the entity
template<typename T, typename ...Ts>
inline T* Entity::add(Ts&&... args)
{
  //allocate the component with the arguments necessary from std::forward
  T* component = new T(std::forward<Ts>(args)...);

  //adds componentptr to list of component pointers by getting the component pointer from the component
  pointers.get<T*>().pointer = component;

  //add the component to the T list and in this case T is MeshComponent
  scene.ListOfTypes.get<T*>().push_back(component);

  //return the component
  return component;
}

//checking if an entity has the component
template<typename T>
inline T* Entity::get()
{
  //get the pointer type of that component
  return pointers.get<T*>().pointer; //calls tuple get which can only get unique types
}

//remove components from the entity
template<typename T>
inline void Entity::remove()
{
  //grab the adress of the component
  T* pointer = pointers.get<T*>().pointer;

  //grab the list of that specific component
  auto& list = scene.ListOfTypes.get<T*>();

  //get the iterator of that component
  auto it = std::find(list.begin(), list.end(), pointer);

  //erase it from the list
  list.erase(it);

  //delete the pointer
  delete pointer;

  //invalidate it by setting it to null
  pointers.get<T*>().pointer = nullptr;
}

using EntityPtr = Entity*;