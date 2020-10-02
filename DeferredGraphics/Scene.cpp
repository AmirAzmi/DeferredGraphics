/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: scene.cpp
Purpose: Scene containing what needs to be updated/drawn
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#include "Scene.h"
#include "Entity.h"
#include <stdlib.h>
#include <cstdlib>
#include <string>

#include "RotationBehavior.h"
#include "CameraPossessBehavior.h"
#include "MovementBehavior.h"

Scene::Scene(const int windowWidth, const int windowHeight) :fov(90.0f), nearDistance(0.1f), farDistance(100.0f), cameraSpeed(3.0f),
eyePosition(glm::vec3(0.0f, 0.0f, 5.0f)), cameraDirection(glm::vec3(0.0f, 0.0f, -1.0f)), upDirection(glm::vec3(0.0f, 1.0f, 0.0f))
{
  windowDimension = getWindowDimension(windowWidth, windowHeight);
  projectionMatrix = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, nearDistance, farDistance);
  screenToWorld = ScreenToWorld();

  newProjectionMatrix = glm::perspective(glm::radians(fov), (float)1919.0f / (float)1000.0f, nearDistance, farDistance);
  viewMatrix = glm::lookAt(eyePosition, cameraDirection, upDirection);
}

std::vector<MeshComponentPtr>& Scene::getMeshes()
{
  //returns the vector of MeshComponentPtrs in the scene
  return ListOfTypes.get<MeshComponentPtr>();
}

std::vector<LightComponentPtr>& Scene::getLights()
{
  //returns the vector of LightComponentPtrs in the scene
  return ListOfTypes.get<LightComponentPtr>();
}

const glm::mat4 Scene::ScreenToWorld()
{
  glm::mat4 screen_to_world = viewMatrix * projectionMatrix;
  return glm::inverse(screen_to_world);
}

const glm::mat4 Scene::getScreenToWorld()
{
  return screenToWorld;
}

void Scene::Init()
{
  /*********************************************************************************/
  //setup the preliminaries to the mesh component
  // ->add a mesh to the component
  // ->add a shader to the component
  // ->add a material to the component
  // ->have a refrence to the entity
  /*********************************************************************************/

  //add a mesh to the component
  ModelHandle cube = std::make_shared<Model>("Resources/sphere.obj", ModelType::DEFAULT);
  ModelHandle bunny = std::make_shared<Model>("Resources/gh_sample_animation.fbx");
  ModelHandle sphere = std::make_shared<Model>("Resources/sphere.obj", ModelType::DEFAULT);
  ModelHandle pitch = std::make_shared<Model>("Resources/pitch.obj", ModelType::DEFAULT);

  //declare the Resources/Shaders that will be used for this scene
  ShaderHandle gBuffer = std::make_shared<Shader>("Resources/Shaders/gBuffer.vert", "Resources/Shaders/gBuffer.frag", true);
  ShaderHandle forwardRenderer = std::make_shared<Shader>("Resources/Shaders/forwardLightingPass.vert", "Resources/Shaders/forwardLightingPass.frag", false);
  ShaderHandle textureShader = std::make_shared<Shader>("Resources/Shaders/texture.vert", "Resources/Shaders/texture.frag", false);

  //add a material(s) to the component
  MaterialHandle material = std::make_shared<Material>(gBuffer);
  MaterialHandle material2 = std::make_shared<Material>(forwardRenderer);
  MaterialHandle material3 = std::make_shared<Material>(textureShader);

  for (auto& m : bunny->meshes)
  {
    m.material = material;
    m.shader = gBuffer;

    //Any material information needed
    m.material->setVec4("diffuse_color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    m.material->setFloat("specular_intensity", 1.0f);
  }

  material2->setVec4("diffuse_color", glm::vec4(0.5f, 0.0f, 0.7f, 1.0f));

  //create the Entity ptr
  EntityPtr centerObject = addEntity("Center Object");

  glm::vec3 offset(0, -1.5f, 4);
  //centerObject->addBehavior<CameraPossessBehavior>(offset);
  //centerObject->addBehavior<MovementBehavior>();

  //add a mesh component pointer to the object with the setup from the prelims
  MeshComponentPtr meshComp = centerObject->add<MeshComponent>(centerObject, bunny);
  LightComponentPtr lightComp = centerObject->add<LightComponent>();

  //manipulate the properties of the object by getting it from the component
  meshComp->getEntityPtr()->scale = glm::vec3(1.0f, 1.0f, 1.0f);
  meshComp->getEntityPtr()->angle = 0;
  meshComp->getEntityPtr()->axisOfRotation = glm::vec3(0.0f, 1.0f, 0.0f);

  //create 8 objects
  for (int i = 0; i < 8; ++i)
  {
    //set the name of each object that is added
    std::string numberAsString = std::to_string(i + 1);
    std::string str = "Object ";
    str.append(numberAsString);

    //add objects to the list
    EntityPtr object = addEntity(str);

    //add a mesh component pointer to the object with the setup from the prelims
    MeshComponentPtr meshComp2 = object->add<MeshComponent>(object, cube);
    //lightComp = object->add<LightComponent>();

    for (auto& m : meshComp2->mesh->meshes)
    {
      m.material = material3;
      m.shader = textureShader;
    }

    //manipulate the properties of the object by getting it from the component
    meshComp2->getEntityPtr()->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    meshComp2->getEntityPtr()->angle = 0;
    meshComp2->getEntityPtr()->axisOfRotation = glm::vec3(0.0f, 1.0f, 0.0f);
    meshComp2->getEntityPtr()->currentPosition = i * 45.0f * 3.1415f / 180.0f;

    //add behiavor to all 8 objects
    object->addBehavior<RotationBehavior>();
  }
}

void Scene::PreRender(int windowWidth, int windowHeight, float delta_time)
{
  //setup the initial view and projection matrices
  projectionMatrix = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, nearDistance, farDistance);

  glViewport(0, 0, windowWidth, windowHeight);

  for (EntityPtr entity : getEntities())
  {
    //updates the behaviors for all entities
    entity->onFrameBegin(delta_time * timeScale);
  }
}

