/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Mesh/model loading class
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include "AABB.h"
#include "Material.h"


class Mesh
{
  //mesh data
  GLuint posVBO;
  GLuint normsVBO;
  GLuint indexVBO;
  GLuint colorVBO;
  GLuint uvVBO;
  GLuint VAO;

public:
  std::vector<glm::vec3> vertices = {};
  std::vector<glm::vec3> normals = {};
  std::vector<glm::vec4> colors = {};
  std::vector<glm::vec2> uv = {};

  std::vector<GLuint> indices = {};
  std::vector<GLuint> normal_indices = {};

  std::string name;

  MaterialHandle material;//8 bytes
  ShaderHandle shader;    //8 bytes

private:
  //model data 
  std::vector<Mesh> meshes;
  std::string directory;

public:

  Mesh();
  ~Mesh();

  void setupMesh();

  inline GLuint getPosVBO()
  {
    return posVBO;
  }

  inline GLuint getNormsVBO()
  {
    return normsVBO;
  }

  inline GLuint getIndexVBO()
  {
    return indexVBO;
  }

  inline GLuint getColorVBO()
  {
    return colorVBO;
  }

  inline GLuint getUVBO()
  {
    return uvVBO;
  }

  GLuint getVAO()
  {
    return VAO;
  }

  inline std::string getName()
  {
    return name;
  }

  const std::vector<GLuint> getIndices();
  const std::vector<glm::vec3> getVertices();
  const std::vector<glm::vec3> getNormals();

};

static glm::vec3 getValuesInBetweenWhiteSpacesVec3(const std::string line);
static glm::vec2 getValuesInBetweenWhiteSpacesVec2(const std::string line);
static std::vector<int> getUVFaceData(const std::string line);
static std::vector<int> getIndicesFaceData(const std::string line);
static std::vector<int> getNormalIndicesFaceData(const std::string line);

typedef std::shared_ptr<Mesh> MeshHandle;