#include "MovementBehavior.h"
#include "Input.h"
#include "Entity.h"
#include "Imgui/imgui.h"

void MovementBehavior::Update(float delta_time)
{
  if (Input::getKeyDown('W'))
  {
    owner->position.z += speed.z * delta_time;
  }

  if (Input::getKeyDown('A'))
  {
    owner->position.x += speed.x * delta_time;
  }

  if (Input::getKeyDown('S'))
  {
    owner->position.z -= speed.z * delta_time;
  }

  if (Input::getKeyDown('D'))
  {
    owner->position.x -= speed.x * delta_time;
  }
}

void MovementBehavior::inspect()
{
  ImGui::DragFloat3("Movement Speed: ",&speed.x);
}
