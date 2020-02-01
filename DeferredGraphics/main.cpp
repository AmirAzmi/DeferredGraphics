#include <glew/GL/glew.h>
#include <glfw/GLFW/glfw3.h>
#include <glm/glm/glm.hpp>

#include "Editor.h"
#include "SystemManager.h"
GLFWwindow* window;
int windowWidth = 1024;
int windowHeight = 768;
const char* glsl_version = "#version 430";

void processInput(GLFWwindow* window, Scene& scene);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
  // Initialise GLFW
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  // Setting up OpenGL properties
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(windowWidth, windowHeight, "Amir Azmi CS350", nullptr, nullptr);
  if (window == nullptr)
  {
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);



  //initialize the systems the scene will be using
  SystemManager systems;
  systems.Init(windowWidth, windowHeight);

  //intialize the scene
  Scene* scene = new Scene(windowWidth, windowHeight);
  scene->Init();

  //initialize the imgui editor
  Editor ImGuiEditor(*scene);

  //initialize the window
  ImGuiEditor.init(window, glsl_version);

  do
  {
    //input
    processInput(window, *scene);

    //render the window with the title Amir Azmi
    ImGuiEditor.preRender("Amir Azmi");

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    scene->PreRender(windowWidth, windowHeight);

    glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);


    //update the objects in the scene
    systems.Update(*scene, windowWidth, windowHeight);

    //render the scene
    scene->Render();

    //render the contents of ImGui
    ImGuiEditor.Render(*scene);

    //Call Imguie::End and other post render information
    ImGuiEditor.postRender();

    glfwSwapBuffers(window);
    glfwPollEvents();

  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  //free the memory for the systems
  systems.Shutdown();

  //delete the scene
  delete scene;

  ImGuiEditor.shutdown();
  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, Scene& scene)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye + glm::vec3(0.0f, 0.01f, 0.0f);
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.cameraDirection, scene.upDirection);
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}