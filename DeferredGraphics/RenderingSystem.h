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
#include <chrono>

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
  GLuint texture; ////////
private:

  //lighting Pass FBO ID
  GLuint LightingPassFBOID;
public:
  GLuint BrightBufferID;
  GLuint ColorBufferID;
private:

  //Bloom FBO ID
  GLuint PingPongFBO[2];

public:
  GLuint PingPongColorBuffer[2];
private:

  //Final Output FBO ID
  GLuint FinalFBOID;
public:
  GLuint FinalColorBufferID;
private:
  //GLuint PostProcessingFBOID;
  AABB bounds;

  //depth buffer ID for the gBuffer
  GLuint rboDepthID;

public:
  //depth buffer ID for the FinalOutput Buffer
  GLuint rboDepthID2;

  //lighting Pass FBOID
  GLuint rboDepthID3;
private:

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
  float exposure = 1.0f;
  bool bloom = false;
  bool gamma = true;
  bool exposure_tone_mapping = true;
  bool uncharted_tone_mapping = false;
  bool splitScreen = false;
  bool depthCopyToggle = true;
  bool brightBuffer = false;
  std::chrono::duration<double, std::milli> rendering_sytem_elapsed_time;

  RenderingSystem(const int windowWidth, const int windowHeight);
  ~RenderingSystem();
  void Update(Scene& scene, const int windowWidth, const int windowHeight);
  void Draw(MeshComponentPtr mesh, Scene& scene, bool isDeffered);
  void DrawQuad();
  void DrawTextures(GLuint textureID, const unsigned posX, const unsigned posY, const unsigned windowWidth, const unsigned windowHeight);
};