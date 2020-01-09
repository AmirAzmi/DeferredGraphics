#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <vector>
#include "Scene.h"

class RenderingSystem
{
  GLuint projectionMatrixID;
  GLuint viewMatrixID;

  GLuint gBufferFBOID;
  GLuint gPositionID;
  GLuint gNormalID;
  GLuint gColorSpecID;

  GLuint rboDepthID;
  
  ShaderHandle defferedLightingShaderID;
  ShaderHandle forwardLightingShaderID;

  bool isDeffered;

public:        
  RenderingSystem(int windowWidth, int windowHeight);
  void Update(Scene& scene);
  void Draw(MeshComponentPtr mesh, Scene& scene, bool isDeffered);
};