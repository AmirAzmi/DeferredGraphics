#include "MovementBehavior.h"
#include "Input.h"
#include "Entity.h"

void MovementBehavior::Update()
{
  if (Input::getKeyDown('W'))
  {
    owner->position.z += speed.z;
  }

  if (Input::getKeyDown('A'))
  {
    owner->position.x += speed.x;
  }

  if (Input::getKeyDown('S'))
  {
    owner->position.z -= speed.z;
  }

  if (Input::getKeyDown('D'))
  {
    owner->position.x -= speed.x;
  }
}
