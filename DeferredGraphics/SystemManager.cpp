#include "SystemManager.h"

void SystemManager::Init(int windowWidth, int windowHeight)
{
  //initialize the renderer
  renderer = new RenderingSystem(windowWidth, windowHeight);
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
