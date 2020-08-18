#include <glm/glm/glm.hpp>
#include "CameraPossessBehavior.h"
#include "Entity.h"

void CameraPossessBehavior::Update()
{
  glm::vec3 eye = owner->position - offset;
  owner->scene.viewMatrix = glm::lookAt(eye, eye - owner->scene.cameraDirection, owner->scene.upDirection);
}
