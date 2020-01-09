#pragma once
#include <glm/glm/glm.hpp>
#include "Mesh.h"
#include "Material.h"
#include "EngineTypes.h"

class MeshComponent
{
  MeshHandle mesh;        //8 bytes
  MaterialHandle material;//8 bytes
  ShaderHandle shader;    //8 bytes
  EntityPtr entity;       //8 bytes
  //-------------------------------
  //total: 32 bytes

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

