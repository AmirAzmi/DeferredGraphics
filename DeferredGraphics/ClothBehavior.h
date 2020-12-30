#pragma once
#include "DebugRenderingSystem.h"
#include "Behavior.h"
#include <glm/glm/glm.hpp>
#include <vector>

struct Point
{
  glm::vec4 position; //point position
  glm::vec4 velocity; //direction vector
  glm::vec4 acceleration; //acceleration
  glm::vec4 previousPosition; //point position
  bool isPinned = false;

};

struct Joint
{
  Point* p0;
  Point* p1;
  float distance_between_joints;
  float ks;
  float ds;
};

extern DebugRenderingSystem* debugSystem;// global debug renderer

class ClothBehavior : public Behavior
{
public:

  std::vector<Joint> Joints;
  std::vector<std::vector<Point>> ClothPoints;
  std::vector<glm::vec3> points;
  std::vector<Line> lines;
  glm::vec4 gravity;
  glm::vec4 wind;
  int ClothWidth = 25;
  int ClothHeight = 25;
  float time = 0.0f;

  ClothBehavior() : ClothWidth(25), ClothHeight(25)
  {
    gravity = glm::vec4(0.0f, -10.0f, 0.0f, 0.0f);
    wind = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    ClothPoints.resize(ClothHeight);
    for (int i = 0; i < ClothPoints.size(); i++)
    {
      ClothPoints[i].resize(ClothWidth);

      for (int j = 0; j < ClothPoints[i].size(); j++)
      {
        ClothPoints[i][j] = Point{ glm::vec4(i * 0.25f, j * 0.25f, 0.0f,1.0f),glm::vec4(0,0,0,0), glm::vec4(0) };

        points.push_back(ClothPoints[i][j].position);
      }
    }

    for (int i = 0; i < ClothPoints.size(); ++i)
    {
      for (int j = 0; j < ClothPoints[i].size(); ++j)
      {

        if (j + 1 != ClothPoints[i].size())
        {

          Joint joint{ &ClothPoints[i][j], &ClothPoints[i][j + 1], .25f, 1000, 50 };
          Joints.push_back(joint);
        }

        if (i + 1 != ClothPoints.size())
        {

          Joint joint1{ &ClothPoints[i][j], &ClothPoints[i + 1][j], .25f, 1000, 50 };
          Joints.push_back(joint1);
        }

        if (j + 1 != ClothPoints[i].size() && i + 1 != ClothPoints.size())
        {
          Joint joint2{ &ClothPoints[i][j], &ClothPoints[i + 1][j + 1], sqrtf(2) * 0.25f, 1000, 50 };
          Joints.push_back(joint2);
        }

        if (j + 1 != ClothPoints[i].size() && i + 1 != ClothPoints.size())
        {
          Joint joint3{ &ClothPoints[i][j + 1], &ClothPoints[i + 1][j], sqrtf(2) * 0.25f, 1000, 50 };
          Joints.push_back(joint3);
        }
      }
    }
  }
  void inspect() override;
  void Update(float delta_time) override;

  std::string name() override
  {
    return std::string("Cloth Physics Simulation");
  }
};
