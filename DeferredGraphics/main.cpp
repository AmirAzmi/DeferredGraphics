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

#include <Windows.h>
#include <glew/GL/glew.h>
#include <glfw/GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <iostream>
#include <fstream>
#include "Input.h"
#include "Editor.h"
#include "SystemManager.h"
#include "JsonParser.h"


//window information
GLFWwindow* window;
const int windowWidth = 1024;
const int windowHeight = 768;
const char* glsl_version = "#version 430";
float lastX = windowWidth / 2;
float lastY = windowHeight / 2;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;


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

  double lastFrame = 0.0f;
  DEVMODE lpDevMode;
  memset(&lpDevMode, 0, sizeof(DEVMODE));
  lpDevMode.dmSize = sizeof(DEVMODE);
  double dt;


  if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode) == 0)
  {
    dt = 1.0 / 144.0;
  }
  else
  {
    dt = 1.0f / lpDevMode.dmDisplayFrequency;
  }

  double currentTime = glfwGetTime();

  // Setting up OpenGL properties
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(mode->width, mode->height, "Trifrost Engine", nullptr, nullptr);
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

  //Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

  //JSON testing
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
    JsonObjectPtr& stuff = std::get<JsonObjectPtr>(level);
    JsonArrayPtr& listOfEntities = std::get<JsonArrayPtr>(stuff->jsonObjects["entities"]);
    JsonObjectPtr& first_entity = std::get<JsonObjectPtr>(listOfEntities->jsonArrays[0]);
    JsonObjectPtr& componenets = std::get<JsonObjectPtr>(first_entity->jsonObjects["components"]);
    JsonObjectPtr& sprite_data = std::get<JsonObjectPtr>(componenets->jsonObjects["Sprite"]);
    std::string& material = std::get<std::string>(sprite_data->jsonObjects["material"]);

    std::cout << material;

  }

  do
  {
    const double currentFrame = glfwGetTime();
    double frameTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //std::cout << "frameTime: "<< frameTime <<'\n';
    //std::cout << "frame rate: " << 1/frameTime <<'\n';

    /*
    //Sytem Order Update:
    //input
    //logic
    //physics
    //graphics
    */

    glfwPollEvents();

    //blocks input to viewport when im focused on an imgui input event, not window
    //but like literally in the capture input event mode
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
      Input::update();
      processInput(window, scene);
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    scene.PreRender(windowWidth, windowHeight, dt);

    //render the window with the title Amir Azmi
    ImGuiEditor.preRender("Amir Azmi");

    glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    //render the contents of ImGui
    ImGuiEditor.Render(scene, systems);


    //render the scene aka update the logic for all behaviors
    scene.Render(dt);

    //update the objects in the scene aka the rendering of objects
    systems.Update(scene, windowWidth, windowHeight);

    //Call Imguie::End and other post render information
    ImGuiEditor.postRender();

    scene.PostRender(dt);

    glfwSwapBuffers(window);

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

  /*if (Input::getKeyDown('W'))
  {

    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye + scene.cameraDirection * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (Input::getKeyDown('A'))
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye - glm::normalize(glm::cross(scene.cameraDirection, scene.upDirection)) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (Input::getKeyDown('S'))
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye - scene.cameraDirection * 0.1f;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (Input::getKeyDown('D'))
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye + glm::normalize(glm::cross(scene.cameraDirection, scene.upDirection)) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (Input::getKeyDown('Q'))
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye - glm::vec3(0.0f, 1.0f, 0.0f) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }

  if (Input::getKeyDown('E'))
  {
    glm::vec3 eye = scene.getEyePosition();
    scene.eyePosition = eye + glm::vec3(0.0f, 1.0f, 0.0f) * scene.cameraSpeed;
    scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), (float)windowWidth / (float)windowHeight, scene.nearDistance, scene.farDistance);
    scene.viewMatrix = glm::lookAt(scene.eyePosition, scene.eyePosition + scene.cameraDirection, scene.upDirection);
  }*/
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

}