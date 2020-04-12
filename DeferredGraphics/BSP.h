#pragma once
#include <glm/glm/glm.hpp>
#include <vector>
#include "MeshComponent.h"
#include "Memory.h"

class BSP
{

public:
  struct Polygon
  {
    glm::vec3 point1;
    glm::vec3 point2;
    glm::vec3 point3;
  };

  std::vector<Polygon> ListofPolygons; //every 3 indices is a polygon
  BSP* Back;
  BSP* Front;

  BSP(const std::vector<Polygon> polygonList);
  BSP * BSPcreateNode(const std::vector<Polygon> polygonList);
  std::vector<Polygon> createPolygonList(std::vector<MeshComponentPtr> meshes);
  ~BSP();
};

