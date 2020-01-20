#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
class Mesh
{
  GLuint posVBO;
  GLuint normsVBO;
  GLuint indexVBO;
  GLuint uvVBO;
  GLuint VAO;

  std::vector<glm::vec3> vertices = {};
  std::vector<glm::vec3> normals = {};
  std::vector<glm::vec2> uv = {};
  std::vector<GLuint> indices = {};
  std::vector<GLuint> normal_indices = {};

public:
  struct Face
  {
    glm::vec3 vertex_position_indices;
    glm::vec3 vertex_normal_indices;
    glm::vec3 vertex_texture_coordinate_indices;
  };

private:
  Mesh();

  glm::vec3 getValuesInBetweenWhiteSpacesVec3(std::string line);
  glm::vec2 getValuesInBetweenWhiteSpacesVec2(std::string line);
  Face getFaceData(std::string line);


public:



  Mesh(std::string filePath);
  GLuint getPosVBO();
  GLuint getNormsVBO();
  GLuint getIndexVBO();
  GLuint getVAO();
  GLuint getUVBO();

  std::vector<GLuint> getIndexBuffer();

};

typedef std::shared_ptr<Mesh> MeshHandle;

