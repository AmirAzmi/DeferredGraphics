#include <glm/glm/glm.hpp>
#include "BezierFollowBehavior.h"
#include "Entity.h"
#include "Imgui/imgui.h"

void BezierFollowBehavior::inspect()
{

  ImGui::Text("Current FrameCount:", &FrameCount);
  ImGui::DragInt("Number of Frames To Play", &NumbeOfFramesToPlay);
  ImGui::DragFloat4("Control Point 0: ", &C0.x);
  ImGui::DragFloat4("Control Point 1: ", &C1.x);
  ImGui::DragFloat4("Control Point 2: ", &C2.x);
  ImGui::DragFloat4("Control Point 3: ", &C3.x);

  ImGui::Checkbox("Ease In", &EaseInOn);
  ImGui::Checkbox("Ease Out", &EaseOutOn);
  ImGui::Checkbox("Ease In Out", &EaseInOutOn);
  ImGui::Checkbox("Reset FrameCount", &ResetFrameCount);
}

void BezierFollowBehavior::Update(float delta_time)
{
  //draw points
  //draw curve

  float t = delta_time * FrameCount;

  UpdateControlPoints();

  if (ResetFrameCount == true)
  {
    FrameCount = 0;
    owner->position = bezier.GetCurrentPosition(t);
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

    owner->position = bezier.GetCurrentPosition(t);
    //update owner rotation

    FrameCount += 1;
  }
}

void BezierFollowBehavior::UpdateControlPoints()
{
  bezier.C0 = C0;
  bezier.C1 = C1;
  bezier.C2 = C2;
  bezier.C3 = C3;
}


