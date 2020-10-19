#pragma once
#include "Behavior.h"
#include "Bezier.h"

class BezierFollowBehavior : public Behavior
{
public:
  Bezier bezier;
  glm::vec4 C0{0,0,1,1};
  glm::vec4 C1{3,3,1,1};
  glm::vec4 C2{5,0,1,1};
  glm::vec4 C3{8,3,1,1};

  std::vector<glm::vec3> positions;

  int FrameCount = 0; //144 is 1 second
  int NumbeOfFramesToPlay = 144;
  bool DrawCL = false;
  bool ResetFrameCount = false;
  bool EaseInOn = false;
  bool EaseOutOn = false;
  bool EaseInOutOn = false;
  bool Loop = false;
  bool FrameStep = false;


  BezierFollowBehavior()
  {
    bezier = Bezier(C0, C1, C2, C3);
  }

  std::string name() override
  {
    return std::string("Bezier Follow");
  }

  void inspect() override;
  void Update(float delta_time) override;
  void UpdateControlPoints();
};

