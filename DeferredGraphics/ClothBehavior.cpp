#include "ClothBehavior.h"
#include "Entity.h"
#include "Imgui/imgui.h"
#include "DebugRenderingSystem.h"

//https://icculus.org/~lucasw/Dynamics/Spring%20Tutorial.html

void ClothBehavior::inspect()
{
  ImGui::DragFloat4("gravity:", &gravity.x);
  ImGui::DragFloat4("wind:", &wind.x);
}

void ClothBehavior::Update(float delta_time)
{
  debugSystem->isDrawPointsOn = true;
  debugSystem->isDrawLinesOn = true;
  debugSystem->savePoints(points);
  points.clear();

  for (int i = 0; i < ClothPoints.size(); i++)
  {
    for (int j = 0; j < ClothPoints[i].size(); j++)
    {
      ClothPoints[i][j].acceleration = gravity + wind;
      points.push_back(ClothPoints[i][j].position);
    }
  }

  //updating the forces for each point in the joints
  for (int i = 0; i < Joints.size(); i++)
  {
    //F = -kx

    //this is the vector between the first two points
    glm::vec4 x = Joints[i].p1->position - Joints[i].p0->position;

    float length = glm::length(x);

    //calculate displacement
    float displacement = length - Joints[i].distance_between_joints;

    //calculate Spring Normal
    glm::vec4 springN = x / length;

    //forces needed to bring the point back into place
    glm::vec4 restoreForce = springN * (displacement * Joints[i].ks);

    Joints[i].p0->acceleration += restoreForce;
    Joints[i].p1->acceleration -= restoreForce;

    //this code is for dampening
    glm::vec4 deltaV = (Joints[i].p1->velocity - Joints[i].p0->velocity);
    glm::vec4 damperForce = springN * (x * Joints[i].ks) + glm::dot(springN, deltaV) * Joints[i].ds;

    Joints[i].p0->acceleration -= damperForce;
    Joints[i].p1->acceleration += damperForce;

    //drawing of the joints
    if (i % 4 == 0)
    {
      Line line{ Joints[i].p0->position, Joints[i].p1->position, glm::vec4(1,0,0,1) };
      lines.push_back(line);
    }
    else if (i % 4 == 1)
    {
      Line line{ Joints[i].p0->position, Joints[i].p1->position, glm::vec4(0,1,0,1) };
      lines.push_back(line);
    }
    else if (i % 4 == 2)
    {
      Line line{ Joints[i].p0->position, Joints[i].p1->position, glm::vec4(0,0,1,1) };
      lines.push_back(line);
    }
    else if (i % 4 == 3)
    {
      Line line{ Joints[i].p0->position, Joints[i].p1->position, glm::vec4(1,1,1,1) };
      lines.push_back(line);
    }
  }

  debugSystem->saveLines(lines);
  lines.clear();

  //setting up the velocity, acceleration
  for (int i = 0; i < ClothPoints.size(); i++)
  {
    for (int j = 0; j < ClothPoints[i].size() - 1; j++)
    {
      /*
      if (j == ClothPoints[j].size() - 1 && i == ClothPoints[i].size() - 1)
      {
        continue;
      }
      
      if (j == ClothPoints[j].size() - 2 && i == ClothPoints[i].size() - 2)
      {
        continue;
      }

      if (j == ClothPoints[j].size() - 3 && i == ClothPoints[i].size() - 3)
      {
        continue;
      }
      //*/

      ClothPoints[i][j].velocity += ClothPoints[i][j].acceleration * 0.5f * delta_time * delta_time;
      ClothPoints[i][j].position += ClothPoints[i][j].velocity * delta_time;

    }
  }

  time += delta_time;

}
