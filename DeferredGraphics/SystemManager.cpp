#include "SystemManager.h"

void SystemManager::Init()
{
  //initialize the renderer
  renderer = new RenderingSystem();
}

void SystemManager::Update(Scene& scene)
{
  renderer->Update(scene);
}

void SystemManager::Shutdown()
{
  //delete in reverse order
  delete renderer;
  
}
