#include "Input.h"
#include <glfw/GLFW/glfw3.h>

static bool keyState[256];
static bool previousKeyState[256];
extern GLFWwindow* window;

void Input::update()
{
  for (int i = 0; i < 256; ++i)
  {
    previousKeyState[i] = keyState[i];
    keyState[i] = glfwGetKey(window, i);
  }
}

bool Input::getKeyDown(int key)
{
  return keyState[key];
}

bool Input::getKeyUp(int key)
{
  return !keyState[key];
}

bool Input::keyChangeThisFrame(int key)
{
  return previousKeyState[key] ^ keyState[key];
}
