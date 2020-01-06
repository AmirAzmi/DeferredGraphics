#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>
#include <string>
#include <utility>
#include "EngineTypes.h"

class Scene;

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

  template<typename T, typename ...Ts>
  T add(Ts&&... args);

  template <typename T>
  void remove();

};

//add components to the entity
template<typename T, typename ...Ts>
inline T Entity::add(Ts&&... args)
{
  //check to add meshComponent note: make sure to compare types not pointers of types
  if constexpr (std::is_same_v<T, MeshComponent>)
  {
    //allocate the component
    T* component = new T(std::forward<Ts>(args)...);

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
inline void Entity::remove()
{
  if constexpr (std::is_same_v<T, MeshComponentPtr>)
  {
    delete meshComponent;
  }
}

using EntityPtr = Entity*;