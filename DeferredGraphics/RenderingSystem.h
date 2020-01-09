#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <vector>
#include "Scene.h"
#include "LightComponent.h"

class RenderingSystem
{
  //projection and view matrix ID
  GLuint projectionMatrixID;
  GLuint viewMatrixID;

  //deffered rendering ID
  GLuint gBufferFBOID;
  GLuint gPositionID;
  GLuint gNormalID;
  GLuint gColorSpecID;

  //depth buffer ID
  GLuint rboDepthID;
  
  //shader handle for deffered pass and forward pas
  ShaderHandle defferedLightingShaderID;
  ShaderHandle forwardLightingShaderID;

  //lighting info ID
  GLuint ssboID[2];
  GLuint blockIndex[2];
  GLuint ssboBindingPointIndex[2];

  struct shader_data_t
  {
    LightComponent::Light lights[16];
    int numberOfLights = 1;
  } shader_data;

  bool isDeffered;

public:        
  RenderingSystem(int windowWidth, int windowHeight);
  void Update(Scene& scene, int windowWidth, int windowHeight);
  void Draw(MeshComponentPtr mesh, Scene& scene, bool isDeffered);
};