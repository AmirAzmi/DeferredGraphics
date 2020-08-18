#pragma once
#include "Behavior.h"

class MovementBehavior: public Behavior
{
  glm::vec3 speed;
public:

  MovementBehavior():speed(glm::vec3(0.06f,0.06f,0.06f))
  {

  }

  MovementBehavior(glm::vec3 speed) :speed(speed)
  {
  }

  void Update() override;
};

