#pragma once
#include "Behavior.h"

class MovementBehavior : public Behavior
{
  glm::vec3 speed;

public:

  MovementBehavior() :speed(glm::vec3(2.0f, 2.0f, 2.0f))
  {

  }

  MovementBehavior(glm::vec3 speed) :speed(speed)
  {
  }

  void Update(float delta_time) override;
};

