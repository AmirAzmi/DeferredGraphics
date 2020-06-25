/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: the game loop fileS
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#include <glew/GL/glew.h>
#include <glfw/GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <iostream>
#include <fstream>

#include "Editor.h"
#include "SystemManager.h"

#include "JsonParser.h"


//window information
GLFWwindow* window;
const int windowWidth = 1024;
const int windowHeight = 768;
const char* glsl_version = "#version 430";

void processInput(GLFWwindow* window, Scene& scene);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

#ifdef _WIN32
extern "C" {
  __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int main()
{
  // Initialise GLFW
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  // Setting up OpenGL properties
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(mode->width, mode->height, "Amir Azmi CS350", nullptr, nullptr);
  if (window == nullptr)
  {
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);

  glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  //intialize the scene
  Scene scene(windowWidth, windowHeight);
  scene.Init();
  glfwSetWindowUserPointer(window, &scene);

  //initialize the systems the scene will be using
  SystemManager systems;
  systems.Init(scene, windowWidth, windowHeight);

  //initialize the imgui editor
  Editor ImGuiEditor(scene, systems);

  //initialize the window
  ImGuiEditor.init(window, glsl_version);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream("Resources\\Json\\Empty.area.json", std::ios::in);
  if (VertexShaderStream.is_open())
  {
    std::string Line;
    while (getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  }
  else
  {
    std::cout << " oof you fucked";
  }
  
  JsonValue level = jsonParser::toJson(VertexShaderCode);

  if (std::holds_alternative<JsonObjectPtr>(level))
  {
    JsonObjectPtr & stuff = std::get<JsonObjectPtr>(level);
    JsonArrayPtr & listOfEntities = std::get<JsonArrayPtr>(stuff->jsonObjects["entities"]);
    JsonObjectPtr& first_entity = std::get<JsonObjectPtr>(listOfEntities->jsonArrays[0]);
    JsonObjectPtr& componenets = std::get<JsonObjectPtr>(first_entity->jsonObjects["components"]);
    JsonObjectPtr& sprite_data = std::get<JsonObjectPtr>(componenets->jsonObjects["Sprite"]);
    std::string& material = std::get<std::string>(sprite_data->jsonObjects["material"]);

    std::cout << material;

  }

  do
  {
    //blocks input to viewport when im focused on an imgui input event, not window
    //but like literally in the capture input event mode
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
      processInput(window, scene);
    }

    //render the window with the title Amir Azmi
    ImGuiEditor.preRender("Amir Azmi");

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    scene.PreRender(windowWidth, windowHeight);

    glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    //update the objects in the scene
    systems.Update(scene, windowWidth, windowHeight);

    //render the scene
    scene.Render();

    //render the contents of ImGui
    ImGuiEditor.Render(scene, systems);

    //Call Imguie::End and other post render information
    ImGuiEditor.postRender();

    glfwSwapBuffers(window);
    glfwPollEvents();

  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  //MOVE THIS OUT
  //generate log right before freeing the data
  std::ofstream myfile("profiler.txt");
  if (myfile.is_open())
  {
    myfile << "Bounding Volume Hierarchy Information";
    myfile << "\nBounding Volume Hierarchy On: " << systems.debugRenderer->isBVHOn;
    myfile << "\nBounding Volume Hierarchy Levels: " << systems.debugRenderer->numberOfLevels;
    myfile << "\nBounding Volume Hierarchy Draw Calls: " << systems.debugRenderer->bvh_draw_calls;
    myfile << "\nBounding Volume Hierarchy Memory Usage: " << systems.debugRenderer->memory_usage_from_BVHTopeDown;

    myfile << "\n\nOctree Information";
    myfile << "\nOctree On: " << systems.debugRenderer->isSubObjectDrawOn;
    myfile << "\nOctree Levels: " << systems.debugRenderer->levelForOneObject;
    myfile << "\nOctree Draw Calls: " << systems.debugRenderer->octree_draw_calls;
    myfile << "\nOctree Memory Usage: " << systems.debugRenderer->memory_usage_from_octree;

    myfile << "\n\nDebug Rendering System Draw Calls";
    myfile << "\n\nTotal Draw Calls: " << systems.debugRenderer->bvh_draw_calls + systems.debugRenderer->octree_draw_calls;
    myfile << "\nTotal Temporary Memory Usage: " << systems.debugRenderer->memory_usage_from_octree + systems.debugRenderer->memory_usage_from_BVHTopeDown << " out of " << linearAllocator.memory_size;

    myfile << "\n\nGraphics Settings";
    myfile << "\nIs Bloom On: " << systems.renderer->bloom;
    myfile << "\nIs Depth Copy On: " << systems.renderer->depthCopyToggle;
    myfile << "\nIs Depth G Buffer Split Screen On: " << systems.renderer->splitScreen;
    myfile << "\nIs Uncharted 2 Tone Mapping On: " << systems.renderer->exposure_tone_mapping;
    myfile << "\nIs Gamma On: " << systems.renderer->gamma;
    myfile << "\nIs Exposure Tone Mapping On: " << systems.renderer->exposure_tone_mapping;

    if (systems.renderer->exposure_tone_mapping == true)
    {
      myfile << "\nExposure Value: " << systems.renderer->exposure;
    }

    myfile << "\n\nDebug Rendering System Frame Per Second: " << systems.debugRenderer->rendering_sytem_elapsed_time.count();
    myfile << "\nMain Rendering System Frame Per Second: " << systems.renderer->rendering_sytem_elapsed_time.count();
    myfile << "\nTotal Rendering System Frame Per Second: " << systems.debugRenderer->rendering_sytem_elapsed_time.count() + systems.renderer->rendering_sytem_elapsed_time.count();
    myfile << "\nFrame Per Second: " << 1000.0f / ImGui::GetIO().Framerate;

    myfile.close();
  }
  else
  {
    std::cout << "Unable to generate profiler log file.\n";
  }

  //free the memory for the systems
  systems.Shutdown();

  //free memory for editor
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
    scene.eyePosition = eye + scene.cameraDirection * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye - glm::normalize(glm::cross(scene.cameraDirection, scene.upDirection)) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye - scene.cameraDirection * 0.1f;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye + glm::normalize(glm::cross(scene.cameraDirection, scene.upDirection)) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye - glm::vec3(0.0f, 1.0f, 0.0f) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye + glm::vec3(0.0f, 1.0f, 0.0f) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  //get cursor postion in screen space
  glfwGetCursorPos(window, &xpos, &ypos);

}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    //TODO: dont know what the heck is happening here since I believe the math is correct going from screen space to world space but somewhere
    //it after this point the world space ccordinates for the mouse are just messed up, ive like triple checked the matrices and how they are made
    //the view matrix is calculated with window dimension as its aspect ratio, i tried sceen coordinates but nothing really changed
    //perspective matrix is also calculated with the window dimension and not screen cooridinates, i also tried this as well but nope
    //the ray is in screen space so the first thing we do is need to get into view space

    //perspective divide => DO NOT do this because a ray does not have depth


    //create ray from camera position in world space to cursor position in world space
    //will have to come back to you later, i tried
  }

}