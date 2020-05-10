#pragma once
#include <glm/glm/glm.hpp>
#include "AABB.h"
class Raycast
{

  //Ray information
  glm::vec3 origin;
  glm::vec3 destination;

  bool RayBoxIntersection(AABB bounds);
};

//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
