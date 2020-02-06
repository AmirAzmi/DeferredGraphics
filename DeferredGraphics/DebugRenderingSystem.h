#pragma once
#include "Scene.h"

class DebugRenderingSystem
{
  //projection and view matrix ID
  GLuint projectionMatrixID;
  GLuint viewMatrixID;

  //AABB Bounding Box data
  GLuint boundingBoxVAOID = 0;
  GLuint boundingBoxVBOID;
  GLuint boundingBoxUVID;
};

