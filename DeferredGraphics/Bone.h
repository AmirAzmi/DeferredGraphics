#pragma once
#include "Line.h"
#include "Quaternion.h"

class Bone
{

public:

  Quaternion rotation;
  glm::mat4 world_matrix;
  glm::vec3 current_position;
  float length;

  Bone()
  {
  }

  Bone(glm::vec3 current_position, Quaternion rotation, float length):current_position(), rotation(rotation),length(length), world_matrix(glm::mat4(1))
  {
  }

  glm::vec3 getBoneEndPosition(glm::mat4 previous_bone_rotation)
  {
    return glm::vec4(current_position, 1) + previous_bone_rotation * rotation.quaternionToMatrix() * glm::vec4(1, 0, 0,1) * length;
  }
};

