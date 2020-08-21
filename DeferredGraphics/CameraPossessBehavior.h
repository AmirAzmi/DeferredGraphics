#pragma once
#include "Behavior.h"

class CameraPossessBehavior : public Behavior
{
public:

  glm::vec3 offset;

  CameraPossessBehavior(glm::vec3 off):offset(off)
  {
  }

  std::string name() override
  {
    return std::string("Camera Possess");
  }

  void inspect() override;
  void Update(float delta_time) override;
};

