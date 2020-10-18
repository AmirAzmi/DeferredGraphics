#include "Model.h"

glm::mat4 convertToGLM(aiMatrix4x4 mat)
{
  glm::mat4 matrix;

  matrix[0][0] = mat.a1;
  matrix[0][1] = mat.a2;
  matrix[0][2] = mat.a3;
  matrix[0][3] = mat.a4;

  matrix[1][0] = mat.b1;
  matrix[1][1] = mat.b2;
  matrix[1][2] = mat.b3;
  matrix[1][3] = mat.b4;

  matrix[2][0] = mat.c1;
  matrix[2][1] = mat.c2;
  matrix[2][2] = mat.c3;
  matrix[2][3] = mat.c4;

  matrix[3][0] = mat.d1;
  matrix[3][1] = mat.d2;
  matrix[3][2] = mat.d3;
  matrix[3][3] = mat.d4;

  return glm::transpose(matrix);
}

static glm::mat4 convertToGLM(aiMatrix3x3 mat)
{
  glm::mat4 matrix;

  matrix[0][0] = mat.a1;
  matrix[0][1] = mat.a2;
  matrix[0][2] = mat.a3;
  matrix[0][3] = 0;

  matrix[1][0] = mat.b1;
  matrix[1][1] = mat.b2;
  matrix[1][2] = mat.b3;
  matrix[1][3] = 0;

  matrix[2][0] = mat.c1;
  matrix[2][1] = mat.c2;
  matrix[2][2] = mat.c3;
  matrix[2][3] = 0;

  matrix[3][0] = 0;
  matrix[3][1] = 0;
  matrix[3][2] = 0;
  matrix[3][3] = 1;

  return glm::transpose(matrix);
}

Model::Model(std::string filepath)
{
  m_pScene = m_Importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | 
    aiProcess_LimitBoneWeights);
  tranformInv = convertToGLM(m_pScene->mRootNode->mTransformation.Inverse());

  if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
  {
    std::cout << "ERROR::ASSIMP::" << m_Importer.GetErrorString() << std::endl;
    return;
  }

  //directory = filepath.substr(0, filepath.find_last_of('/'));

  processNode(m_pScene->mRootNode, m_pScene);
}


void Model::BoneTransform(float TimeInSeconds, int currentAnimation)
{
  glm::mat4 Identity(1);

  float TicksPerSecond = m_pScene->mAnimations[currentAnimation]->mTicksPerSecond != 0 ?
    m_pScene->mAnimations[currentAnimation]->mTicksPerSecond : 60.0f;
  float TimeInTicks = TimeInSeconds * TicksPerSecond;
  float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[currentAnimation]->mDuration);

  ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity, currentAnimation);
}

const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
  for (int i = 0; i < pAnimation->mNumChannels; i++)
  {
    const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

    if (std::string(pNodeAnim->mNodeName.data) == NodeName)
    {
      return pNodeAnim;
    }
  }

  return NULL;
}

