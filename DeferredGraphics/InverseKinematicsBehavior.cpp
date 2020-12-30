#include "InverseKinematicsBehavior.h"
#include "Entity.h"
#include "Imgui/imgui.h"
#include <assimp/quaternion.h>
#include <cmath>

void InverseKinematicsBehavior::inspect()
{
  int z = iteration_count;

  ImGui::DragInt("Number of Iterations: ", &z);

  if (ImGui::IsItemDeactivatedAfterEdit())
  {
    iteration_count = z;
  }

  ImGui::DragFloat3("Target location: ", &target.x);
  ImGui::DragFloat("Threshhold: ", &threshold);

  ImGui::DragFloat("Angle bone 0: ", &bone_angle_0);
  ImGui::DragFloat("Angle bone 1: ", &bone_angle_1);
  ImGui::DragFloat("Angle bone 2: ", &bone_angle_2);
  ImGui::DragFloat("Angle bone 3: ", &bone_angle_3);
  ImGui::DragFloat("t: ", &debug_float, 0.001f);
  ImGui::Text("Accumulator: %f ", timer_accumulator);

  ImGui::DragFloat("Timer: ", &timer);

  if (ImGui::Button("Step"))
  {
    iteration_count++;
  }
}

void InverseKinematicsBehavior::Update(float delta_time)
{
  debugSystem->isDrawLinesOn = true;
  debugSystem->isDrawPointsOn = true;

  //draw initial set of lines
  Line line(glm::vec4(bones[0].current_position, 1), glm::vec4(bones[0].getBoneEndPosition(glm::mat4(1)), 1), glm::vec4(1, 1, 1, 1));
  Line line2(glm::vec4(bones[1].current_position, 1), glm::vec4(bones[1].getBoneEndPosition(bones[0].world_matrix), 1), glm::vec4(1, 0, 0, 1));
  Line line3(glm::vec4(bones[2].current_position, 1), glm::vec4(bones[2].getBoneEndPosition(bones[1].world_matrix), 1), glm::vec4(0, 1, 0, 1));
  Line line4(glm::vec4(bones[3].current_position, 1), glm::vec4(bones[3].getBoneEndPosition(bones[2].world_matrix), 1), glm::vec4(0, 0, 1, 1));
  lines.push_back(line);
  lines.push_back(line2);
  lines.push_back(line3);
  lines.push_back(line4);
  debugSystem->saveLines(lines);
  lines.clear();

  //draw initial set of points
  points.push_back(target);
  debugSystem->savePoints(points);
  points.clear();

  

  /*
  bones[0].rotation.setToRotatAboutAxis(glm::vec3(0, 0, 1), glm::radians(bone_angle_0));
  bones[1].rotation.setToRotatAboutAxis(glm::vec3(0, 0, 1), glm::radians(bone_angle_1));
  bones[2].rotation.setToRotatAboutAxis(glm::vec3(0, 0, 1), glm::radians(bone_angle_2));
  bones[3].rotation.setToRotatAboutAxis(glm::vec3(0, 0, 1), glm::radians(bone_angle_3));
  UpdateRotPos(0, glm::mat4(1));
  //*/

  if (iteration_count > 0)
  {
    for (int end = bones.size(); end > 0; --end)
    {
      //update position and rotation for each bone going up
      glm::mat4 local_rot = UpdateRotPos(0, glm::mat4(1));

      //End Effector Bone's end position
      glm::vec3 E = bones[3].getBoneEndPosition(local_rot);

      //used to calculate the angle and axis for rotation
      glm::vec3 T = target;
      glm::vec3 U = E - bones[end - 1].current_position;
      glm::vec3 V = T - bones[end - 1].current_position;

      //old rotation needed for concatenation of rotations
      //its in the parent bone coordinate system, while the newly calculated is in worlds coordinate system
      Quaternion old_rot = bones[end - 1].rotation;

      //calculate axis
      glm::vec3 wi = glm::vec3(0);

      if (glm::length(glm::cross(U, V)) != 0.0f)
      {
        wi = glm::cross(U, V) / glm::length(glm::cross(U, V));
      }


      //transform wi t bone to to world
      //wi = glm::inverse(bones[end - 2].world_matrix) * glm::vec4(wi, 0.0f);


      //calculate angle
      float val = clamp(glm::dot(U, V) / (glm::length(U) * glm::length(V)), 1.0f, -1.0f);
      float si = acosf(val); //this is in radians and we convert it to degrees

      /*************************************************************************************/
      /******************Everything Up to Here Seems Right *********************************/
      /*************************************************************************************/

      //set new rotation of bone - 1
      Quaternion rotation(wi, si);
      bones[end - 1].rotation = rotation * old_rot; //compute local rotation

      //update position and rotation for each bone going up
      local_rot = UpdateRotPos(0, glm::mat4(1));

      E = bones[3].getBoneEndPosition(local_rot);

      if (glm::length(T - E) < threshold)
      {
        solutionQuats.clear();
        solutionQuats.push_back(bones[0].rotation);
        solutionQuats.push_back(bones[1].rotation);
        solutionQuats.push_back(bones[2].rotation);
        solutionQuats.push_back(bones[3].rotation);
        iteration_count = 0;
        solution_found = true;

        Quaternion quat = Quaternion::Slerp(oldQuats[0], solutionQuats[0], 0);
        Quaternion quat_end = Quaternion::Slerp(oldQuats[0], solutionQuats[0], 1);

        break;
      }
    }
    --iteration_count;
  }

  if (solution_found == true)
  {

    if (timer_accumulator < timer)
    {

      for (int i = 0; i < oldQuats.size(); ++i)
      {

        Quaternion quat = Quaternion::Slerp(oldQuats[i], solutionQuats[i], timer_accumulator / timer);
        quat.normalized();

        //std::cout << "(" << timer_accumulator / timer << ") "<< i << ": " << quat << " "<< "\n\n";

        bones[i].rotation = quat;
      }

      UpdateRotPos(0, glm::mat4(1));
      timer_accumulator += delta_time;
    }
    else
    {
      timer_accumulator = 0.0f;
      solution_found = false;
    }

  }

}
