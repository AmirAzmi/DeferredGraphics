#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include "TemplateUtils.h"
#include "EngineTypes.h"


class Scene
{
  std::vector<EntityPtr> ListOfEntities; //list of entities

  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
  glm::vec3 eyePosition;
  glm::vec3 cameraDirection;
  glm::vec3 upDirection;
  float nearDistance;
  float farDistance;
  float fov;



public:
  
  ComponentLists ListOfTypes; //list of component types

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
  void PreRender();
  void Render();
  void PostRender();

  EntityPtr addEntity(std::string name);
  EntityPtr findEntity(std::string name);
  void removeEntity(std::string name);
};

