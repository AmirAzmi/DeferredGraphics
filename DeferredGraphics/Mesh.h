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

class Mesh
{
  GLuint posVBO;
  GLuint normsVBO;
  GLuint indexVBO;
  GLuint uvVBO;
  GLuint VAO;

  std::vector<glm::vec3> vertices = {};
  std::vector<glm::vec3> normals = {};
  std::vector<glm::vec2> uv = {};
  std::vector<GLuint> indices = {};
  std::vector<GLuint> normal_indices = {};

public:
  struct Face
  {
    glm::vec3 vertex_position_indices;
    glm::vec3 vertex_normal_indices;
    glm::vec3 vertex_texture_coordinate_indices;
  };

private:
  Mesh();

  glm::vec3 getValuesInBetweenWhiteSpacesVec3(std::string line);
  glm::vec2 getValuesInBetweenWhiteSpacesVec2(std::string line);
  Face getFaceData(std::string line);


public:

  Mesh(std::string filePath);
  GLuint getPosVBO();
  GLuint getNormsVBO();
  GLuint getIndexVBO();
  GLuint getVAO();
  GLuint getUVBO();

  std::vector<GLuint> getIndices();
  std::vector<glm::vec3> getVertices();
  std::vector<glm::vec3> getNormals();

};

typedef std::shared_ptr<Mesh> MeshHandle;