void Model::ReadNodeHeirarchy(float AnimationTime, aiNode* pNode, glm::mat4 parent, int currentAnimation)
{
  std::string NodeName(pNode->mName.data);

  const aiAnimation* pAnimation = m_pScene->mAnimations[currentAnimation];

  glm::mat4 NodeTransformation(convertToGLM(pNode->mTransformation));

  const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

  if (pNodeAnim)
  {
    // Interpolate scaling and generate scaling transformation matrix
    aiVector3D Scaling;
    CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);

    glm::mat4 ScalingM
    (
      { Scaling.x, 0.0f, 0.0f, 0.0f },
      { 0.0f, Scaling.y, 0.0f, 0.0f },
      { 0.0f, 0.0f, Scaling.z, 0.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f }
    );

    // Interpolate rotation and generate rotation transformation matrix
    aiQuaternion RotationQ;
    CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
    glm::mat4 RotationM = convertToGLM(RotationQ.GetMatrix());

    // Interpolate translation and generate translation transformation matrix
    aiVector3D Translation;
    CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);

    glm::mat4 TranslationM
    (
      { 1.0f, 0.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f, 0.0f },
      { Translation.x, Translation.y, Translation.z, 1.0f }
    );

    // Combine the above transformations
    NodeTransformation = TranslationM * RotationM * ScalingM;
  }

  glm::mat4 GlobalTransformation = parent * NodeTransformation;

  if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
  {
    int BoneIndex = m_BoneMapping[NodeName];
    //upload onto GPU
    m_BoneInfo[BoneIndex].FinalTransformation = tranformInv * GlobalTransformation *
      m_BoneInfo[BoneIndex].BoneOffset;
  }

  for (int i = 0; i < pNode->mNumChildren; i++)
  {
    ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, currentAnimation);
  }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
  Mesh defaultMesh;
  defaultMesh.vertices.reserve(mesh->mNumVertices);
  defaultMesh.normals.reserve(mesh->mNumVertices);
  defaultMesh.uv.reserve(mesh->mNumUVComponents[0]);

  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    glm::vec3 vertex(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    defaultMesh.vertices.push_back(vertex);
  }

  if (mesh->mNormals != nullptr)
  {
    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
      glm::vec3 normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
      defaultMesh.normals.push_back(normal);
    }
  }
  else
  {
    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
      glm::vec3 normal(1.0f, 0.0f, 0.0f);
      defaultMesh.normals.push_back(normal);
    }
  }

  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    if (mesh->GetNumUVChannels() > 0 && mesh->mTextureCoords[0] != nullptr)
    {
      glm::vec2 uv(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
      defaultMesh.uv.push_back(uv);
    }
    else
    {
      defaultMesh.uv.push_back(glm::vec2(0, 0));
    }
  }

  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    unsigned int j = mesh->GetNumColorChannels();

    if (j > 0)
    {
      glm::vec4 colors(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
      defaultMesh.colors.push_back(colors);
    }
    else
    {
      defaultMesh.colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
  }

  for (int i = 0; i < mesh->mNumFaces; ++i)
  {
    for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
    {
      defaultMesh.indices.push_back(mesh->mFaces[i].mIndices[j]);

      assert(mesh->mFaces[i].mIndices[j] < mesh->mNumVertices);
    }
  }

  defaultMesh.bones.resize(mesh->mNumVertices);

  for (int i = 0; i < mesh->mNumBones; i++)
  {
    int BoneIndex;
    std::string BoneName(mesh->mBones[i]->mName.data);

    if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
    {
      BoneIndex = m_NumBones;
      m_NumBones++;
      BoneInfo bi;
      m_BoneInfo.push_back(bi);
    }
    else
    {
      BoneIndex = m_BoneMapping[BoneName];
    }

    m_BoneMapping[BoneName] = BoneIndex;

    mesh->mBones[i]->mOffsetMatrix.Transpose();

    m_BoneInfo[BoneIndex].BoneOffset[0][0] = mesh->mBones[i]->mOffsetMatrix.a1;
    m_BoneInfo[BoneIndex].BoneOffset[0][1] = mesh->mBones[i]->mOffsetMatrix.a2;
    m_BoneInfo[BoneIndex].BoneOffset[0][2] = mesh->mBones[i]->mOffsetMatrix.a3;
    m_BoneInfo[BoneIndex].BoneOffset[0][3] = mesh->mBones[i]->mOffsetMatrix.a4;

    m_BoneInfo[BoneIndex].BoneOffset[1][0] = mesh->mBones[i]->mOffsetMatrix.b1;
    m_BoneInfo[BoneIndex].BoneOffset[1][1] = mesh->mBones[i]->mOffsetMatrix.b2;
    m_BoneInfo[BoneIndex].BoneOffset[1][2] = mesh->mBones[i]->mOffsetMatrix.b3;
    m_BoneInfo[BoneIndex].BoneOffset[1][3] = mesh->mBones[i]->mOffsetMatrix.b4;

    m_BoneInfo[BoneIndex].BoneOffset[2][0] = mesh->mBones[i]->mOffsetMatrix.c1;
    m_BoneInfo[BoneIndex].BoneOffset[2][1] = mesh->mBones[i]->mOffsetMatrix.c2;
    m_BoneInfo[BoneIndex].BoneOffset[2][2] = mesh->mBones[i]->mOffsetMatrix.c3;
    m_BoneInfo[BoneIndex].BoneOffset[2][3] = mesh->mBones[i]->mOffsetMatrix.c4;

    m_BoneInfo[BoneIndex].BoneOffset[3][0] = mesh->mBones[i]->mOffsetMatrix.d1;
    m_BoneInfo[BoneIndex].BoneOffset[3][1] = mesh->mBones[i]->mOffsetMatrix.d2;
    m_BoneInfo[BoneIndex].BoneOffset[3][2] = mesh->mBones[i]->mOffsetMatrix.d3;
    m_BoneInfo[BoneIndex].BoneOffset[3][3] = mesh->mBones[i]->mOffsetMatrix.d4;

    for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
    {
      int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
      float Weight = mesh->mBones[i]->mWeights[j].mWeight;
      defaultMesh.bones[VertexID].AddBoneData(BoneIndex, Weight);
    }
  }

  defaultMesh.name = mesh->mName.C_Str();
  defaultMesh.setupMesh();

  return defaultMesh;
}

