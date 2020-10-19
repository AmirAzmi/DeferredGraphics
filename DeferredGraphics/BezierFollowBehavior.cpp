#include <glm/glm/glm.hpp>
#include "BezierFollowBehavior.h"
#include "Entity.h"
#include "Imgui/imgui.h"
#include "DebugRenderingSystem.h"

extern DebugRenderingSystem* debugSystem;// global debug renderer

void BezierFollowBehavior::inspect()
{

  ImGui::Text("Current FrameCount:", &FrameCount);
  ImGui::DragInt("Number of Frames To Play", &NumbeOfFramesToPlay);
  ImGui::DragFloat3("Control Point 0: ", &C0.x);
  ImGui::DragFloat3("Control Point 1: ", &C1.x);
  ImGui::DragFloat3("Control Point 2: ", &C2.x);
  ImGui::DragFloat3("Control Point 3: ", &C3.x);

  ImGui::Checkbox("Draw Control Points and Line:", &debugSystem->isDrawPointsOn);
  ImGui::Checkbox("Ease In", &EaseInOn);
  ImGui::Checkbox("Ease Out", &EaseOutOn);
  ImGui::Checkbox("Ease In Out", &EaseInOutOn);
  ImGui::Checkbox("Loop Animation", &Loop);
  ImGui::Checkbox("Reset FrameCount", &ResetFrameCount);
}

void BezierFollowBehavior::Update(float delta_time)
{
  UpdateControlPoints();

  std::vector<glm::vec3> points{ C0, C1, C2, C3 };

  float t = delta_time * FrameCount;


  if (ResetFrameCount == true)
  {
    FrameCount = 0;
    //update owner position
    owner->position = bezier.GetCurrentPosition(t);
    positions.clear();
  }

  if (FrameCount <= NumbeOfFramesToPlay && ResetFrameCount == false)
  {
    //apply ease in here
    if (EaseInOn)
    {
      t = t * t * t;
    }

    if (EaseOutOn)
    {
      t = 1 - std::pow(1 - t, 3);
    }

    if (EaseInOutOn)
    {
      t = t < 0.5 ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3) / 2;
    }

    //update owner position
    owner->position = bezier.GetCurrentPosition(t);
    positions.push_back(owner->position);

    //update owner rotation

    //update frame count
    FrameCount += 1;
  }
  else
  {
    //reset framecount
    if (Loop == true)
    {
      FrameCount = 0;
    }
  }

  //draw points
  //draw curve
  if (debugSystem != nullptr)
  {
    if (debugSystem->isDrawPointsOn == true)
    {
      debugSystem->savePoints(points);
      debugSystem->savePoints(positions);
    }
  }
}

void BezierFollowBehavior::UpdateControlPoints()
{
  bezier.C0 = C0;
  bezier.C1 = C1;
  bezier.C2 = C2;
  bezier.C3 = C3;
}


