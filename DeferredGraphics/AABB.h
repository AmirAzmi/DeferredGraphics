#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>
#include <vector>
class AABB
{
  public:
  glm::vec3 min;
  glm::vec3 max;

  AABB():min(glm::vec3(0.0f)),max(glm::vec3(0.0f))
  {
  }

  AABB(glm::vec3 min, glm::vec3 max): min(min), max(max)
  {
  }

  void Add(const glm::vec3 &point);
  
  void Empty();
  glm::vec3 getCenter();
  glm::vec3 getSize();
  glm::vec3 getRadius();

  AABB getSquareBounds();
  //This manipulates the total points passed in as well return the set of points of that bounding box
  static std::vector<glm::vec3> isContained(const glm::vec3* points, size_t size, const AABB& boundingVolume);

};