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
  return (min + max) / 2.0f;
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

AABB AABB::getBounds()
{
  return AABB(min, max);
}

//This manipulates the total points passed in as well return the set of points of that bounding box
std::vector<glm::vec3> AABB::isContained(std::vector<glm::vec3> & points, AABB boundingVolume)
{
  std::vector<glm::vec3> contained_points;

  for (int i = 0; i < points.size(); ++i )
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

AABB AABB::getSquareBounds()
{
  AABB bounds;

  //get longest side
  glm::vec3 LongestAxis(max - min);

  //pick a side and get its half vector value
  float LongestAxisRadius = LongestAxis.x / 2.0f;

  //center + Longest Axis Radius
  bounds.min.x = getCenter().x - LongestAxisRadius;
  bounds.min.y = getCenter().y - LongestAxisRadius;
  bounds.min.z = getCenter().z - LongestAxisRadius;

  bounds.max.x = getCenter().x + LongestAxisRadius;
  bounds.max.y = getCenter().y + LongestAxisRadius;
  bounds.max.z = getCenter().z + LongestAxisRadius;

  return bounds;
}

AABB AABB::combineBounds(AABB a, AABB b)
{
  AABB bounds;

  if (a.min.x < b.min.x)
  {
    bounds.min.x = a.min.x;
  }
  else
  {
    bounds.min.x = b.min.x;
  }

  if (a.min.y < b.min.y)
  {
    bounds.min.y = a.min.y;
  }
  else
  {
    bounds.min.y = b.min.y;
  }

  if (a.min.z < b.min.z)
  {
    bounds.min.z = a.min.z;
  }
  else
  {
    bounds.min.z = b.min.z;
  }

  return bounds;
}

AABB AABB::combineBounds(std::vector<AABB> bounds)
{
  AABB totalBounds;
  totalBounds.Empty();

  for (int i = 0; i < bounds.size(); ++i)
  {
    if (bounds[i].min.x < totalBounds.min.x)
    {
      totalBounds.min.x = bounds[i].min.x;
    }

    if (bounds[i].min.y < totalBounds.min.y)
    {
      totalBounds.min.y = bounds[i].min.y;
    }

    if (bounds[i].min.z < totalBounds.min.z)
    {
      totalBounds.min.z = bounds[i].min.z;
    }

    if (bounds[i].max.x > totalBounds.max.x)
    {
      totalBounds.max.x = bounds[i].max.x;
    }

    if (bounds[i].max.y > totalBounds.max.y)
    {
      totalBounds.max.y = bounds[i].max.y;
    }

    if (bounds[i].max.z > totalBounds.max.z)
    {
      totalBounds.max.z = bounds[i].max.z;
    }
  }

  return totalBounds;
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
