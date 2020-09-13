#include "BSP.h"

BSP::BSP(const std::vector<Polygon> polygonList) :ListofPolygons(polygonList)
{
  Front = nullptr;
  Back = nullptr;
}

BSP* BSP::BSPcreateNode(const std::vector<Polygon> polygonList)
{
  BSP* BSPNode = linearAllocator.TAllocate<BSP>(polygonList);
  return BSPNode;
}

std::vector<BSP::Polygon> BSP::createPolygonList(std::vector<MeshComponentPtr> meshes)
{
  std::vector<BSP::Polygon> polygonList;
  std::vector<GLint> indices;

  for (auto& mesh : meshes)
  {
    for (auto & m : mesh->getMesh()->meshes)
    {
      //get all indices of the mesh
      std::vector<GLuint> indices = m.getIndices();
      std::vector<glm::vec3> vertices = m.getVertices();

      //for all indices of the mesh
      for (int i = 0; i < indices.size(); i += 3)
      {
        //store every 3 into a polygon
        Polygon polygon;
        polygon.point1 = vertices[indices[i + 0]];
        polygon.point2 = vertices[indices[i + 1]];
        polygon.point3 = vertices[indices[i + 2]];

        //store the polygon into the polygonlist
        polygonList.push_back(polygon);
      }
    }
  }

  return polygonList;
}

BSP::~BSP()
{
  linearAllocator.TDeallocate<BSP>(Front);
  linearAllocator.TDeallocate<BSP>(Back);
}
