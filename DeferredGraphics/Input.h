#pragma once
namespace Input
{
  void update();

  bool getKeyDown(int key);
  bool getKeyUp(int key);
  bool keyChangeThisFrame(int key);

}
