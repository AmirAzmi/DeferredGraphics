#include "BoundingVolumeHierarchy.h"

static AABB combineBounds(std::vector<AABB>& bounds)
{
  AABB totalBounds;
  totalBounds.Empty();

  for (int i = 0; i < bounds.size(); ++i)
  {
    if (bounds[i].min.x < totalBounds.min.x)
    {
      totalBounds.min.x = bounds[i].min.x;
    }

    if (bounds[i].min.y < totalBounds.min.y)
    {
      totalBounds.min.y = bounds[i].min.y;
    }

    if (bounds[i].min.z < totalBounds.min.z)
    {
      totalBounds.min.z = bounds[i].min.z;
    }

    if (bounds[i].max.x > totalBounds.max.x)
    {
      totalBounds.max.x = bounds[i].max.x;
    }

    if (bounds[i].max.y > totalBounds.max.y)
    {
      totalBounds.max.y = bounds[i].max.y;
    }

    if (bounds[i].max.z > totalBounds.max.z)
    {
      totalBounds.max.z = bounds[i].max.z;
    }
  }

  return totalBounds;
}

BoundingVolumeHierarchy::BoundingVolumeHierarchy(std::vector<MeshComponentPtr> mesh_lisr)
{
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

  boundingVolume = combineBounds(meshAABBs);

  return boundingVolume;
}

BoundingVolumeHierarchy * BoundingVolumeHierarchy::createNode(std::vector<MeshComponentPtr> meshes)
{
  BoundingVolumeHierarchy * BVHNode = new BoundingVolumeHierarchy(meshes);
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

