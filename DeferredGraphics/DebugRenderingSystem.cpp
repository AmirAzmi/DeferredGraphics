#include <algorithm>
#include "DebugRenderingSystem.h"
#include "MeshComponent.h"
#include "Entity.h"


DebugRenderingSystem::DebugRenderingSystem(Scene& scene, int windowWidth, int windowHeight) : projectionMatrixID(-1), viewMatrixID(-1)
{
  debugDrawID = std::make_shared <Shader>("debugDraw.vert", "debugDraw.frag", false);

  // gets all the current mesh components in th scene
  std::vector<MeshComponentPtr>& meshes = scene.getMeshes();

  for (auto mesh : meshes)
  {
    mesh->vertices = mesh->getVec4Vertices();
  }
}

DebugRenderingSystem::~DebugRenderingSystem()
{

}

void DebugRenderingSystem::Update(Scene& scene, int windowWidth, int windowHeight)
{
  //set the active shader
  debugDrawID->UseShader();

  // gets all the current mesh components in th scene
  std::vector<MeshComponentPtr>& meshes = scene.getMeshes();

  //for each mesh component
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  //aabb Check
  if (isAABBOn == true)
  {
    std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawAABB(mesh, scene); });
  }

  if (isBSOn == true)
  {
    switch (sphereType)
    {
    case BoundingSphere::BoundingSphereCalculationType::Centroid:
    {
      std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawBS(mesh, scene, BoundingSphere::BoundingSphereCalculationType::Centroid); });
      break;
    }

    case BoundingSphere::BoundingSphereCalculationType::Ritters:
    {

      std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawBS(mesh, scene, BoundingSphere::BoundingSphereCalculationType::Ritters); });
      break;
    }

    case BoundingSphere::BoundingSphereCalculationType::ModifiedLarsons:
    {

      std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawBS(mesh, scene, BoundingSphere::BoundingSphereCalculationType::ModifiedLarsons); });
      break;
    }

    case BoundingSphere::BoundingSphereCalculationType::PCA:
    {

      std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawBS(mesh, scene, BoundingSphere::BoundingSphereCalculationType::PCA); });
      break;
    }

    default:
    {
      //do not draw if this somehow happens
      break;
    }
    }
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DebugRenderingSystem::drawAABB(MeshComponentPtr mesh, Scene& scene)
{
  AABB bounds = mesh->getMeshBounds();
  bounds.Empty();

  std::vector<glm::vec4> ObjectToWorldVertices;

  //object to world matrix for a bounding box using the bounding boxes center and size
  glm::mat4 ObjectToWorld = glm::translate(mesh->getEntityPtr()->position) * glm::rotate(mesh->getEntityPtr()->angle, mesh->getEntityPtr()->axisOfRotation) * glm::scale(mesh->getEntityPtr()->scale);

  //get the world vertices and create an AABB on the go
  for (int i = 0; i < mesh->vertices.size(); ++i)
  {
    //get vec4 vertices from the mesh component and multiply them by object to woerk to get object to world verices
    ObjectToWorldVertices.push_back(ObjectToWorld * mesh->vertices[i]);
    bounds.Add(glm::vec3(ObjectToWorldVertices[i].x, ObjectToWorldVertices[i].y, ObjectToWorldVertices[i].z));
  }

  mesh->bounds = bounds;

  //get min and max
  glm::vec3 min = bounds.min;
  glm::vec3 max = bounds.max;

  //cube indices
  std::vector<glm::uint> cubeIndices =
  {
    0,1,
    1,2,
    2,3,
    3,0,
    0,4,
    4,5,
    5,6,
    6,7,
    3,7,
    7,4,
    1,5,
    2,6
  };

  float cubeVertices[] =
  {
     max.x, max.y, min.z, //back top right
     min.x, max.y, min.z, //back top left
     min.x, min.y, min.z, //back bottom left
     max.x, min.y, min.z, //back bottom right
     max.x, max.y, max.z, //front top right
     min.x, max.y, max.z, //front top left
     min.x, min.y, max.z, //front bottom left
     max.x, min.y, max.z  //front bottom right
  };

  if (boundingBoxVAOID == 0)
  {
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
  glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBOID);

  //to update the buffer when it is live
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVertices), &cubeVertices);

  //send the variables "perspective_matrix" and "view matrix" onto the GPU
  projectionMatrixID = glGetUniformLocation(debugDrawID->getProgramID(), "perspective_matrix");
  viewMatrixID = glGetUniformLocation(debugDrawID->getProgramID(), "view_matrix");

  //get the projection and view matrix from the scene set it as a variables for the GPU
  glUniformMatrix4fv(projectionMatrixID, 1, false, &scene.getProjectionMatrix()[0][0]);
  glUniformMatrix4fv(viewMatrixID, 1, false, &scene.getViewMatrix()[0][0]);

  glLineWidth(2);
  glBindVertexArray(boundingBoxVAOID);
  glDrawElements(GL_LINES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
}

void DebugRenderingSystem::drawBS(MeshComponentPtr mesh, Scene& scene, BoundingSphere::BoundingSphereCalculationType type)
{
  //calculate the radius, sectors, and stacks of the sphere
  BoundingSphere boundingSphere;
  boundingSphere.info = boundingSphere.calculateBS(type, mesh->getMesh()->getVertices());

  std::vector<glm::vec4> ObjectToWorldVertices;

  //object to world matrix for a bounding box using the bounding boxes center and size
  glm::mat4 ObjectToWorld = 
    glm::translate(mesh->getEntityPtr()->position) * 
    glm::rotate(mesh->getEntityPtr()->angle, mesh->getEntityPtr()->axisOfRotation) * 
    glm::scale(mesh->getEntityPtr()->scale);

   
  if (boundingSphereVAOID == 0)
  {

  }
}

