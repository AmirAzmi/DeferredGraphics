#include "Raycast.h"
#include <algorithm>

bool Raycast::RayBoxIntersection(AABB bounds)
{
  //direction from origin
  glm::vec3 direction_from_origin = destination;

  //ray
  glm::vec3 ray = origin + (direction_from_origin);

  float tmin = (bounds.min.x - ray.x) / direction_from_origin.x;
  float tmax = (bounds.max.x - ray.x) / direction_from_origin.x;

  if (tmin > tmax) std::swap(tmin, tmax);

  float tymin = (bounds.min.y - ray.y) / direction_from_origin.y;
  float tymax = (bounds.max.y - ray.y) / direction_from_origin.y;

  if (tymin > tymax) std::swap(tymin, tymax);

  if ((tmin > tymax) || (tymin > tmax))
    return false;

  if (tymin > tmin)
    tmin = tymin;

  if (tymax < tmax)
    tmax = tymax;

  float tzmin = (bounds.min.z - ray.z) / direction_from_origin.z;
  float tzmax = (bounds.max.z - ray.z) / direction_from_origin.z;

  if (tzmin > tzmax) std::swap(tzmin, tzmax);

  if ((tmin > tzmax) || (tzmin > tmax))
    return false;

  if (tzmin > tmin)
    tmin = tzmin;

  if (tzmax < tmax)
    tmax = tzmax;

  return true;
}