//update the scene objects
void Scene::Render(float delta_time)
{
  for (int i = 0; i < getEntities().size(); ++i)
  {
    //updates the entity behaviors
    getEntities()[i]->update(delta_time * timeScale);
  }
}

void Scene::PostRender(float delta_time)
{
  for (EntityPtr entity : getEntities())
  {
    entity->onFrameEnd(delta_time * timeScale);
  }
}

EntityPtr Scene::addEntity(std::string name)
{
  //add the entity with the given name
  ListOfEntities.push_back(new Entity(name, *this));

  //return the entity since we know its at the back
  return ListOfEntities.back();

}

EntityPtr Scene::findEntity(std::string name)
{

  //loop through the entity list
  for (int i = 0; i < ListOfEntities.size(); ++i)
  {
    //look for the entity specified
    if (ListOfEntities[i]->name == name)
    {
      //return the potr to that entity
      return ListOfEntities[i];
    }
  }

  //could not find entity in the list of entities
  return nullptr;

}

void Scene::removeEntity(std::string name)
{
  //loop through the list
  for (int i = 0; i < ListOfEntities.size(); ++i)
  {
    //look for the first specified entity
    if (ListOfEntities[i]->name == name)
    {
      //ask shareef
      if (ListOfEntities[i]->get<MeshComponent>() != nullptr)
      {
        ListOfEntities[i]->remove<MeshComponent>();
      }

      if (ListOfEntities[i]->get<LightComponent>() != nullptr)
      {
        ListOfEntities[i]->remove<LightComponent>();
      }

      //delete the entity at that location
      delete ListOfEntities[i];

      //remove the entity at that point
      ListOfEntities.erase(ListOfEntities.begin() + i);

      //break out of the loop because you removed the entity
      break;
    }
  }
}

const glm::mat4 Scene::getProjectionMatrix() const
{
  return projectionMatrix;
}

const glm::mat4 Scene::getViewMatrix() const
{
  return viewMatrix;
}

const glm::vec3 Scene::getEyePosition() const
{
  return eyePosition;
}

const glm::vec3 Scene::getCameraDirection() const
{
  return cameraDirection;
}

const glm::vec3 Scene::getUpDirection() const
{
  return upDirection;
}

const float Scene::getNearDistance() const
{
  return nearDistance;
}

const float Scene::getFarDistance()const
{
  return farDistance;
}

const float Scene::getFOV() const
{
  return fov;
}

std::vector<EntityPtr>& Scene::getEntities()
{
  return ListOfEntities;
}
