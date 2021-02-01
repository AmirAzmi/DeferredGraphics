#pragma once
#include <glm/glm/glm.hpp>

/*
  Plane Equation: Ax + By + Cz + D  = 0
  N(A,B,C) dot point + D = 0 => N dot point = -D
*/

class Plane
{
public:
  glm::vec3 Normal;
  float DistanceScalar;

  Plane(glm::vec3 norm, float d) :Normal(norm), DistanceScalar(d)
  {
  }
  Plane(float a, float b, float c, float d)
  {
    Normal.x = a;
    Normal.y = b;
    Normal.z = c;
    DistanceScalar = d;
  }
  Plane(glm::vec4 planeEq)
  {
    Normal.x = planeEq.x;
    Normal.y = planeEq.y;
    Normal.z = planeEq.z;
    DistanceScalar = planeEq.w;
  }
  Plane():Normal(glm::vec3()), DistanceScalar(0.0f)
  {
  }

  void NormalizePlane()
  {
    float normalizedNormal = glm::length(Normal);

    Normal = Normal / normalizedNormal;
    DistanceScalar = DistanceScalar / normalizedNormal;
  }

  inline const glm::vec3 getNormal() const
  {
    return Normal;
  };

  inline const float getDistance() const
  {
    return DistanceScalar;
  };

  inline bool PointAbovePlane(const glm::vec3& point) const
  {
    if (glm::dot(Normal, point) + DistanceScalar > 0.0f)
    {
      return true;
    }

    return false;
  }

  inline bool PointOnPlane(const glm::vec3& point) const
  {
    if (glm::dot(Normal, point) + DistanceScalar == 0.0f)
    {
      return true;
    }

    return false;
  }

  inline bool PointBelowPlane(const glm::vec3& point) const
  {
    if (glm::dot(Normal, point) + DistanceScalar < 0.0f)
    {
      return true;
    }

    return false;
  }

  inline bool PointAboveOnPlane(const glm::vec3& point) const
  {
    if (glm::dot(Normal, point) + DistanceScalar >= 0.0f)
    {
      return true;
    }

    return false;
  }

  inline bool PointBelowOnPlane(const glm::vec3& point) const
  {
    if (glm::dot(Normal, point) + DistanceScalar <= 0.0f)
    {
      return true;
    }

    return false;
  }
};

