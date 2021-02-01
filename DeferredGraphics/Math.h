#pragma once
#include <glm/glm/glm.hpp>
#include "Plane.h"
#include "Line.h"
#include "Quaternion.h"

namespace Math
{
  bool PointOnLine(const glm::vec3 & point, const Line & line);

  bool PointPlane(const glm::vec3& point, const Plane& plane);
  bool LinePlane(const Line& point, const Plane& plane);
  bool PlanePlane(const Plane & plane, const Plane & planetwo);

}

