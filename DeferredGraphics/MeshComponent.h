/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MeshCompnent.h
Purpose: Mesh Component of ECS system
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#pragma once
#include <glm/glm/glm.hpp>
#include "Model.h"
#include "Material.h"
#include "EngineTypes.h"

class MeshComponent
{

public:

  EntityPtr entity;       //8 bytes
  AABB bounds;            //24 bytes (2 * vec3)
  ModelHandle mesh;        //8 bytes
  //MaterialHandle material;//8 bytes
  //ShaderHandle shader;    //8 bytes
  std::string name;       //32 bytes
  float m_AnimationTime = 0.0f;
  int currentAnimation = 0;
  //-------------------------------
  //total: ?? bytes

  MeshComponent()
  {
    mesh = std::make_shared<Model>("Resources/sphere.obj");
  }

  MeshComponent(EntityPtr entity, ModelHandle meshHandle) :entity(entity), mesh(meshHandle)
  {
    bounds.Empty();
  }

  //find a way to not do this pls
  std::vector<glm::vec4> vertices;

  EntityPtr getEntityPtr()
  {
    return entity;
  }

  ModelHandle getMesh()
  {
    return mesh;
  }

  AABB getMeshBounds()
  {
    return bounds;
  }
};

using MeshComponentPtr = MeshComponent*;

