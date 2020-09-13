#include "Model.h"

Model::Model(std::string filepath)
{

  Assimp::Importer import;
  const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
    return;
  }
  //directory = filepath.substr(0, filepath.find_last_of('/'));

  //processNode(scene->mRootNode, scene);
}

Model::Model(std::string filepath, ModelType type)
{

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
    defaultMesh.colors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
  }

  defaultMesh.setupMesh();

  meshes.push_back(defaultMesh);
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
