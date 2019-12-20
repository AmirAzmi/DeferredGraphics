#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <vector>
#include "Scene.h"

class RenderingSystem
{
  GLuint projectionMatrixID;
  GLuint viewMatrixID;
  void Update(Scene& scene);
};

