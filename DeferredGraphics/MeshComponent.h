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
#include "Mesh.h"
#include "Material.h"
#include "EngineTypes.h"

class MeshComponent
{
  MeshHandle mesh;        //8 bytes
  MaterialHandle material;//8 bytes
  ShaderHandle shader;    //8 bytes
  EntityPtr entity;       //8 bytes
  //-------------------------------
  //total: 32 bytes


  public:
  MeshComponent(EntityPtr entity, MeshHandle meshHandle, ShaderHandle shaderHandle, MaterialHandle materialHandle):entity(entity),mesh(meshHandle), shader(shaderHandle), material(materialHandle)
  {
  }

  std::vector<glm::vec4> vertices;
  GLuint DrawMode = GL_TRIANGLES;

  EntityPtr getEntityPtr()
  {
    return entity;
  }

  MeshHandle getMesh()
  {
    return mesh;
  }

  MaterialHandle getMaterial()
  {
    return material;
  }

  ShaderHandle getShader()
  {
    return shader;
  }

  GLuint getDrawMode()
  {
    return DrawMode;
  }

  std::vector<glm::vec4> getVec4Vertices()
  {
    //convert vertices to vec4
    for (int i = 0; i < this->getMesh()->getVertices().size(); ++i)
    {
      vertices.push_back(glm::vec4(getMesh()->getVertices()[i].x, getMesh()->getVertices()[i].y, getMesh()->getVertices()[i].z, 1.0f));
    }
    return vertices;
  }
};

using MeshComponentPtr = MeshComponent*;

