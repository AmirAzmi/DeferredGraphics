#include "Math.h"

namespace glm
{
  //project v onto n
  template <typename T>
  T project(const T& v, const T& n)
  {
    float dot = glm::dot(v, n);
    float mag_n = glm::length(n);

    float scalar = dot / (mag_n * mag_n);
    return n * scalar;
  }
}

float Math::PointOnLine(const glm::vec3& point, const Line& line)
{
  glm::vec3 AC = point - glm::vec3(line.start);
  glm::vec3 AB = glm::vec3(line.end) - glm::vec3(line.start);

  glm::vec3 dir = glm::project(AC, AB);

  return (glm::length(AC - dir));

}

bool Math::PointPlane(const glm::vec3& point, const Plane& plane)
{
  if (glm::dot(plane.Normal, point) + plane.DistanceScalar == 0.0f)
    return true;

  return false;
}

glm::vec3 Math::LinePlaneIntersection(const Line& line, const Plane& plane)
{
  glm::vec3 dir = glm::vec3(line.end - line.start);
  float t = ((plane.DistanceScalar + glm::dot(plane.Normal, glm::vec3(line.start))) / glm::dot(plane.Normal, dir));

  glm::vec3 point = glm::vec3(line.start) + t * (dir);

  if (plane.PointOnPlane(point))
  {
    return point;
  }

  return glm::vec3(0,0,0);
}

glm::vec3 Math::PlanePlane(const Plane& plane, const Plane& planetwo)
{
  return glm::cross(plane.Normal, planetwo.Normal);
}

glm::vec3 Math::Reflect(glm::vec3 vector, glm::vec3 normal)
{
  glm::vec3 perp = glm::project(vector, normal);
  glm::vec3 parallel = vector - perp;
  return parallel - perp;
}
