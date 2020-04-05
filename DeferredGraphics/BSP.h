#pragma once
#include <glm/glm/glm.hpp>
#include <vector>
class BSP
{
public:
  BSP* parent;
  std::vector<glm::vec3> pointsInPolygon; //does this contains all the points in the scene???
  BSP* Back;
  BSP* Front;


};

