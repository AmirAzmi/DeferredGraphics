#pragma once
#include "Scene.h"

class DebugRenderingSystem
{
  //projection and view matrix ID
  GLuint projectionMatrixID;
  GLuint viewMatrixID;

  //AABB Bounding Box data
  GLuint boundingBoxVAOID = 0;
  GLuint boundingBoxVBOID;
  GLuint boundingBoxIBOID;

  //shader used for rendering the lines
  ShaderHandle debugDrawID;

  public:
  DebugRenderingSystem(int windowWidth, int windowHeight);
  ~DebugRenderingSystem();
  void Update(Scene& scene, int windowWidth, int windowHeight);

  bool isAABBOn = false;
  bool isOBBOn = false;
  bool isSBBOn = false;

  void drawAABB(MeshComponentPtr mesh, Scene& scene);
  void drawOBB(MeshComponentPtr mesh, Scene& scene);
  void drawSBB(MeshComponentPtr mesh, Scene& scene);

};