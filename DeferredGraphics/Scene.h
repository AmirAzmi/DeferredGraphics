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
#include "PrimitiveRenderer.h"
#include "TemplateUtils.h"
#include "EngineTypes.h"


class Scene
{
  std::vector<EntityPtr> ListOfEntities; //list of entities
  std::vector<AABB> listOfBounds;


public:
  PrimitiveRenderer primRenderer;
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
  glm::mat4 newProjectionMatrix;
  glm::mat4 newViewMatrix;
  glm::vec3 eyePosition; //camera position
  glm::vec3 cameraDirection;
  glm::vec2 windowDimension;

  glm::mat4 screenToWorld;

  float timeScale = 1.0f;
  float cameraSpeed;
  glm::vec3 upDirection;
  float nearDistance;
  float farDistance;
  float fov;

  ComponentLists ListOfTypes; //list of component types

  float positionUpdate = 0.0f;

  Scene(const int windowWidth, const int windowHeight);
  const glm::mat4 getProjectionMatrix() const;
  const glm::mat4 getViewMatrix() const;
  const glm::vec3 getEyePosition() const;
  const glm::vec3 getCameraDirection() const;
  const glm::vec3 getUpDirection() const;
  const float getNearDistance() const;
  const float getFarDistance() const;
  const float getFOV() const;
  std::vector<EntityPtr>& getEntities();
  std::vector<MeshComponentPtr>& getMeshes();
  std::vector<LightComponentPtr>& getLights();
  const glm::mat4 ScreenToWorld();
  const glm::mat4 getScreenToWorld();
 

  void Init();
  void PreRender(int windowWidth, int windowHeight, float delta_time);
  void Render(float delta_time);
  void PostRender(float delta_time);

  EntityPtr addEntity(std::string name, EntityPtr parent = nullptr);
  EntityPtr findEntity(std::string name);
  void removeEntity(std::string name);

  glm::vec2 getWindowDimension(int windowWidth, int WindowHeight) const
  {
    return glm::vec2(windowWidth, WindowHeight);
  }

};

