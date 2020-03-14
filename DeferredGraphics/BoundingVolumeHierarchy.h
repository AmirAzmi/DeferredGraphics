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

  BoundingVolumeHierarchy(std::vector<MeshComponentPtr> meshes);
  AABB calculateBoundingVolume(std::vector<MeshComponentPtr> meshes);
  BoundingVolumeHierarchy * createNode(std::vector<MeshComponentPtr> meshes);
  bool isLeaf();

};