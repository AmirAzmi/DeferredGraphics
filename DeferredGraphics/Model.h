#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <algorithm>
#include <map>

#include "Mesh.h"

enum class ModelType
{
  DEFAULT
};

class Model
{

public:
  Model(std::string filepath);
  Model(std::string filepath, ModelType type);

  std::vector<Mesh> meshes;
};

static glm::vec3 getValuesInBetweenWhiteSpacesVec3(const std::string line);
static glm::vec2 getValuesInBetweenWhiteSpacesVec2(const std::string line);
static std::vector<int> getUVFaceData(const std::string line);
static std::vector<int> getIndicesFaceData(const std::string line);
static std::vector<int> getNormalIndicesFaceData(const std::string line);

typedef std::shared_ptr<Model> ModelHandle;


