#include "AABB.h"

void AABB::Empty()
{
  //set the min and max to its extreme values and also reset and previous values
  max.x = max.y = max.z = -INFINITY;
  min.x = min.y = min.z = INFINITY;
}

//gets the center of the AABB
glm::vec3 AABB::getCenter()
{
  return (min + max) * .5f;
}

//dimension
glm::vec3 AABB::getSize()
{
  return max - min;
}

glm::vec3 AABB::getRadius()
{
  return max - getCenter();
}

AABB AABB::getSquareBounds()
{
  AABB bounds;

  //get longest side
  glm::vec3 LongestAxis(max - min);

  //pick a side and get its half vector value
  float LongestAxisRadius = LongestAxis.x * .5f;

  //center + Longest Axis Radius
  bounds.min.x = getCenter().x - LongestAxisRadius;
  bounds.min.y = getCenter().y - LongestAxisRadius;
  bounds.min.z = getCenter().z - LongestAxisRadius;

  bounds.max.x = getCenter().x + LongestAxisRadius;
  bounds.max.y = getCenter().y + LongestAxisRadius;
  bounds.max.z = getCenter().z + LongestAxisRadius;

  return bounds;
}

//updates AABB with every point added
void AABB::Add(const glm::vec3& point)
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
