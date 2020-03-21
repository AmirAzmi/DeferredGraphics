/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene.h
Purpose: Scene containing what needs to be updated/drawn
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include "TemplateUtils.h"
#include "EngineTypes.h"


class Scene
{
  std::vector<EntityPtr> ListOfEntities; //list of entities
  std::vector<AABB> listOfBounds;


public:
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
  glm::vec3 eyePosition; //camera position
  glm::vec3 cameraDirection;
  glm::vec2 windowDimension;

  float cameraSpeed;
  glm::vec3 upDirection;
  float nearDistance;
  float farDistance;
  float fov;

  ComponentLists ListOfTypes; //list of component types

  float positionUpdate = 0.0f;

  Scene(int windowWidth, int windowHeight);
  glm::mat4 getProjectionMatrix();
  glm::mat4 getViewMatrix();
  glm::vec3 getEyePosition();
  glm::vec3 getCameraDirection();
  glm::vec3 getUpDirection();
  float getNearDistance();
  float getFarDistance();
  float getFOV();
  std::vector<EntityPtr>& getEntities();
  std::vector<MeshComponentPtr>& getMeshes();
  std::vector<LightComponentPtr>& getLights();

  void Init();
  void PreRender(int windowWidth, int windowHeight);
  void Render();
  void PostRender();

  EntityPtr addEntity(std::string name);
  EntityPtr findEntity(std::string name);
  void removeEntity(std::string name);

  glm::vec2 getWindowDimension(int windowWidth, int WindowHeight)
  {
    return glm::vec2(windowWidth, WindowHeight);
  }

};

