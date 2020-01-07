#pragma once
#include <glm/glm/glm.hpp>
#include "Mesh.h"
#include "Material.h"
#include "EngineTypes.h"

class MeshComponent
{
  MeshHandle mesh;
  MaterialHandle material;
  ShaderHandle shader;
  EntityPtr entity;

  public:
  MeshComponent(EntityPtr entity, MeshHandle meshHandle, ShaderHandle shaderHandle, MaterialHandle materialHandle):entity(entity),mesh(meshHandle), shader(shaderHandle), material(materialHandle)
  {
    
  }

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

