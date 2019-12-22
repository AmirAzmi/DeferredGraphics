#pragma once
#include "RenderingSystem.h"

class Scene;

class SystemManager
{
  RenderingSystem* renderer;

public:

  void Init();
  void Update(Scene& scene);
  void Shutdown();
};

