#pragma once
#include "Behavior.h"
#include "Bone.h"
#include "DebugRenderingSystem.h"
#include <array>

extern DebugRenderingSystem* debugSystem;// global debug renderer

class InverseKinematicsBehavior : public Behavior
{

public:

  std::array<Bone, 4> bones; //the number of bones in the scene
  std::vector<Line> lines;
  std::vector < glm::vec3 > points;
  glm::vec3 target; //where the bones need to rotate towards
  float threshold; //error margin
  float bone_angle_0;
  float bone_angle_1;
  float bone_angle_2;
  float bone_angle_3;
  float timer_accumulator;
  float timer;
  float debug_float;
  int iteration_count; //number of iterations
  bool solution_found;

  std::vector<Quaternion> oldQuats;
  std::vector<Quaternion> solutionQuats;

  std::string name() override
  {
    return std::string("Inverse Kinematics");
  }

  InverseKinematicsBehavior()
  {
    solution_found = false;
    timer_accumulator = 0.0f;
    timer = 10.0f;
    threshold = 0.1f;
    iteration_count = 0;
    target = glm::vec3(2, 2, 0);

    for (int i = 0; i < bones.size(); ++i)
    {
      //set first bone
      if (i == 0)
      {
        bones[i].current_position = glm::vec3(0, 0, 0);
      }
      else //set all the other bones
      {
        bones[i].current_position = bones[i - 1].getBoneEndPosition(glm::mat4(1));
      }

      bones[i].rotation = Quaternion(1, 0, 0, 0);
      bones[i].length = 1;
      bones[i].world_matrix = glm::mat4(1);

      oldQuats.push_back(bones[i].rotation);
    }
  }


  glm::mat4 UpdateRotPos(int bone_index, glm::mat4 local_rotation)
  {
    for (int i = bone_index + 1; i < bones.size(); ++i)
    {
      bones[i].current_position =
        glm::vec4(bones[i - 1].getBoneEndPosition(local_rotation), 1); //point

      local_rotation = local_rotation * bones[i - 1].rotation.quaternionToMatrix();
      bones[i - 1].world_matrix = local_rotation;
    }

    return local_rotation;
  }



  void inspect() override;
  void Update(float delta_time) override;

};

