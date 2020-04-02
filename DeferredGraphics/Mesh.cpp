/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: mesh.cpp
Purpose: model parser implementation
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#include "Mesh.h"
#include <iostream>
#include <regex>
#include <map>

Mesh::Mesh(std::string filePath)
{
  std::ifstream file(filePath);//read in the file
  std::string text_file_string;//stores all the text into string
  std::string line;            //variable for reading in a line

  glm::vec3 max(std::numeric_limits<float>::min());//min point
  glm::vec3 min(std::numeric_limits<float>::max());//max point
  glm::vec3 averagePosition = glm::vec3(0.0f, 0.0f, 0.0f);

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
      //if the first character is an 'f' that means its an index ordering
      case 'f':
      {
        if (line.find('/') == true)
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
        }
        else
        {
          glm::vec3 faceIndices;

          faceIndices = getValuesInBetweenWhiteSpacesVec3(line);

          indices.push_back(faceIndices[0] - 1);
          indices.push_back(faceIndices[1] - 1);
          indices.push_back(faceIndices[2] - 1);
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
    std::cout << " failed to load" << filePath << "\n";
    exit(1);
  }

  std::vector<glm::vec2> uploadedUVs;
  std::vector<glm::vec3> faceNormals;

  averagePosition /= vertices.size(); //center object based off of position

  //grab the bounds
  float rangex = max.x - min.x;
  float rangey = max.y - min.y;
  float rangez = max.z - min.z;

  //get the range max
  float rangemax(std::max(std::max(rangex, rangey), rangez));

  //for all the vertices set the offset so it is centered and divide by the range max
  for (auto& vert : vertices)
  {
    vert -= averagePosition;
    vert /= rangemax;
  }

  //normals are now formed into vertex normals from face normals
  std::map<int, int> count;
  normals.resize(vertices.size());
  for (int i = 0; i < indices.size() - 2; i += 3)
  {
    //calculate the face normals from the cross product of a triangle
    glm::vec3 v0 = vertices[indices[i]];
    glm::vec3 v1 = vertices[indices[i + 1]];
    glm::vec3 v2 = vertices[indices[i + 2]];
    glm::vec3 R_P = v2 - v0;
    glm::vec3 Q_P = v1 - v0;
    faceNormals.push_back(glm::normalize(glm::cross(Q_P, R_P)));

    //set up the vertex normals (dont know how this works yet)
    count[indices[i]] += 1;
    normals[indices[i]] += faceNormals.back();
    count[indices[i]+1] += 1;
    normals[indices[i+1]] += faceNormals.back();
    count[indices[i+2]] += 1;
    normals[indices[i+2]] += faceNormals.back();

    //upload UV data
    uploadedUVs.push_back(glm::vec2(0.0f, 0.0f));
    uploadedUVs.push_back(glm::vec2(0.0f, 1.0f));
    uploadedUVs.push_back(glm::vec2(1.0f, 1.0f));
  }

  //https://stackoverflow.com/questions/4703432/why-does-my-opengl-phong-shader-behave-like-a-flat-shader
  //renormalize the normals becuase that is what some website said to do for calculating vertex normals
  for (int i = 0; i < indices.size(); ++i)
  { 
    normals[indices[i]] = glm::normalize(normals[indices[i]] / (float)count[indices[i]]);
  }
  
  //generate the Vertex Array Object
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  //genereate the buffer that stores position data from the vector of vertices
  glGenBuffers(1, &posVBO);
  glBindBuffer(GL_ARRAY_BUFFER, posVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
    vertices.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores normal data from the vector of normal
  glGenBuffers(1, &normsVBO);
  glBindBuffer(GL_ARRAY_BUFFER, normsVBO);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
    normals.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores uv data from the vector of uv
  glGenBuffers(1, &uvVBO);
  glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
  glBufferData(GL_ARRAY_BUFFER, uploadedUVs.size() * sizeof(glm::vec2),
    uploadedUVs.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores index data from the vector of indices
  glGenBuffers(1, &indexVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uint), indices.data(), GL_STATIC_DRAW);
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

std::vector<GLuint> Mesh::getIndices()
{
  return indices;
}

std::vector<glm::vec3> Mesh::getVertices()
{
  return vertices;
}

std::vector<glm::vec3> Mesh::getNormals()
{
  return normals;
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
