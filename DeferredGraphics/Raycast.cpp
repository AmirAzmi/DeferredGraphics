#include "Raycast.h"
#include <algorithm>

bool Raycast::RayBoxIntersection(AABB bounds)
{
  //direction from origin
  glm::vec3 direction_from_origin = destination - origin;

  //ray
  glm::vec3 ray = origin + (direction_from_origin);

  //get the t values from the min bounds for x and y => we will get z later
  float tminx = (bounds.min.x - origin.x) / direction_from_origin.x;
  float tminy = (bounds.min.y - origin.y) / direction_from_origin.y;

  //get the max of the t values for x and y
  float greatest_t_min = std::max(tminx, tminy);

  //get the t values from the max bounds fro x and y => we will getz later
  float tmaxx = (bounds.max.x - origin.x) / direction_from_origin.x;
  float tmaxy = (bounds.max.y - origin.y) / direction_from_origin.y;

  //what if t1 is smaller than t0
  if (tminx > tmaxx)
  {
    std::swap(tminx, tmaxx);
  }

  if (tminy > tmaxy)
  {
    std::swap(tminy, tmaxy);
  }

  //get the min of the t values
  float smallest_t_max = std::min(tmaxx, tmaxy);

  //if t0x > t1y this ray does not hit the cube so therefore you need to return false => draw the diagram to see this
  //vice versa is also true
  if (tminx > tmaxy || tminy > tmaxx)
  {
    return false;
  }

  //get the t values for the min andmax bounds z
  float tminz = (bounds.min.z - origin.z) / direction_from_origin.z;
  float tmaxz = (bounds.max.z - origin.z) / direction_from_origin.z;


  if (tminz > tmaxz)
  {
    std::swap(tminz, tmaxz);
  }

  //extendinf the idea of the cases where the ray can be wrong in the z direction
  if (greatest_t_min > tmaxz || tminz > smallest_t_max)
  {
    return false;
  }

  //getting largest t with z in mind for min bounds
  if (tminz > greatest_t_min)
  {
    greatest_t_min = tminz;
  }

  //getting smallest t with z in mind for max bounds
  if (tmaxz > smallest_t_max)
  {
    smallest_t_max = tmaxz;
  }

  //
  return false;
}
