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
private:
  struct BoneInfo
  {
    glm::mat4 BoneOffset;
    glm::mat4 FinalTransformation;

    BoneInfo() :BoneOffset{ 1 }, FinalTransformation{1}
    {
    }
  };

public:
  Model(std::string filepath);
  Model(std::string filepath, ModelType type);
  ~Model();

  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);

  void BoneTransform(float TimeInSeconds, int currentAnimation);
  void ReadNodeHeirarchy(float animationTime, aiNode * node, glm::mat4 parent, int currentAnimation);

  const aiScene* m_pScene;
  Assimp::Importer m_Importer;

  std::vector<Mesh> meshes;
  glm::mat4 tranformInv;
  std::map<std::string, int> m_BoneMapping;
  std::vector<BoneInfo> m_BoneInfo;
  int m_NumBones = 0;
};

static int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
static int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
static int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
static void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
static void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
static void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
static glm::vec3 getValuesInBetweenWhiteSpacesVec3(const std::string line);
static glm::vec2 getValuesInBetweenWhiteSpacesVec2(const std::string line);
static std::vector<int> getUVFaceData(const std::string line);
static std::vector<int> getIndicesFaceData(const std::string line);
static std::vector<int> getNormalIndicesFaceData(const std::string line);

typedef std::shared_ptr<Model> ModelHandle;


