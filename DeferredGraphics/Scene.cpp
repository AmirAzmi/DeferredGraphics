#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight):fov(90.0f), nearDistance(0.1f), farDistance(1000.0f), 
eyePosition(glm::vec3(0.0f,0.0f,5.0f)), cameraDirection(glm::vec3(0.0f, 0.0f, 0.0f)), upDirection(glm::vec3(0.0f, 1.0f, 0.0f))
{
  projectionMatrix = glm::perspective(glm::radians(fov), (float)windowWidth / windowHeight, nearDistance, farDistance);
  viewMatrix = glm::lookAt(eyePosition, cameraDirection, upDirection);
}

std::vector<MeshComponentPtr> & Scene::getMeshes()
{
  return ListOfTypes.get<MeshComponentPtr>();
}

void Scene::Init()
{

  //declare the shaders that will be used for this scene
  Shader phongShader("PhongShading.vert","PhongShading.frag");

  //setup the preliminaries to the mesh component
  //necessary
  // ->add a mesh to the component
  // ->add a shader to the component
  // ->have a refrence to the entity

  //optional
  // ->add a material to the component

  //use the shader for the scene currently
  phongShader.UseShader();

  //add objects to the scene
  EntityPtr objectOne = addEntity("Object One");

  //add a mesh to the component
  Mesh mesh = Mesh("Resources//cube.obj");

  //note ask shareef how to use constructors but refrence as handles?
  //add a mesh component pointer to the object with the setup from the prelims
  MeshComponentPtr meshComp = objectOne->add<MeshComponentPtr>(objectOne, mesh, phongShader);

  //manipulate the properties of the object by getting it from the component
  meshComp->getEntityPtr()->scale = glm::vec3(2.0f,2.0f,2.0f);
  
}

void Scene::PreRender()
{
}

void Scene::Render()
{
}

void Scene::PostRender()
{
}

EntityPtr Scene::addEntity(std::string name)
{
  //add the entity with the given name
  ListOfEntities.push_back(new Entity(name, *this));

  //return the entity since we know its at the back
  return ListOfEntities.back();

}

EntityPtr Scene::findEntity(std::string name)
{
  //if the list is not empty
  if (!ListOfEntities.empty())
  {
    //loop through the entity list
    for (int i = 0; i < ListOfEntities.size(); ++i)
    {
      //look for the entity specified
      if (ListOfEntities[i]->name == name)
      {
        //return the potr to that entity
        return ListOfEntities[i];
      }
    }

    //could not find entity in the list of entities
    return nullptr;
  }
}

void Scene::removeEntity(std::string name)
{
  //if the list is not empty
  if (!ListOfEntities.empty())
  {
    //loop through the list
    for (int i = 0; i < ListOfEntities.size(); ++i)
    {
      //look for the first specified entity
      if (ListOfEntities[i]->name == name)
      {
        //delete the entity at that location
        delete ListOfEntities[i];

        //remove the entity at that point
        ListOfEntities.erase(ListOfEntities.begin() + i);
 
        //break out of the loop because you removed the entity
        break;
      }
    }
  }
}

glm::mat4 Scene::getProjectionMatrix()
{
  return projectionMatrix;
}

glm::mat4 Scene::getViewMatrix()
{
  return viewMatrix;
}

glm::vec3 Scene::getEyePosition()
{
  return eyePosition;
}

glm::vec3 Scene::getCameraDirection()
{
  return cameraDirection;
}

glm::vec3 Scene::getUpDirection()
{
  return upDirection;
}

float Scene::getNearDistance()
{
  return nearDistance;
}

float Scene::getFarDistance()
{
  return farDistance;
}

float Scene::getFOV()
{
  return fov;
}
