#pragma once
#include <glm/glm/glm.hpp>
#include <math.h>

class Bezier
{
public:
  glm::vec4 C0;
  glm::vec4 C1;
  glm::vec4 C2;
  glm::vec4 C3;


  Bezier()
  {
  }

  Bezier(glm::vec4 C0, glm::vec4 C1, glm::vec4 C2, glm::vec4 C3):C0(C0), C1(C1), C2(C2), C3(C3)
  {
  };

  glm::vec4 GetCurrentPosition(float t)
  {
    glm::vec4 CurrentPosition;

    CurrentPosition = 
      (std::pow((1 - t),3) * C0) + 
      (3 * std::pow((1 - t), 2) * t * C1) + 
      (3 * (1 - t) * std::pow(t, 2) * C2) + 
      (std::pow(t, 3) * C3);

    return CurrentPosition;
  }

};