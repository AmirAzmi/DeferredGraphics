#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>

class LightSystem
{
  GLuint ssboID;
  GLuint blockIndex;
  GLuint ssboBindingPointIndex;

  public:
  LightSystem();

  void Init(GLuint programID);
};

