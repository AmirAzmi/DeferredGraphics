#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <vector>
#include <memory>
class Mesh
{
  GLuint posVBO;
  GLuint normsVBO;
  GLuint indexVBO;
  GLuint VAO;

  std::vector<glm::vec3> vertices = {};
  std::vector<glm::vec3> normals = {};
  std::vector<GLuint> indices = {};

  Mesh();
public:
  GLuint getPosVBO();
  GLuint getNormsVBO();
  GLuint getIndexVBO();
  GLuint getVAO();

  std::vector<GLuint> getIndexBuffer();

};

typedef std::shared_ptr<Mesh> MeshHandle;

