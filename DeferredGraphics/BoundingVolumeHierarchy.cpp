#include "BoundingVolumeHierarchy.h"

BoundingVolumeHierarchy::BoundingVolumeHierarchy(Scene & scene)
{
  children[0] = nullptr;
  children[1] = nullptr;
  meshes = scene.getMeshes();
  boundingVolume.combineBounds(scene.getSceneObjectBounds()); //gets total object bounds from the scene
}

