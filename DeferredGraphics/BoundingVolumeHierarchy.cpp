#include "BoundingVolumeHierarchy.h"

BoundingVolumeHierarchy::BoundingVolumeHierarchy(std::vector<MeshComponentPtr> mesh_lisr)
{
  parent = this;
  left_child = nullptr;
  right_child = nullptr;
  boundingVolume = calculateBoundingVolume(meshes);
  meshes = mesh_lisr;
}

AABB BoundingVolumeHierarchy::calculateBoundingVolume(std::vector<MeshComponentPtr> meshes)
{
  std::vector<AABB> meshAABBs;

  for (auto & mesh: meshes)
  {
    meshAABBs.push_back(mesh->bounds);
  }

  boundingVolume = boundingVolume.combineBounds(meshAABBs);

  return boundingVolume;
}

BoundingVolumeHierarchy * BoundingVolumeHierarchy::createNode(std::vector<MeshComponentPtr> meshes)
{
  BoundingVolumeHierarchy * BVHNode = new BoundingVolumeHierarchy(meshes);
  BVHNode->parent = BVHNode;
  BVHNode->boundingVolume = BVHNode->calculateBoundingVolume(meshes);
  BVHNode->meshes = meshes;
  return BVHNode;
  
}

BoundingVolumeHierarchy::~BoundingVolumeHierarchy()
{
  delete left_child;
  delete right_child;
}

bool BoundingVolumeHierarchy::isLeaf()
{
  if (this->right_child == nullptr && left_child == nullptr)
  {
    return true;
  }

  return false;
}