Model::Model(std::string filepath, ModelType type)
{
  m_pScene = nullptr;
  Mesh defaultMesh;

  std::ifstream file(filepath);//read in the file
  std::string line;            //variable for reading in a line

  glm::vec3 max(std::numeric_limits<float>::min());//min point
  glm::vec3 min(std::numeric_limits<float>::max());//max point
  glm::vec3 averagePosition = glm::vec3(0.0f, 0.0f, 0.0f);

  std::vector<int> vt_locations;
  std::vector<int> ind_locations;
  std::vector<int> norm_locations;

  size_t last_char = filepath.find_last_of('/');

  if (last_char == std::string::npos)
  {
    defaultMesh.name = filepath;
  }
  else
  {
    defaultMesh.name = filepath.substr(last_char + 1);
  }

  //if the file is open
  if (file.is_open())
  {
    while (std::getline(file, line))//read the first line
    {
      //if th first character is a v
      switch (line[0])
      {
        //this means there is a comment on this line
        case '#':
        {
          break;
        }

        //this means the first character is 'v' which means it could either ve a vertex, normal, or uv coordinates
        case 'v':
        {
          //grab the second character of the line
          switch (line[1])
          {
            //if it is a space then we know its a vertex
            case ' ':
            {
              //create the temporary vertex
              glm::vec3 vertex;

              //gets the values in between the spaces and stores it into a vec3
              vertex = getValuesInBetweenWhiteSpacesVec3(line);


              averagePosition += vertex;

              if (max.x < vertex.x) { max.x = vertex.x; }
              if (max.y < vertex.y) { max.y = vertex.y; }
              if (max.z < vertex.z) { max.z = vertex.z; }

              if (min.x > vertex.x) { min.x = vertex.x; }
              if (min.y > vertex.y) { min.y = vertex.y; }
              if (min.z > vertex.z) { min.z = vertex.z; }

              //stores vertex into vertices buffer
              defaultMesh.vertices.push_back(vertex);

              //break out
              break;
            }

            //if it is a 'n' then we know its a vertex normal
            case 'n':
            {
              //create the temporary vertex
              glm::vec3 vertexNormal;

              //gets the values in between the spaces and stores it into a vec3
              vertexNormal = getValuesInBetweenWhiteSpacesVec3(line);

              //stores vertex into normals buffer
              defaultMesh.normals.push_back(vertexNormal);

              //break out
              break;
            }

            //if it is a 't' then we know its a UV value
            case 't':
            {
              //create the temporary vertex
              glm::vec2 UVs;

              //gets the values in between the spaces and stores it into a vec3
              UVs = getValuesInBetweenWhiteSpacesVec2(line);

              //stores vertex into normals buffer
              defaultMesh.uv.push_back(UVs);

              //break out
              break;
            }

            //if it is none of those characters stated above
            default:
            {
              break;
            }
          }
          break;
        }
        //if the first character is an 'f' that means its an index ordering
        case 'f':
        {
          if (line.find('/') != std::string::npos)
          {
            /*
            std::vector<int> index_data = getIndicesFaceData(line);
            std::vector<int> uv_data = getUVFaceData(line);
            std::vector<int> norm_data = getNormalIndicesFaceData(line);

            for (int i = 0; i < index_data.size(); ++i)
            {
              vt_locations.push_back(index_data[i]);
            }

            for (int i = 0; i < uv_data.size(); ++i)
            {
              ind_locations.push_back(uv_data[i]);
            }

            for (int i = 0; i < norm_data.size(); ++i)
            {
              norm_locations.push_back(norm_data[i]);
            }*/

            glm::vec3 faceIndices;

            faceIndices = getValuesInBetweenWhiteSpacesVec3(line);

            defaultMesh.indices.push_back(faceIndices[0] - 1);
            defaultMesh.indices.push_back(faceIndices[1] - 1);
            defaultMesh.indices.push_back(faceIndices[2] - 1);
          }
          else
          {
            glm::vec3 faceIndices;

            faceIndices = getValuesInBetweenWhiteSpacesVec3(line);

            defaultMesh.indices.push_back(faceIndices[0] - 1);
            defaultMesh.indices.push_back(faceIndices[1] - 1);
            defaultMesh.indices.push_back(faceIndices[2] - 1);
          }

          //break out    
          break;
        }
        default:
        {
          break;
        }
      }
    }
  }
  else
  {
    std::cout << " failed to load" << filepath << "\n";
    exit(1);
  }


  /*std::vector<FaceTemp> temp_faces;
  temp_faces.resize(ind_locations.size());

  for (int i = 0; i < ind_locations.size(); ++i)
  {
    temp_faces[i].vt = vt_locations[i];
    temp_faces[i].uv = ind_locations[i];
    temp_faces[i].norm = norm_locations[i];
  }*/

  bool isEmpty = true;

  //if uv is not empty
  if (!defaultMesh.uv.empty())
  {
    isEmpty = false;
  }

  std::vector<glm::vec2> uploadedFromSlashesUVs;
  std::vector<glm::vec2> uploadedUVs;
  std::vector<glm::vec3> faceNormals;

  averagePosition /= defaultMesh.vertices.size(); //center object based off of position

  //grab the bounds
  float rangex = max.x - min.x;
  float rangey = max.y - min.y;
  float rangez = max.z - min.z;

  //get the range max
  float rangemax(std::max(std::max(rangex, rangey), rangez));

  //for all the vertices set the offset so it is centered and divide by the range max
  for (auto& vert : defaultMesh.vertices)
  {
    vert -= averagePosition;
    vert /= rangemax;
  }

  //normals are now formed into vertex normals from face normals
  std::map<int, int> count;
  defaultMesh.normals.resize(defaultMesh.vertices.size());
  for (int i = 0; i < defaultMesh.indices.size() - 2; i += 3)
  {
    //calculate the face normals from the cross product of a triangle
    glm::vec3 v0 = defaultMesh.vertices[defaultMesh.indices[i]];
    glm::vec3 v1 = defaultMesh.vertices[defaultMesh.indices[i + 1]];
    glm::vec3 v2 = defaultMesh.vertices[defaultMesh.indices[i + 2]];
    glm::vec3 R_P = v2 - v0;
    glm::vec3 Q_P = v1 - v0;
    faceNormals.push_back(glm::normalize(glm::cross(Q_P, R_P)));

    //set up the vertex normals (dont know how this works yet)
    count[defaultMesh.indices[i]] += 1;
    defaultMesh.normals[defaultMesh.indices[i]] += faceNormals.back();
    count[defaultMesh.indices[i] + 1] += 1;
    defaultMesh.normals[defaultMesh.indices[i + 1]] += faceNormals.back();
    count[defaultMesh.indices[i + 2]] += 1;
    defaultMesh.normals[defaultMesh.indices[i + 2]] += faceNormals.back();

    //upload UV data

    if (isEmpty == true)
    {
      uploadedUVs.push_back(glm::vec2(0.0f, 0.0f));
      uploadedUVs.push_back(glm::vec2(0.0f, 1.0f));
      uploadedUVs.push_back(glm::vec2(1.0f, 1.0f));
    }
  }

  if (isEmpty == true)
  {
    defaultMesh.uv = uploadedUVs;
  }

  //https://stackoverflow.com/questions/4703432/why-does-my-opengl-phong-shader-behave-like-a-flat-shader
  //renormalize the normals becuase that is what some website said to do for calculating vertex normals
  for (int i = 0; i < defaultMesh.indices.size(); ++i)
  {
    defaultMesh.normals[defaultMesh.indices[i]] = glm::normalize(defaultMesh.normals[defaultMesh.indices[i]] / (float)count[defaultMesh.indices[i]]);
  }

  defaultMesh.colors.resize(defaultMesh.vertices.size());
  for (int i = 0; i < defaultMesh.colors.size(); ++i)
  {
    defaultMesh.colors[i] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  defaultMesh.setupMesh();

  meshes.push_back(defaultMesh);
}

Model::~Model()
{
  //__debugbreak();
}

const std::vector<GLuint> Mesh::getIndices()
{
  return indices;
}

const std::vector<glm::vec3> Mesh::getVertices()
{
  return vertices;
}

const std::vector<glm::vec3> Mesh::getNormals()
{
  return normals;
}

int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  assert(pNodeAnim->mNumRotationKeys > 0);

  for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
  {
    if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
    {
      return i;
    }
  }

  assert(0);
}

