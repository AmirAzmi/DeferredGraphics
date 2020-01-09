#include "SystemManager.h"

void SystemManager::Init(int windowWidth, int windowHeight)
{
  //initialize the renderer
  renderer = new RenderingSystem(windowWidth, windowHeight);
}

void SystemManager::Update(Scene& scene, int windowWidth, int windowHeight)
{
  renderer->Update(scene, windowWidth, windowHeight);
}

void SystemManager::Shutdown()
{
  //delete in reverse order
  delete renderer;
  
}
