#pragma once
#include "AABB.h"
#include "MeshComponent.h"
#include "Scene.h"
class BoundingVolumeHierarchy
{
public:
  BoundingVolumeHierarchy * left_child;
  BoundingVolumeHierarchy * right_child;
  std::vector<MeshComponentPtr> meshes;
  AABB boundingVolume;

  BoundingVolumeHierarchy(const std::vector<MeshComponentPtr> meshes);
  AABB calculateBoundingVolume(const std::vector<MeshComponentPtr> meshes);
  BoundingVolumeHierarchy * createNode(const std::vector<MeshComponentPtr> meshes);
  ~BoundingVolumeHierarchy();
  bool isLeaf();

};