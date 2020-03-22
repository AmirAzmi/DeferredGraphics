/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: RenderingSystem.h
Purpose: Deffered + forward rendering
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

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

  //gBuffer ID and FBO ID
  GLuint gBufferFBOID;
public:
  GLuint gPositionID;
  GLuint gNormalID;
  GLuint gColorSpecID;
private:

  //lighting Pass FBO ID
  GLuint LightingPassFBOID;
public:
  GLuint BrightBufferID;
  GLuint ColorBufferID;
private:

  //Bloom FBO ID
  GLuint PingPongFBO[2];
  GLuint PingPongColorBuffer[2];

  //Final Output FBO ID
  GLuint FinalFBOID;
public:
  GLuint FinalColorBufferID;
private:
  //GLuint PostProcessingFBOID;
  AABB bounds;

public:
  float exposure = 1.0f;
  bool bloom = false;
  bool gamma = true;
  bool exposure_tone_mapping = true;
  bool uncharted_tone_mapping = false;

private:

  //depth buffer ID for the gBuffer
  GLuint rboDepthID;

  //depth buffer ID for the FinalOutput Buffer
  GLuint rboDepthID2;

  //quad data
  GLuint quadVAOID = 0;
  GLuint quadVBOID;
  GLuint quadUVID;

  //split screen data
  GLuint splitScreenVAOID;
  GLuint splitScreenVBOID;
  GLuint splitscreenUVID;
  
  //shader handle for deffered pass and forward pas
  ShaderHandle colorAndBrightShaderID;
  ShaderHandle defferedLightingShaderID;
  ShaderHandle forwardLightingShaderID;
  ShaderHandle splitScreenShaderID;
  ShaderHandle gBufferShaderID;
  ShaderHandle bloomFinalID;
  ShaderHandle finalColorID;

  //lighting info ID
  GLuint ssboID[2];
  GLuint blockIndex[2];
  GLuint ssboBindingPointIndex[2];

  struct shader_data_t
  {
    LightComponent::Light lights[1000];
    int numberOfLights = 1;
  } shader_data;


public:        
  bool splitScreen = false;
  bool depthCopyToggle = true;
  bool brightBuffer = false;

  RenderingSystem(int windowWidth, int windowHeight);
  ~RenderingSystem();
  void Update(Scene& scene, int windowWidth, int windowHeight);
  void Draw(MeshComponentPtr mesh, Scene& scene, bool isDeffered);
  void DrawQuad();
  void DrawTextures(GLuint textureID, unsigned posX, unsigned posY, unsigned windowWidth, unsigned windowHeight);
};