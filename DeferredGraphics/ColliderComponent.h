#include <glm/glm/glm.hpp>


class ColliderComponent
{
  //bounds struct contation bottom left and top right for a box collider type
  struct bounds
  {
    glm::vec3 min_extent;
    float pad;
    glm::vec3 max_extent;
    float pad_1;
  };

  //component data
  bounds extents;
  bool isTrigger;
  bool enabled;


};
