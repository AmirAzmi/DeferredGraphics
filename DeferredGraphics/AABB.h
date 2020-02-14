#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>

class AABB
{
  public:
  glm::vec3 min;
  glm::vec3 max;

  void Add(glm::vec3 &point);
  void Empty();
  glm::vec3 getCenter();
  glm::vec3 getSize();
  glm::vec3 getRadius();
};