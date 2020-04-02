#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>
#include <vector>
class AABB
{
  public:
  glm::vec3 min;
  glm::vec3 max;

  AABB() :min(glm::vec3(0.0f)),max(glm::vec3(0.0f))
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
  AABB getBounds();
  [[nodiscard]] std::vector<glm::vec3> isContained(std::vector<glm::vec3> & points, AABB boundingVolume);
  AABB getSquareBounds();
  AABB combineSquareBounds(AABB a, AABB b);
  AABB combineSquareBounds(std::vector<AABB> bounds);

  AABB combineBounds(AABB a, AABB b);
  AABB combineBounds(std::vector<AABB> bounds);
};