void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  // we need at least two values to interpolate...
  if (pNodeAnim->mNumRotationKeys == 1)
  {
    Out = pNodeAnim->mRotationKeys[0].mValue;
    return;
  }

  int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
  int NextRotationIndex = (RotationIndex + 1);
  assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
  float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
  float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
  assert(Factor >= 0.0f && Factor <= 1.0f);
  const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
  const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
  aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
  Out = Out.Normalize();
}

int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
  {
    if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1 % pNodeAnim->mNumPositionKeys].mTime)
    {
      return i;
    }
  }

  assert(0);

  return 0;
}

void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{

  if (pNodeAnim->mNumPositionKeys == 1)
  {
    Out = pNodeAnim->mPositionKeys[0].mValue;
    return;
  }

  int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
  int NextPositionIndex = (PositionIndex + 1);
  assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
  float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
  float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
  assert(Factor >= 0.0f && Factor <= 1.0f);
  const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
  const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
  aiVector3D Delta = End - Start;
  Out = Start + Factor * Delta;
}

int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  assert(pNodeAnim->mNumScalingKeys > 0);

  for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
  {
    if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1 % pNodeAnim->mNumScalingKeys].mTime)
    {
      return i;
    }
  }

  assert(0);

  return 0;
}

void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  if (pNodeAnim->mNumScalingKeys == 1)
  {
    Out = pNodeAnim->mScalingKeys[0].mValue;
    return;
  }

  int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
  int NextScalingIndex = (ScalingIndex + 1);
  assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
  float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
  float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
  assert(Factor >= 0.0f && Factor <= 1.0f);
  const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
  const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
  aiVector3D Delta = End - Start;
  Out = Start + Factor * Delta;
}

