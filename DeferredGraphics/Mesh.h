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


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
enum ModelType
{
  DEFAULT,
  ASSIMP
};

struct Texture
{
  unsigned int id;
  std::string type;
};

class Mesh
{
  //mesh data
  GLuint posVBO;
  GLuint normsVBO;
  GLuint indexVBO;
  GLuint colorVBO;
  GLuint uvVBO;
  GLuint VAO;
  std::string name;

public:
  std::vector<glm::vec3> vertices = {};
  std::vector<glm::vec3> normals = {};
  std::vector<glm::vec3> colors = {};
  std::vector<glm::vec2> uv = {};

  std::vector<GLuint> indices = {};
  std::vector<GLuint> normal_indices = {};

private:
  //model data 
  std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  std::vector<Mesh> meshes;
  std::string directory;

public:

  struct FaceTemp
  {
    int vt;
    int uv;
    int norm;
  };

  struct Face
  {
    glm::vec3 vertex_position_indices;
    glm::vec3 vertex_normal_indices;
    glm::vec3 vertex_texture_coordinate_indices;
  };

private:
  Mesh();

  static glm::vec3 getValuesInBetweenWhiteSpacesVec3(const std::string line);
  static glm::vec2 getValuesInBetweenWhiteSpacesVec2(const std::string line);
  static std::vector<int> getUVFaceData(const std::string line);
  static std::vector<int> getIndicesFaceData(const std::string line);
  static std::vector<int> getNormalIndicesFaceData(const std::string line);

public:

  Mesh(const std::string filePath);
  Mesh(const std::string filePath, ModelType type);
  Mesh(float radius, int latitudeSlices, int longitudeSlices);

  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);

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

typedef std::shared_ptr<Mesh> MeshHandle;