#pragma once
#include <glm/glm/glm.hpp>
#include <glew/GL/glew.h>
#include <vector>
class AABB
{
  public:
  glm::vec3 min;
  glm::vec3 max;

  AABB()
  {
    Empty();
  }

  AABB(glm::vec3 min, glm::vec3 max): min(min), max(max)
  {
  }

  void Empty();
  const glm::vec3 getCenter();
  const glm::vec3 getSize();
  const glm::vec3 getRadius();

  const AABB getSquareBounds();//updates AABB with every point added
  inline void Add(const glm::vec3& point)
  {
    if (point.x < min.x)
    {
      min.x = point.x;
    }
    if (point.x > max.x)
    {
      max.x = point.x;
    }
    if (point.y < min.y)
    {
      min.y = point.y;
    }
    if (point.y > max.y)
    {
      max.y = point.y;
    }
    if (point.z < min.z)
    {
      min.z = point.z;
    }
    if (point.z > max.z)
    {
      max.z = point.z;
    }
  }

  //This manipulates the total points passed in as well return the set of points of that bounding box
  static std::vector<glm::vec3> isContained(const glm::vec3* points, size_t size, const AABB& boundingVolume);

};