#include "Mesh.h"

Mesh::Mesh()
{
  //load the vertices into the vertex vector
  vertices = {};
  //load the normals into the normals vector
  normals = {};
  //load the indices into the index vector
  indices = {};

  
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

  //genereate the buffer that stores index data from the vector of indices
  glGenBuffers(1, &indexVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uint),
    indices.data(), GL_STATIC_DRAW);
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

std::vector<GLuint> Mesh::getIndexBuffer()
{
  return indices;
}
