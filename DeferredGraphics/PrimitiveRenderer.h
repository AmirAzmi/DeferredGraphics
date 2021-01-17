#pragma once
#include <glm/glm/glm.hpp>
#include <vector>
#include "EngineTypes.h"

class Scene;

class PrimitiveRenderer
{
  public:
  std::vector<EntityPtr> m_PrimitiveEntities;

  PrimitiveRenderer()
  {
  }

  void createRectPrism(glm::vec3 scalar_dimension, Scene & m_Scene);
  void createSphere(float radius);
  void createCube(float scalar);
};

