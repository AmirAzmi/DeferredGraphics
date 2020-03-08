#pragma once
#include "Scene.h"
#include "AABB.h"
#include "BoundingSphere.h"


class DebugRenderingSystem
{
  //projection and view matrix ID
  GLuint projectionMatrixID;
  GLuint viewMatrixID;

  //AABB Bounding Box data
  GLuint boundingBoxVAOID = 0;
  GLuint boundingBoxVBOID;
  GLuint boundingBoxIBOID;

  //Bounding Sphere data
  GLuint boundingSphereVAOID = 0;
  GLuint boundingSphereVBOID;
  GLuint boundingSphereIBOID;

  //shader used for rendering the lines
  ShaderHandle debugDrawID;

  public:
  DebugRenderingSystem(Scene & scene, int windowWidth, int windowHeight);
  ~DebugRenderingSystem();
  void Update(Scene& scene, int windowWidth, int windowHeight);

  //check for AABB
  bool isAABBOn = false;
  //draw the OBJ Vertices once
  bool isBSOn = false;
  BoundingSphere::BoundingSphereCalculationType sphereType = BoundingSphere::BoundingSphereCalculationType::Centroid;
  std::vector<std::vector<glm::vec4>> meshAABBs;

  void drawAABB(MeshComponentPtr mesh, Scene& scene);
  void drawBS(MeshComponentPtr mesh, Scene& scene, BoundingSphere::BoundingSphereCalculationType type);

};