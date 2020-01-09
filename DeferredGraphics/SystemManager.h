#pragma once
#include "RenderingSystem.h"

class Scene;

class SystemManager
{
  RenderingSystem* renderer;

public:

  void Init(int windowWidth, int windowHeight);
  void Update(Scene& scene);
  void Shutdown();
};

