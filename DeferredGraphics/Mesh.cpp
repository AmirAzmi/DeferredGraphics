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
#include <algorithm>
#include <map>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
  glDeleteBuffers(0, &posVBO);
  glDeleteBuffers(0, &normsVBO);
  glDeleteBuffers(0, &uvVBO);
  glDeleteBuffers(0, &colorVBO);
  glDeleteBuffers(0, &indexVBO);
  glDeleteBuffers(0, &VAO);
}

void Mesh::setupMesh()
{
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
  glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2),
    uv.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores normal data from the vector of normal
  glGenBuffers(1, &colorVBO);
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
    colors.data(), GL_STATIC_DRAW);

  //genereate the buffer that stores index data from the vector of indices
  glGenBuffers(1, &indexVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uint), indices.data(), GL_STATIC_DRAW);

  //enable position data that will be transferred to the GPU
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, getPosVBO());
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  //enable normals data that will be transferred to the GPU
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, getNormsVBO());
  glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);

  //enable UV data that will be transferred to the GPU
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, getUVBO());
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  //enable colors data that will be transferred to the GPU
  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, getColorVBO());
  glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, (void*)0);

  //bind the index buffer that will be transferred to the GPU
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getIndexVBO());
}

