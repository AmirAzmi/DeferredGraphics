#pragma once
#include "Behavior.h"
#include "Bezier.h"
#include "Line.h"

class BezierFollowBehavior : public Behavior
{
public:
  std::vector<float> LengthBetweenPoints;
  std::vector<float> summedDist;
  std::vector<Line> lines;
  Bezier bezier;
  glm::vec4 C0{ 0.0f,0.5f,-2.0f,1 };
  glm::vec4 C1{ 3.0f,0.5f,0.0f,1 };
  glm::vec4 C2{ 1.0f,0.5f,2.0f,1 };
  glm::vec4 C3{ -2.0f,0.5f,1.0f,1 };

  std::vector<glm::vec3> positions;

  float totalArcLength;
  float behaviorUpdateOverWrite = 1.0f / 144.0f;
  int FrameCount = 0; //144 is 1 second
  int NumbeOfFramesToPlay = 144;
  bool DrawCL = false;
  bool ResetFrameCount = false;
  bool EaseInOn = false;
  bool EaseOutOn = false;
  bool EaseInOutOn = false;
  bool Loop = false;
  bool FrameStep = false;
  bool DrawNewPathEveryLoop = false;
  bool arcTable = false;



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
  float Length(glm::vec3 currentPosition, glm::vec3 nextPosition);
  std::vector<float> getArcLengthTable(float time);
  std::vector<float> getgetDistancePerStepTable(std::vector<float> distBetweenPoints);
  float getTotalDistance(std::vector<float> distBetweenPoints);
};