glm::vec3 getValuesInBetweenWhiteSpacesVec3(const std::string line)
{
  size_t iterator = line.find(' '); //find the first space
  glm::vec3 positionVertex = glm::vec3(0, 0, 0); //initialize the returned position vertex
  std::string value; //temp value holding the float to be added into the position vertex
  int counter = 0; //counter for the positionVertex so when it adds an element, increment to the next place

  //loop thorught the characters in the line
  for (size_t i = iterator + 1; i < line.size(); ++i)
  {
    //if the line is not a space add it to a temp vector
    if (line[i] != ' ')
    {
      //add values to the the temp buffer
      value.push_back(line[i]);
    }
    else
    {
      if (value.empty())
      {
        continue;
      }

      //once you find a space store the value into the glm temp vector
      positionVertex[counter++] = std::stof(value);

      //clear the vector for the next variable to be transcribed
      value.clear();
    }
  }

  //if there is no space at the end of the obj file then it will do this
  if (counter != 3)
  {
    positionVertex[counter] = std::stof(value);
  }

  //return the vec3
  return positionVertex;
}

glm::vec2 getValuesInBetweenWhiteSpacesVec2(const std::string line)
{
  size_t iterator = line.find(' '); //find the first space
  glm::vec2 positionVertex = glm::vec2(0, 0); //initialize the returned position vertex
  std::string value; //temp value holding the float to be added into the position vertex
  int counter = 0; //counter for the positionVertex so when it adds an element, increment to the next place

  //loop thorught the characters in the line
  for (size_t i = iterator + 1; i < line.size(); ++i)
  {
    //if the line is not a space add it to a temp vector
    if (line[i] != ' ')
    {
      //add values to the the temp buffer
      value.push_back(line[i]);
    }
    else
    {
      //once you find a space store the value into the glm temp vector
      positionVertex[counter++] = std::stof(value);

      //clear the vector for the next variable to be transcribed
      value.clear();
    }
  }

  //if there is no space at the end of the obj file then it will do this
  if (counter != 2)
  {
    positionVertex[counter] = std::stof(value);
  }

  //return the vec3
  return positionVertex;
}

