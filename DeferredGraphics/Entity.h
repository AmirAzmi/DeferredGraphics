#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>
#include <string>
#include <utility>
#include "EngineTypes.h"
#include "Scene.h"

class Entity
{
  MeshComponentPtr meshComponent;
  LightComponentPtr lightComponent;

public:
  Scene& scene;
  std::string name;
  glm::vec3 position;
  glm::vec3 axisOfRotation;
  glm::vec3 scale;
  float angle;

  Entity(std::string name, Scene& scene);

  template<typename T, typename ...Ts>
  T* add(Ts&&... args);

  template<typename T>
  T* get();

  template <typename T>
  void remove();

};

//add components to the entity
template<typename T, typename ...Ts>
inline T* Entity::add(Ts&&... args)
{
  //check to add meshComponent note: make sure to compare types not pointers of types
  if constexpr (std::is_same_v<T, MeshComponent>)
  {
    //allocate the component
    T* component = new T(std::forward<Ts>(args)...);

    //set the pointer of the meshComponent to be a valid pointer
    meshComponent = component;

    //add the component to the T list and in this case T is MeshComponent
    scene.ListOfTypes.get<T*>().push_back(component);

    //return the component
    return meshComponent;
  }

  //check to add lightComponent note: make sure to compare types not pointers of types
  if constexpr (std::is_same_v<T, LightComponent>)
  {
    //allocate the component
    T* component = new T(std::forward<Ts>(args)...);

    //set the pointer of the meshComponent to be a valid pointer
    lightComponent = component;

    //add the component to the T list and in this case T is MeshComponent
    scene.ListOfTypes.get<T*>().push_back(component);

    //return the component
    return lightComponent;
  }

  //passed in something that was not a component
  return nullptr;
}

//checking if an entity has the component
template<typename T>
inline T* Entity::get()
{

  //check to see if meshComponent was passed in
  if constexpr (std::is_same_v<T, MeshComponent>)
  {
    //check if the meshComponent is valid
    if (meshComponent != nullptr)
    {
      //give the meshComponent
      return meshComponent;
    }
  }

  //check to see if lightComponent was passed in
  if constexpr (std::is_same_v<T, LightComponent>)
  {
    //check if the lightComponent is valid
    if (lightComponent != nullptr)
    {
      //give the lightComponent
      return lightComponent;
    }
  }

  return nullptr;
}

//remove components from the entity
template<typename T>
inline void Entity::remove()
{
  if constexpr (std::is_same_v<T, MeshComponentPtr>)
  {
    delete meshComponent;
  }

  if constexpr (std::is_same_v<T, LightComponentPtr>)
  {
    delete lightComponent;
  }
}

using EntityPtr = Entity*;