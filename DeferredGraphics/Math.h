#pragma once
#include <glm/glm/glm.hpp>
#include "Plane.h"
#include "Line.h"
#include "Quaternion.h"

namespace Math
{
  //return magnitude
  float PointOnLine(const glm::vec3 & point, const Line & line);

  //check if point is on plane
  bool PointPlane(const glm::vec3& point, const Plane& plane);

  //Get intersection point
  glm::vec3 LinePlaneIntersection(const Line& point, const Plane& plane);

  //get intersection vector of two planes
  glm::vec3 PlanePlane(const Plane & plane, const Plane & planetwo);

  glm::vec3 Reflect(glm::vec3 vector, glm::vec3 normal);
}
