#pragma once
#include <glm/glm/glm.hpp>
#include "Material.h"
#include "Entity.h"
#include "Mesh.h"

class MeshComponent
{
  MeshHandle mesh;
  MaterialHandle material;
  ShaderHandle shader;
  EntityPtr entity;

  MeshComponent(EntityPtr entity);

public:
  GLuint DrawMode = GL_TRIANGLES;


  EntityPtr getEntityPtr()
  {
    return entity;
  }

  MeshHandle getMesh()
  {
    return mesh;
  }

  MaterialHandle getMaterial()
  {
    return material;
  }

  ShaderHandle getShader()
  {
    return shader;
  }

  GLuint getDrawMode()
  {
    return DrawMode;
  }
};

using MeshComponentPtr = MeshComponent*;