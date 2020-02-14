#include "AABB.h"

void AABB::Empty()
{
  //set the min and max to its extreme values and also reset and previous values
  max.x = max.y = max.z = (std::numeric_limits<float>::min());
  min.x = min.y = min.z = (std::numeric_limits<float>::max());
}

//gets the center of the AABB
glm::vec3 AABB::getCenter()
{
  return (min + max) / 2.0f;
}

glm::vec3 AABB::getSize()
{
  return max - min;
}

glm::vec3 AABB::getRadius()
{
  return max - getCenter();
}

//updates AABB with every point added
void AABB::Add(glm::vec3 &point)
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
