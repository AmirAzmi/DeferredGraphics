#pragma once
#include <glm/glm/glm.hpp>
#include <vector>

class BoundingSphere
{
public:
  enum class BoundingSphereCalculationType
  {
    None,
    Centroid,
    Ritters,
    ModifiedLarsons,
    PCA
  };

  struct BSInfo
  {
    glm::vec3 center;
    float radius;
  };

  BSInfo info; //information containing center and radius
  BoundingSphereCalculationType type; //what emthod to construct the center and sphere


public:
  BoundingSphere() :info{ glm::vec3(0.0f,0.0f,0.0f), 0.0f }, type(BoundingSphereCalculationType::Centroid)
  {
  }

  BoundingSphere(BSInfo information, BoundingSphereCalculationType type, int latitudeSlices, int longitudeSlices) :info{ information.center, information.radius }, type(BoundingSphereCalculationType::Centroid)
  {
  }

  BSInfo calculateBS(BoundingSphereCalculationType type, const std::vector<glm::vec3> vertices);
};

