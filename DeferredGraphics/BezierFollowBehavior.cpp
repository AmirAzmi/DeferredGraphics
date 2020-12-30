#include <glm/glm/glm.hpp>
#include "BezierFollowBehavior.h"
#include "Entity.h"
#include "Imgui/imgui.h"
#include "DebugRenderingSystem.h"

extern DebugRenderingSystem* debugSystem;// global debug renderer

void BezierFollowBehavior::inspect()
{

  ImGui::Text("Current FrameCount: ", &FrameCount);
  ImGui::DragFloat("behavior Update Time: ", &behaviorUpdateOverWrite);
  ImGui::DragInt("Number of Frames To Play: ", &NumbeOfFramesToPlay);
  ImGui::DragFloat3("Control Point 0: ", &C0.x);
  ImGui::DragFloat3("Control Point 1: ", &C1.x);
  ImGui::DragFloat3("Control Point 2: ", &C2.x);
  ImGui::DragFloat3("Control Point 3: ", &C3.x);

  ImGui::Checkbox("Draw Control Points and Line: ", &debugSystem->isDrawPointsOn);
  ImGui::Checkbox("Draw Control Line: ", &debugSystem->isDrawLinesOn);
  ImGui::Checkbox("Draw New Path Every Loop: ", &DrawNewPathEveryLoop);
  ImGui::Checkbox("Ease In: ", &EaseInOn);
  ImGui::Checkbox("Ease Out: ", &EaseOutOn);
  ImGui::Checkbox("Ease In Out: ", &EaseInOutOn);
  ImGui::Checkbox("Loop Animation: ", &Loop);
  ImGui::Checkbox("Reset FrameCount: ", &ResetFrameCount);
  ImGui::Checkbox("Calcuate ArcLength Table: ", &arcTable);
}

void BezierFollowBehavior::Update(float delta_time)
{
  delta_time = behaviorUpdateOverWrite;
  UpdateControlPoints();

  std::vector<glm::vec3> points{ C0, C1, C2, C3 };

  Line line(C0,C1,glm::vec4(1,1,1,1));
  Line line2(C1,C2,glm::vec4(1,1,1,1));
  Line line3(C2,C3,glm::vec4(1,1,1,1));
  lines.push_back(line);
  lines.push_back(line2);
  lines.push_back(line3);

  float t = delta_time * FrameCount;
  float t_next = delta_time * (FrameCount + 1);
  float t_prev = delta_time * (FrameCount - 1);


  if (arcTable == true)
  {
    //generate arc length table
    LengthBetweenPoints = getArcLengthTable(delta_time);
    summedDist = getgetDistancePerStepTable(LengthBetweenPoints);
    totalArcLength = getTotalDistance(LengthBetweenPoints);
    arcTable = false;
  }

  if (ResetFrameCount == true)
  {
    FrameCount = 0;
    float t = delta_time * FrameCount;
    float t_next = delta_time * (FrameCount + 1);

    owner->position = bezier.GetCurrentPosition(t);

    //update rotation
    glm::vec3 forward_vector = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 next_position = bezier.GetCurrentPosition(t_next);
    glm::vec3 direction_from_old_pos_to_new_pos = next_position - owner->position;
    float new_angle = atan2f(direction_from_old_pos_to_new_pos.z, direction_from_old_pos_to_new_pos.x);
    //float new_angle = acos(glm::dot(forward_vector, direction_from_old_pos_to_new_pos) / (glm::length(forward_vector) * glm::length(direction_from_old_pos_to_new_pos)));
    owner->angle = glm::degrees(-new_angle) + 90.0f;


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

    //update rotation
    glm::vec3 forward_vector = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 next_position = bezier.GetCurrentPosition(t_next);
    glm::vec3 direction_from_old_pos_to_new_pos = next_position - owner->position;

    if (EaseOutOn)
    {
      direction_from_old_pos_to_new_pos = owner->position - next_position;
    }

    glm::vec3 direction = direction_from_old_pos_to_new_pos - forward_vector;
    float new_angle = atan2f(direction_from_old_pos_to_new_pos.z, direction_from_old_pos_to_new_pos.x);
    //float new_angle = acos(glm::dot(forward_vector, direction_from_old_pos_to_new_pos) / (glm::length(forward_vector) * glm::length(direction_from_old_pos_to_new_pos)));

    owner->angle = glm::degrees(-new_angle) + 90.0f;
    //update owner position
    positions.push_back(owner->position);
    owner->position = bezier.GetCurrentPosition(t);


    //update frame count
    FrameCount += 1;
  }
  else
  {
    //reset framecount
    if (Loop == true)
    {
      FrameCount = 0;

      if (DrawNewPathEveryLoop)
      {
        positions.clear();
      }
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
      debugSystem->saveLines(lines);
      lines.clear();
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

float BezierFollowBehavior::Length(glm::vec3 currentPosition, glm::vec3 nextPosition)
{
  float dz = (nextPosition.z - currentPosition.z);
  float dy = (nextPosition.y - currentPosition.y);
  float dx = (nextPosition.x - currentPosition.x);

  return sqrtf((dz * dz) + (dx * dx));
}

std::vector<float> BezierFollowBehavior::getArcLengthTable(float time)
{
  LengthBetweenPoints.clear();
  for (int i = 0; i < NumbeOfFramesToPlay; ++i)
  {
    glm::vec3 current = bezier.GetCurrentPosition(time * i);
    glm::vec3 next = bezier.GetCurrentPosition(time * (i + 1));

    float dist = Length(current, next);
    LengthBetweenPoints.push_back(dist);
  }

  return LengthBetweenPoints;
}

std::vector<float> BezierFollowBehavior::getgetDistancePerStepTable(std::vector<float> distBetweenPoints)
{
  std::vector<float> summedDist;
  float sum = 0.0f;

  for (int i = 0; i < distBetweenPoints.size(); ++i)
  {
    sum += distBetweenPoints[i];

    std::cout << i << ", " << sum << "\n";

    summedDist.push_back(sum);
  }
  std::cout << std::endl;
  return summedDist;
}

float BezierFollowBehavior::getTotalDistance(std::vector<float> distBetweenPoints)
{
  float sum = 0.0f;

  for (int i = 0; i < distBetweenPoints.size(); ++i)
  {
    sum += distBetweenPoints[i];
  }

  return sum;
}