std::vector<int> getUVFaceData(const std::string line)
{
  size_t iterator = 0;
  std::vector<int> uvs;

  while (iterator < line.size() - 1)
  {
    size_t start = line.find('/', iterator);

    if (start == std::string::npos)
    {
      break;
    }

    size_t end = line.find('/', start + 1);
    std::string num = line.substr(start + 1, end - (start + 1));
    int val = atoi(num.c_str());
    uvs.push_back(val);
    iterator = end + 1;
  }

  return uvs;
}

std::vector<int> getIndicesFaceData(const std::string line)
{
  size_t iterator = 0;
  std::vector<int> indices;

  while (iterator < line.size() - 1)
  {
    size_t start = line.find(' ', iterator);

    if (start == std::string::npos)
    {
      break;
    }
    size_t end = line.find('/', start + 1);
    std::string num = line.substr(start + 1, end - (start + 1));
    int val = atoi(num.c_str());
    indices.push_back(val);
    iterator = end + 1;
  }

  return indices;
}

std::vector<int> getNormalIndicesFaceData(const std::string line)
{

  size_t iterator = 0;
  std::vector<int> normal_indices;

  while (iterator < line.size() - 1)
  {
    size_t first_slash = line.find('/', iterator);

    if (first_slash == std::string::npos)
    {
      break;
    }

    size_t start = line.find('/', first_slash + 1);

    if (start == std::string::npos)
    {
      break;
    }
    size_t end_of_line = line.size() - 1;

    size_t end = line.find(' ', start + 1);

    if (end > end_of_line)
    {
      std::string num = line.substr(start + 1, line.size() - (start + 1));
      int val = atoi(num.c_str());
      normal_indices.push_back(val);
      break;
    }
    else
    {
      end = line.find(' ', start + 1);
    }

    std::string num = line.substr(start + 1, end - (start + 1));
    int val = atoi(num.c_str());
    normal_indices.push_back(val);
    iterator = end + 1;
  }

  return normal_indices;

}

void Model::getSkeletonRec(aiNode* pNode, glm::mat4x4 parentTransform, glm::vec3 startPosition, int prevID, bool recThroughSkeleton)
{
  //bones are joints, we draw lines that connect them - we need previous
  //positionand this position
  std::string NodeName(pNode->mName.data);
  glm::mat4x4 nodeTransformation = convertToGLM(pNode->mTransformation);
  //pAnimation->mChannels[0]->mNodeName
  //mat4x4 res1;
  glm::mat4x4 res(1);
  glm::vec3 pos;
  pos[0] = startPosition[0];
  pos[1] = startPosition[1];
  pos[2] = startPosition[2];

  res = parentTransform * nodeTransformation;

  int ID = -1;
  std::string cName = NodeName.substr(NodeName.find_last_of("_") + 1);
  if (m_BoneMapping.find(NodeName) != m_BoneMapping.end() || cName == "end")
  {
    if (cName == "end")
    {
      ID = prevID;
    }
    else
    {
      ID = m_BoneMapping[NodeName];
    }

    recThroughSkeleton = true;
    pos[0] = res[3][0];
    pos[1] = res[3][1];
    pos[2] = res[3][2];


    skeletonBones.AddBoneData(prevID, ID, startPosition, pos);
  }

  if (recThroughSkeleton) 
  {
    for (GLuint i = 0; i < pNode->mNumChildren; i++) 
    {

      getSkeletonRec(pNode->mChildren[i], res, pos, ID, recThroughSkeleton);
    }
  }
  else
  {
    glm::mat4x4 I(1);

    for (GLuint i = 0; i < pNode->mNumChildren; i++)
    {
      getSkeletonRec(pNode->mChildren[i], I, pos, ID, recThroughSkeleton);
    }
  }
}

