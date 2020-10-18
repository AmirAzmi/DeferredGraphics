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

  ImGui::Checkbox("Reset FrameCount", &ResetFrameCount);
}

void BezierFollowBehavior::Update(float delta_time)
{
  UpdateControlPoints();

  if (ResetFrameCount == true)
  {
    FrameCount = 0;
    owner->position = bezier.GetCurrentPosition(delta_time * FrameCount);
  }

  if (FrameCount <= NumbeOfFramesToPlay && ResetFrameCount == false)
  {
    owner->position = bezier.GetCurrentPosition(delta_time * FrameCount);

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


