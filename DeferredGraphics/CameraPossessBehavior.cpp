#include <glm/glm/glm.hpp>
#include "CameraPossessBehavior.h"
#include "Entity.h"
#include "Imgui/imgui.h"

void CameraPossessBehavior::inspect()
{
  ImGui::DragFloat3("Camera Offset: ", &offset.x, 0.05, -10.0f, 10.0f);
}

void CameraPossessBehavior::Update(float delta_time)
{
  glm::vec3 eye = owner->position - offset;
  owner->scene.viewMatrix = glm::lookAt(eye, eye - owner->scene.cameraDirection, owner->scene.upDirection);
}

