#pragma once
#include "AABB.h"
#include "MeshComponent.h"
#include "Scene.h"
class BoundingVolumeHierarchy
{
public:
  BoundingVolumeHierarchy * left_child; //8 bytes
  BoundingVolumeHierarchy * right_child; //8 bytes
  std::vector<MeshComponentPtr> meshes;  //24 bytes
  AABB boundingVolume; //24 bytes

  BoundingVolumeHierarchy(const std::vector<MeshComponentPtr> meshes);
  AABB calculateBoundingVolume(const std::vector<MeshComponentPtr> meshes);
  BoundingVolumeHierarchy * createNode(const std::vector<MeshComponentPtr> meshes);
  ~BoundingVolumeHierarchy();
  bool isLeaf();

};