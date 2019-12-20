#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>
#include <string>

#include "MeshComponent.hpp"
#include "Scene.h"

class Entity
{
  MeshComponentPtr meshComponent;
  Scene& scene;

public:
  std::string name;
  glm::vec3 position;
  glm::vec3 axisOfRotation;
  glm::vec3 scale;
  float angle;

  Entity(std::string name, Scene& scene);

  template<typename T>
  T add(T component);

  template <typename T>
  void remove(T component);

};

//add components to the entity
template<typename T>
inline T Entity::add(T component)
{
  //check to add meshComponent
  if constexpr (std::is_same_v<T, MeshComponentPtr>)
  {
    //set the pointer of the meshComponent to be a valid pointer
    meshComponent = component; 

    //return the component
    return meshComponent;
  }

  //passed in something that was not a component
  return nullptr;

}

//remove components from the entity
template<typename T>
inline void Entity::remove(T component)
{
  //if a valid component is to be removed
  if (component)
  {
    //set that component to be null
    component = nullptr;
  }
}

using EntityPtr = Entity*;
