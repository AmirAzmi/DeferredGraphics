#include <algorithm>
#include "DebugRenderingSystem.h"
#include "MeshComponent.h"
#include "AABB.h"

DebugRenderingSystem::DebugRenderingSystem(int windowWidth, int windowHeight): projectionMatrixID(-1), viewMatrixID(-1)
{
  forwardLightingShaderID = std::make_shared <Shader>("forwardLightingPass.vert", "forwardLightingPass.frag", false);
}

DebugRenderingSystem::~DebugRenderingSystem()
{

}

void DebugRenderingSystem::Update(Scene& scene, int windowWidth, int windowHeight)
{
  //set the active shader
  forwardLightingShaderID->UseShader();

  // gets all the current mesh components in th scene
  std::vector<MeshComponentPtr>& meshes = scene.getMeshes();

  //for each mesh component

  if (isAABBOn == true)
  {
    std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawAABB(mesh, scene); });
  }

  if (isOBBOn == true)
  {
    std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawOBB(mesh, scene); });
  }

  if (isSBBOn == true)
  {
    std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawSBB(mesh, scene); });
  }

}

void DebugRenderingSystem::drawAABB(MeshComponentPtr mesh, Scene& scene)
{
  std::vector<glm::uint> cubeIndices = 
  {
    4, 0, 3,
    4, 3, 7,
    2, 6, 7,
    2, 7, 3,
    1, 5, 2,
    5, 6, 2,
    0, 4, 1,
    4, 5, 1,
    4, 7, 5,
    7, 6, 5,
    0, 1, 2,
    0, 2, 3
  };

  if (boundingBoxVAOID == 0)
  {

    float cubeVertices[] =
    {
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f
    };

    //generate the vao for the cube
    glGenVertexArrays(1, &boundingBoxVAOID);

    //generate the vertex and index buffers for the quad
    glGenBuffers(1, &boundingBoxVBOID);
    glGenBuffers(1, &boundingBoxIBOID);

    //bind the VAO for transferring of data to the GPU
    glBindVertexArray(boundingBoxVAOID);

    //enable position data that will be transferred to the GPU from the quad vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //enable Index buffer object data that will be transferred to the GPU from the quad vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundingBoxIBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(glm::uint), cubeIndices.data(), GL_STATIC_DRAW);
  }

  //bind the index buffer object to be used for drawing the cube
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundingBoxIBOID);  

  //object to world matrix for a bounding box using the bounding boxes center and size
  glm::mat4 ObjectToWorld = glm::translate(glm::mat4(1), mesh->getMesh()->getBounds().getCenter()) * glm::scale(glm::mat4(1), mesh->getMesh()->getBounds().getSize());

  //object to world matrix sent to the GPU called "object_to_world"
  GLint umodel_matrix = glGetUniformLocation(mesh->getShader()->getProgramID(), "object_to_world");
  glUniformMatrix4fv(umodel_matrix, 1, false, &ObjectToWorld[0][0]);

  //normal matrix sent to the GPU called "normal_matrix"
  GLint unormal_matrix = glGetUniformLocation(mesh->getShader()->getProgramID(), "normal_matrix");
  glUniformMatrix4fv(unormal_matrix, 1, false, &(glm::transpose(glm::inverse(ObjectToWorld)))[0][0]);

  //send the variables "perspective_matrix" and "view matrix" onto the GPU
  projectionMatrixID = glGetUniformLocation(mesh->getShader()->getProgramID(), "perspective_matrix");
  viewMatrixID = glGetUniformLocation(mesh->getShader()->getProgramID(), "view_matrix");

  //get the projection and view matrix from the scene set it as a variables for the GPU
  glUniformMatrix4fv(projectionMatrixID, 1, false, &scene.getProjectionMatrix()[0][0]);
  glUniformMatrix4fv(viewMatrixID, 1, false, &scene.getViewMatrix()[0][0]);
 
  glBindVertexArray(boundingBoxVAOID);
  glDrawElements(GL_LINES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
}

void DebugRenderingSystem::drawOBB(MeshComponentPtr mesh, Scene& scene)
{
}

void DebugRenderingSystem::drawSBB(MeshComponentPtr mesh, Scene& scene)
{
}
