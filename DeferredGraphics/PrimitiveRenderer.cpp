#include "PrimitiveRenderer.h"
#include "Entity.h"
#include "Scene.h"

void PrimitiveRenderer::createRectPrism(glm::vec3 scalar_dimension, Scene & m_Scene)
{
  //add a mesh to the component
  ModelHandle cube = std::make_shared<Model>("Resources/cube2.obj");
  
  //declare the Resources/Shaders that will be used for this scene
  ShaderHandle gBuffer = std::make_shared<Shader>("Resources/Shaders/gBuffer.vert", "Resources/Shaders/gBuffer.frag", true);

  MaterialHandle material = std::make_shared<Material>(gBuffer);

  EntityPtr l_Entity = m_Scene.addEntity("Poop");

  //add a mesh component pointer to the object with the setup from the prelims
  MeshComponentPtr meshComp = l_Entity->add<MeshComponent>(l_Entity, cube);

  //manipulate the properties of the object by getting it from the component
  meshComp->getEntityPtr()->scale = scalar_dimension;

  for (auto& m : meshComp->mesh->meshes)
  {
    m.material = material;

    //Any material information needed
    m.material->setVec4("diffuse_color", glm::vec4(0.3f, 1.0f, 0.3f, 1.0f));
    m.material->setFloat("specular_intensity", 1.0f);

    m.shader = gBuffer;
  }

  //add to vector if we want to know something about the total list of these type of entities
  m_PrimitiveEntities.push_back(l_Entity);

  //return l_Entity;
}

void PrimitiveRenderer::createSphere(float radius)
{
}

void PrimitiveRenderer::createCube(float scalar)
{
}
