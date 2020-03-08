#pragma once
#include "AABB.h"
#include "MeshComponent.h"
#include "Scene.h"
class BoundingVolumeHierarchy
{
  BoundingVolumeHierarchy * children[2];
  std::vector<MeshComponentPtr> meshes;
  AABB boundingVolume;

public:
  BoundingVolumeHierarchy(Scene & scene);
};