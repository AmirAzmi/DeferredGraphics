#include "AABB.h"
#include <algorithm>

std::vector<glm::vec3> AABB::isContained(const glm::vec3* points, size_t size, const AABB& boundingVolume)
{
  std::vector<glm::vec3> contained_points;
  contained_points.reserve(128);

  for (int i = 0; i < size; ++i)
  {
    if (points[i].x >= boundingVolume.min.x && points[i].x <= boundingVolume.max.x)
    {
      if (points[i].y >= boundingVolume.min.y && points[i].y <= boundingVolume.max.y)
      {
        if (points[i].z >= boundingVolume.min.z && points[i].z <= boundingVolume.max.z)
        {
          contained_points.push_back(points[i]);
        }
      }
    }
  }

  return contained_points;
}

void AABB::Empty()
{
  //set the min and max to its extreme values and also reset and previous values
  max.x = max.y = max.z = -INFINITY;
  min.x = min.y = min.z = INFINITY;
}

//gets the center of the AABB
const glm::vec3 AABB::getCenter()
{
  return (min + max) * .5f;
}

//dimension
const glm::vec3 AABB::getSize()
{
  return max - min;
}

const glm::vec3 AABB::getRadius()
{
  return max - getCenter();
}

const AABB AABB::getSquareBounds()
{
  AABB bounds;

  //get longest side
  glm::vec3 LongestAxis(max - min);

  //pick a side and get its half vector value
  float LongestAxisHalf = std::max(std::max(LongestAxis.x, LongestAxis.y), LongestAxis.z) * .5f;

  //center + Longest Axis Radius
  bounds.min.x = getCenter().x - LongestAxisHalf;
  bounds.min.y = getCenter().y - LongestAxisHalf;
  bounds.min.z = getCenter().z - LongestAxisHalf;
                                 
  bounds.max.x = getCenter().x + LongestAxisHalf;
  bounds.max.y = getCenter().y + LongestAxisHalf;
  bounds.max.z = getCenter().z + LongestAxisHalf;

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



