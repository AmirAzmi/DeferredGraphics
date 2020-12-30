#pragma once
#include <glm/glm/glm.hpp>

//never edit this class because it is uploaded to the gpu in a specific order
class Line
{
public:
  glm::vec4 start;
  glm::vec4 color_start;

  glm::vec4 end;
  glm::vec4 color_end;

  Line(glm::vec4 start_point, glm::vec4 end_point,glm::vec4 color): start(start_point), end(end_point), color_start(color), color_end(color)
  {
  }
};

