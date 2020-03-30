#include "Octree.h"

Octree::Octree(AABB boundingBox, std::vector<MeshComponentPtr> meshes):boundingVolume(boundingBox),meshes(meshes), parent(nullptr)
{
  //points is empty
  points.clear();

  //set all children to null for now
  for (auto & child : children)
  {
    child = nullptr;
  }

  //set the active children to 0
  active_children = std::byte();
}

Octree::Octree(std::vector<glm::vec3> points) :points(points),parent(nullptr)
{
  
  for (auto & point : points)
  {
    boundingVolume.Add(point);
  }

  //set all children to null for now
  for (auto& child_octree : children)
  {
    child_octree = nullptr;
  }

  //set the active children to 0
  active_children = std::byte();
}

