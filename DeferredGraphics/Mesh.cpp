#include "Mesh.h"
#include <iostream>
#include <regex>

Mesh::Mesh(std::string filePath)
{
  std::ifstream file(filePath);//read in the file
  std::string text_file_string;//stores all the text into string
  std::string line;            //variable for reading in a line

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

          //stores vertex into vertices buffer
          vertices.push_back(vertex);

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
          normals.push_back(vertexNormal);

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
          uv.push_back(UVs);

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
      //if the first character is an 'f' that means its a index ordering
      case 'f':
      {

        //gets the values in between the spaces and stores it into a vec3
        Face indexLocation = getFaceData(line);

        //stores values into index buffer
        indices.push_back(indexLocation.vertex_position_indices[0] - 1);
        indices.push_back(indexLocation.vertex_position_indices[1] - 1);
        indices.push_back(indexLocation.vertex_position_indices[2] - 1);

        normal_indices.push_back(indexLocation.vertex_normal_indices[0] - 1);
        normal_indices.push_back(indexLocation.vertex_normal_indices[1] - 1);
        normal_indices.push_back(indexLocation.vertex_normal_indices[2] - 1);
        std::cout << "";

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

  std::vector<glm::vec3> uploadedVertices;
  std::vector<glm::vec2> uploadedUVs;
  std::vector<glm::vec3> uploadedNormals;
  std::vector<GLuint> uploadedIndices;

  for (int i = 0; i < indices.size(); i += 3)
  {
    glm::vec3 v0 = vertices[indices[i]];
    glm::vec3 v1 = vertices[indices[i + 1]];
    glm::vec3 v2 = vertices[indices[i + 2]];
    glm::vec3 R_P = v2 - v0;
    glm::vec3 Q_P = v1 - v0;
    glm::vec3 normal = glm::normalize(glm::cross(Q_P, R_P));

    uploadedVertices.push_back(v0);
    uploadedVertices.push_back(v1);
    uploadedVertices.push_back(v2);
    uploadedNormals.push_back(normal);
    uploadedNormals.push_back(normal);
    uploadedNormals.push_back(normal);
    uploadedIndices.push_back(i);
    uploadedIndices.push_back(i + 1);
    uploadedIndices.push_back(i + 2);
    uploadedUVs.push_back(glm::vec2(0.0f, 0.0f));
    uploadedUVs.push_back(glm::vec2(0.0f, 1.0f));
    uploadedUVs.push_back(glm::vec2(1.0f, 1.0f));
  }

  //generate the Vertex Array Object
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  //genereate the buffer that stores position data from the vector of vertices
  glGenBuffers(1, &posVBO);
  glBindBuffer(GL_ARRAY_BUFFER, posVBO);
  glBufferData(GL_ARRAY_BUFFER, uploadedVertices.size() * sizeof(glm::vec3),
    uploadedVertices.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores normal data from the vector of normal
  glGenBuffers(1, &normsVBO);
  glBindBuffer(GL_ARRAY_BUFFER, normsVBO);
  glBufferData(GL_ARRAY_BUFFER, uploadedNormals.size() * sizeof(glm::vec3),
    uploadedNormals.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores uv data from the vector of uv
  glGenBuffers(1, &uvVBO);
  glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
  glBufferData(GL_ARRAY_BUFFER, uploadedUVs.size() * sizeof(glm::vec2),
    uploadedUVs.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores index data from the vector of indices
  glGenBuffers(1, &indexVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, uploadedIndices.size() * sizeof(glm::uint),
    uploadedIndices.data(), GL_STATIC_DRAW);
}

GLuint Mesh::getPosVBO()
{
  return posVBO;
}

GLuint Mesh::getNormsVBO()
{
  return normsVBO;
}

GLuint Mesh::getIndexVBO()
{
  return indexVBO;
}

GLuint Mesh::getVAO()
{
  return VAO;
}

GLuint Mesh::getUVBO()
{
  return uvVBO;
}

std::vector<GLuint> Mesh::getIndexBuffer()
{
  return indices;
}

glm::vec3 Mesh::getValuesInBetweenWhiteSpacesVec3(std::string line)
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

glm::vec2 Mesh::getValuesInBetweenWhiteSpacesVec2(std::string line)
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

Mesh::Face Mesh::getFaceData(std::string line)
{
  Face face;
  face.vertex_normal_indices = glm::vec3();
  face.vertex_position_indices = glm::vec3();
  face.vertex_texture_coordinate_indices = glm::vec3();

  std::string vn;
  std::string uv;
  std::string pos;

  int pos_counter = 0;
  int uv_counter = 0;
  int vn_counter = 0;

  int slash_counter = 0;

  for (int i = 0; i <= line.size(); ++i)
  {
    //f  10/20/30  4/5/6  7/8/9
    if (i == line.size())
    {
      if (pos_counter < 3)
      {
        slash_counter++;
        face.vertex_position_indices[pos_counter++] = std::atof(pos.c_str());
        face.vertex_texture_coordinate_indices[uv_counter++] = std::atof(uv.c_str());
        face.vertex_normal_indices[vn_counter++] = std::atof(vn.c_str());

        pos.clear();
        vn.clear();
        uv.clear();
      }
    }
    if (line[i] == ' ' || line[i] == 'f' || i == line.size())
    {
      if (pos.empty() == true || line[i + 1] == ' ')
      {
        continue;
      }
      else
      {
        slash_counter++;
        face.vertex_position_indices[pos_counter++] = std::atof(pos.c_str());
        face.vertex_texture_coordinate_indices[uv_counter++] = std::atof(uv.c_str());
        face.vertex_normal_indices[vn_counter++] = std::atof(vn.c_str());

        pos.clear();
        uv.clear();
        vn.clear();
      }
    }
    else
    {
      if (line[i] == '/')
      {
        slash_counter++;
        continue;
      }

      if (slash_counter % 3 == 0)
      {
        pos.push_back(line[i]);
      }

      if (slash_counter % 3 == 1)
      {
        uv.push_back(line[i]);
      }

      if (slash_counter % 3 == 2)
      {
        vn.push_back(line[i]);
      }
    }

  }

  return face;
}
