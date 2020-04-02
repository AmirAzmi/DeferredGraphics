#include "Octree.h"

Octree::Octree(AABB boundingBox, std::vector<MeshComponentPtr> meshes) :boundingVolume(boundingBox), meshes(meshes), parent(nullptr)
{
  //points is empty
  points.clear();

  //set all children to null for now
  for (auto& child : children)
  {
    child = nullptr;
  }
}

Octree::Octree(std::vector<glm::vec3> points) :points(points), parent(nullptr)
{
  //set all children to null for now
  for (auto& child_octree : children)
  {
    child_octree = nullptr;
  }
}

Octree* Octree::createOctreeNode(AABB bv, const std::vector<glm::vec3>& points, Octree* parent)
{
  if (points.size() > 2)
  {
    Octree* octree = new Octree(points); //sets children to null and points
    octree->parent = parent; //sets the parent
    octree->boundingVolume = bv; //sets the bounding volume
    return octree;
  }
  else
  {
    return nullptr;
  }

}

Octree::~Octree()
{
  for (int i = 0; i < children.size(); ++i)
  {
    delete children[i];
  }
}

