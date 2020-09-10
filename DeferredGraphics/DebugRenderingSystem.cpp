#include <algorithm>
#include <list>
#include <queue>
#include "DebugRenderingSystem.h"
#include "MeshComponent.h"
#include "Entity.h"

static int height(BoundingVolumeHierarchy * root)
{
  if (root == nullptr)
  {
    return 0;
  }

  else
  {
    int left_height = height(root->left_child);
    int right_height = height(root->right_child);

    if (left_height > right_height)
    {
      return left_height + 1;
    }
    else
    {
      return right_height + 1;
    }
  }
}

DebugRenderingSystem::DebugRenderingSystem(Scene& scene, int windowWidth, int windowHeight) : projectionMatrixID(-1), viewMatrixID(-1)
{
  debugDrawID = std::make_shared <Shader>("Resources/Shaders/debugDraw.vert", "Resources/Shaders/debugDraw.frag", false);
  sphereDebugDrawID = std::make_shared <Shader>("Resources/Shaders/sphereDebugDraw.vert", "Resources/Shaders/sphereDebugDraw.frag", false);

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
  auto start = std::chrono::high_resolution_clock::now();

  // gets all the current mesh components in th scene
  std::vector<MeshComponentPtr>& meshes = scene.getMeshes();
  memory_usage_from_octree = 0;
  memory_usage_from_BVHTopeDown = 0;
  memory_usage_from_BSP = 0;
  octree_draw_calls = 0;
  bvh_draw_calls = 0;

  //aabb Check
  if (isAABBOn == true)
  {
    debugDrawID->UseShader();
    std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawAABB(mesh, scene, false); });
  }

  if (isSquareAABBOn == true)
  {
    debugDrawID->UseShader();
    std::for_each(meshes.begin(), meshes.end(), [&scene, this](MeshComponentPtr mesh) {drawAABB(mesh, scene, true); });
  }

  if (isSubObjectDrawOn == true)
  {
    //set the shader to use for coloring the octree
    debugDrawID->UseShader();
    debugDrawID->setBool("octree", isSubObjectDrawOn);

    //create octree for the center object, currently hard coded, planned to be for any object
    LinearAllocatorScope scope(linearAllocator);
    OctreePerObject = linearAllocator.TAllocate<Octree>(meshes[0]->getMesh()->getVertices());
    OctreePerObject->boundingVolume = meshes[0]->bounds;
    createOctree(OctreePerObject, OctreePerObject->points.data(), OctreePerObject->points.size(), levelForOneObject);

    //draw octree bv for one object
    drawOctree(OctreePerObject, levelForOneObject, scene);

    linearAllocator.TDeallocate<Octree>(OctreePerObject);
    memory_usage_from_octree = linearAllocator.current_main_memory - linearAllocator.main_memory;
  }

  if (isBSOn == true)
  {
    sphereDebugDrawID->UseShader();
    switch (sphereType)
    {
    case BoundingSphere::BoundingSphereCalculationType::None:
    {
      break;
    }
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

  if (isBVHOn == true)
  {
    debugDrawID->UseShader();

    //create root node Note: REPLACE WITH POOL OR LINEAR ALLOCATOR
    //placement new -> calling a constructor on an already created block of memory
    LinearAllocatorScope scope(linearAllocator);
    BVHTree = linearAllocator.TAllocate<BoundingVolumeHierarchy>(meshes);

    //list of meshes in the root node
    BVHTree->meshes = meshes;

    //set the bounding volume of the current meshes in the root node
    BVHTree->boundingVolume = BVHTree->calculateBoundingVolume(BVHTree->meshes);

    //creates the BVH tree so right now the root nodes
    createBVHTree(BVHTree, BVHTree->meshes, numberOfLevels - 1);

    //draw level order aabb tree Note: this is O(N^2) Drawing method so you can draw each level
    //there is O(N) method using a queue
    //this is also considered breadth first search
    printLevelOrderAABBIter(BVHTree, scene);

    //manually call the destructor because I used placement new to use my own memory
    linearAllocator.TDeallocate<BoundingVolumeHierarchy>(BVHTree);
    memory_usage_from_BVHTopeDown = linearAllocator.current_main_memory - linearAllocator.main_memory;
  }

  if (isBVHBottomUpOn == true)
  {
    debugDrawID->UseShader();

    int level = 7;
    BoundingVolumeHierarchy* root = createBVHTreeBottomUp(meshes, level);

    //storing the children of the left and rgiht trees
    BoundingVolumeHierarchy* left_tree = root->left_child;
    BoundingVolumeHierarchy* right_tree = root->right_child;
    drawAABB(root->boundingVolume, scene);

    while (left_tree != nullptr && right_tree != nullptr && level < numberOfLevels)
    {
      //draw all the left children
      drawAABB(left_tree->boundingVolume, scene);
      left_tree = left_tree->left_child;

      //draw all the right children
      drawAABB(right_tree->boundingVolume, scene);
      right_tree = right_tree->right_child;

      //increment level counter
      level--;

    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  auto end = std::chrono::high_resolution_clock::now();
  rendering_sytem_elapsed_time = end - start;
}

void DebugRenderingSystem::drawAABB(const MeshComponentPtr mesh, Scene& scene, bool isSquareAABB)
{
  AABB bounds;
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

  //regular aabb
  if (isSquareAABBOn == false)
  {
    mesh->bounds = bounds;
  }
  else
  {
    mesh->bounds = bounds.getSquareBounds();
  }

  //get min and max
  glm::vec3 min = mesh->bounds.min;
  glm::vec3 max = mesh->bounds.max;

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

  //bind the buffers the object needs to be used for drawing the cube
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

void DebugRenderingSystem::drawAABB(const AABB bounds, Scene& scene)
{
  //get min and max
  glm::vec3 min = bounds.min;
  glm::vec3 max = bounds.max;

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

  //bind the buffers the object needs to be used for drawing the cube
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

void DebugRenderingSystem::drawBS(const MeshComponentPtr mesh, Scene& scene, BoundingSphere::BoundingSphereCalculationType type)
{
  BoundingSphere sphere;
  /**/

  /**/
  if (boundingSphereVAOID == 0)
  {
    /*std::vector<glm::vec3> sphereVertices = {
      glm::vec3(0.000000, 0.000000, -1.849175),
      glm::vec3(0.172309, 0.000000, -1.834635),
      glm::vec3(0.337484, 0.000000, -1.791151),
      glm::vec3(0.488390, -0.000000, -1.718858),
      glm::vec3(0.000000, -0.172309, -1.834635),
      glm::vec3(-0.000000, -0.337484, -1.791151),
      glm::vec3(-0.000000, -0.488390, -1.718858),
      glm::vec3(-0.172309, 0.000000, -1.834635),
      glm::vec3(-0.337484, 0.000000, -1.791151),
      glm::vec3(-0.488390, 0.000000, -1.718858),
      glm::vec3(0.000000, 0.172309, -1.834635),
      glm::vec3(0.000000, 0.337484, -1.791151),
      glm::vec3(0.000000, 0.488390, -1.718858),
      glm::vec3(0.169271, 0.169271, -1.820312),
      glm::vec3(0.331543, 0.163194, -1.777506),
      glm::vec3(0.479818, 0.157118, -1.706380),
      glm::vec3(0.163195, 0.331543, -1.777506),
      glm::vec3(0.318952, 0.318952, -1.737422),
      glm::vec3(0.459834, 0.304941, -1.671748),
      glm::vec3(0.157118, 0.479818, -1.706380),
      glm::vec3(0.304941, 0.459834, -1.671748),
      glm::vec3(0.434173, 0.434172, -1.617766),
      glm::vec3(0.169271, -0.169271, -1.820312),
      glm::vec3(0.163194, -0.331543, -1.777506),
      glm::vec3(0.157118, -0.479818, -1.706380),
      glm::vec3(0.331543, -0.163194, -1.777506),
      glm::vec3(0.318952, -0.318952, -1.737422),
      glm::vec3(0.304941, -0.459834, -1.671748),
      glm::vec3(0.479818, -0.157118, -1.706380),
      glm::vec3(0.459834, -0.304941, -1.671748),
      glm::vec3(0.434172, -0.434172, -1.617766),
      glm::vec3(-0.169271, -0.169271, -1.820312),
      glm::vec3(-0.331543, -0.163194, -1.777506),
      glm::vec3(-0.479818, -0.157118, -1.706380),
      glm::vec3(-0.163194, -0.331543, -1.777506),
      glm::vec3(-0.318952, -0.318951, -1.737422),
      glm::vec3(-0.459834, -0.304940, -1.671748),
      glm::vec3(-0.157118, -0.479818, -1.706380),
      glm::vec3(-0.304941, -0.459834, -1.671748),
      glm::vec3(-0.434173, -0.434172, -1.617766),
      glm::vec3(-0.169271, 0.169271, -1.820312),
      glm::vec3(-0.163194, 0.331543, -1.777506),
      glm::vec3(-0.157118, 0.479818, -1.706380),
      glm::vec3(-0.331543, 0.163195, -1.777506),
      glm::vec3(-0.318952, 0.318952, -1.737422),
      glm::vec3(-0.304941, 0.459835, -1.671748),
      glm::vec3(-0.479818, 0.157118, -1.706380),
      glm::vec3(-0.459834, 0.304941, -1.671748),
      glm::vec3(-0.434172, 0.434173, -1.617766),
      glm::vec3(-0.000000, -0.000000, -0.150825),
      glm::vec3(-0.000000, 0.172309, -0.165365),
      glm::vec3(-0.000000, 0.337484, -0.208849),
      glm::vec3(0.000000, 0.488390, -0.281142),
      glm::vec3(-0.172309, -0.000000, -0.165365),
      glm::vec3(-0.337484, -0.000000, -0.208849),
      glm::vec3(-0.488390, 0.000000, -0.281142),
      glm::vec3(-0.000000, -0.172309, -0.165365),
      glm::vec3(-0.000000, -0.337484, -0.208849),
      glm::vec3(-0.000000, -0.488390, -0.281142),
      glm::vec3(0.172309, -0.000000, -0.165365),
      glm::vec3(0.337484, -0.000000, -0.208849),
      glm::vec3(0.488390, -0.000000, -0.281142),
      glm::vec3(0.169271, 0.169271, -0.179688),
      glm::vec3(0.163194, 0.331543, -0.222494),
      glm::vec3(0.157118, 0.479818, -0.293620),
      glm::vec3(0.331543, 0.163194, -0.222494),
      glm::vec3(0.318952, 0.318951, -0.262578),
      glm::vec3(0.304941, 0.459834, -0.328252),
      glm::vec3(0.479818, 0.157118, -0.293620),
      glm::vec3(0.459834, 0.304940, -0.328252),
      glm::vec3(0.434173, 0.434172, -0.382234),
      glm::vec3(-0.169271, 0.169271, -0.179688),
      glm::vec3(-0.331543, 0.163194, -0.222494),
      glm::vec3(-0.479818, 0.157118, -0.293620),
      glm::vec3(-0.163194, 0.331543, -0.222494),
      glm::vec3(-0.318952, 0.318952, -0.262578),
      glm::vec3(-0.459834, 0.304941, -0.328252),
      glm::vec3(-0.157118, 0.479818, -0.293620),
      glm::vec3(-0.304941, 0.459834, -0.328252),
      glm::vec3(-0.434172, 0.434172, -0.382234),
      glm::vec3(-0.169271, -0.169271, -0.179688),
      glm::vec3(-0.163195, -0.331543, -0.222494),
      glm::vec3(-0.157118, -0.479818, -0.293620),
      glm::vec3(-0.331543, -0.163194, -0.222494),
      glm::vec3(-0.318952, -0.318952, -0.262578),
      glm::vec3(-0.304941, -0.459834, -0.328252),
      glm::vec3(-0.479818, -0.157118, -0.293620),
      glm::vec3(-0.459835, -0.304941, -0.328252),
      glm::vec3(-0.434173, -0.434172, -0.382234),
      glm::vec3(0.169271, -0.169271, -0.179688),
      glm::vec3(0.331543, -0.163195, -0.222494),
      glm::vec3(0.479818, -0.157118, -0.293620),
      glm::vec3(0.163194, -0.331543, -0.222494),
      glm::vec3(0.318951, -0.318952, -0.262578),
      glm::vec3(0.459834, -0.304941, -0.328252),
      glm::vec3(0.157118, -0.479818, -0.293620),
      glm::vec3(0.304940, -0.459835, -0.328252),
      glm::vec3(0.434172, -0.434173, -0.382234),
      glm::vec3(0.849175, -0.000000, -1.000000),
      glm::vec3(0.834635, 0.172309, -1.000000),
      glm::vec3(0.791151, 0.337483, -1.000000),
      glm::vec3(0.718859, 0.488390, -1.000000),
      glm::vec3(0.834635, -0.000000, -0.827691),
      glm::vec3(0.791151, -0.000000, -0.662516),
      glm::vec3(0.718858, -0.000000, -0.511610),
      glm::vec3(0.834635, -0.172309, -1.000000),
      glm::vec3(0.791151, -0.337484, -1.000000),
      glm::vec3(0.718858, -0.488390, -1.000000),
      glm::vec3(0.834635, -0.000000, -1.172309),
      glm::vec3(0.791151, -0.000000, -1.337484),
      glm::vec3(0.718858, -0.000000, -1.488390),
      glm::vec3(0.820313, 0.169271, -1.169271),
      glm::vec3(0.777507, 0.331543, -1.163194),
      glm::vec3(0.706380, 0.479818, -1.157118),
      glm::vec3(0.777507, 0.163194, -1.331543),
      glm::vec3(0.737422, 0.318951, -1.318952),
      glm::vec3(0.671749, 0.459834, -1.304941),
      glm::vec3(0.706380, 0.157118, -1.479818),
      glm::vec3(0.671749, 0.304941, -1.459834),
      glm::vec3(0.617766, 0.434172, -1.434172),
      glm::vec3(0.820312, 0.169271, -0.830729),
      glm::vec3(0.777506, 0.163194, -0.668457),
      glm::vec3(0.706380, 0.157118, -0.520182),
      glm::vec3(0.777507, 0.331543, -0.836806),
      glm::vec3(0.737422, 0.318951, -0.681048),
      glm::vec3(0.671748, 0.304940, -0.540166),
      glm::vec3(0.706380, 0.479817, -0.842882),
      glm::vec3(0.671749, 0.459834, -0.695059),
      glm::vec3(0.617766, 0.434172, -0.565827),
      glm::vec3(0.820312, -0.169271, -0.830729),
      glm::vec3(0.777506, -0.331543, -0.836806),
      glm::vec3(0.706380, -0.479818, -0.842882),
      glm::vec3(0.777506, -0.163195, -0.668457),
      glm::vec3(0.737422, -0.318952, -0.681048),
      glm::vec3(0.671748, -0.459835, -0.695059),
      glm::vec3(0.706380, -0.157118, -0.520182),
      glm::vec3(0.671748, -0.304941, -0.540166),
      glm::vec3(0.617766, -0.434173, -0.565828),
      glm::vec3(0.820312, -0.169271, -1.169271),
      glm::vec3(0.777506, -0.163195, -1.331543),
      glm::vec3(0.706380, -0.157118, -1.479818),
      glm::vec3(0.777506, -0.331543, -1.163194),
      glm::vec3(0.737422, -0.318952, -1.318952),
      glm::vec3(0.671748, -0.304941, -1.459834),
      glm::vec3(0.706380, -0.479818, -1.157118),
      glm::vec3(0.671748, -0.459835, -1.304941),
      glm::vec3(0.617766, -0.434173, -1.434173),
      glm::vec3(-0.000000, -0.849175, -1.000000),
      glm::vec3(0.172309, -0.834635, -1.000000),
      glm::vec3(0.337483, -0.791151, -1.000000),
      glm::vec3(0.488390, -0.718859, -1.000000),
      glm::vec3(-0.000000, -0.834635, -0.827691),
      glm::vec3(-0.000000, -0.791151, -0.662516),
      glm::vec3(-0.000000, -0.718859, -0.511610),
      glm::vec3(-0.172309, -0.834635, -1.000000),
      glm::vec3(-0.337484, -0.791151, -1.000000),
      glm::vec3(-0.488390, -0.718858, -1.000000),
      glm::vec3(-0.000000, -0.834635, -1.172309),
      glm::vec3(-0.000000, -0.791151, -1.337484),
      glm::vec3(-0.000000, -0.718858, -1.488390),
      glm::vec3(0.169271, -0.820313, -1.169271),
      glm::vec3(0.331543, -0.777507, -1.163194),
      glm::vec3(0.479818, -0.706380, -1.157118),
      glm::vec3(0.163194, -0.777507, -1.331543),
      glm::vec3(0.318951, -0.737422, -1.318952),
      glm::vec3(0.459834, -0.671749, -1.304941),
      glm::vec3(0.157118, -0.706380, -1.479818),
      glm::vec3(0.304941, -0.671748, -1.459834),
      glm::vec3(0.434172, -0.617766, -1.434172),
      glm::vec3(0.169271, -0.820313, -0.830729),
      glm::vec3(0.163194, -0.777507, -0.668457),
      glm::vec3(0.157118, -0.706380, -0.520182),
      glm::vec3(0.331543, -0.777507, -0.836806),
      glm::vec3(0.318951, -0.737422, -0.681048),
      glm::vec3(0.304940, -0.671749, -0.540166),
      glm::vec3(0.479817, -0.706380, -0.842882),
      glm::vec3(0.459834, -0.671749, -0.695059),
      glm::vec3(0.434172, -0.617766, -0.565827),
      glm::vec3(-0.169271, -0.820312, -0.830729),
      glm::vec3(-0.331543, -0.777506, -0.836806),
      glm::vec3(-0.479818, -0.706380, -0.842882),
      glm::vec3(-0.163195, -0.777506, -0.668457),
      glm::vec3(-0.318952, -0.737422, -0.681048),
      glm::vec3(-0.459835, -0.671748, -0.695059),
      glm::vec3(-0.157118, -0.706380, -0.520182),
      glm::vec3(-0.304941, -0.671748, -0.540166),
      glm::vec3(-0.434173, -0.617766, -0.565828),
      glm::vec3(-0.169271, -0.820312, -1.169271),
      glm::vec3(-0.163195, -0.777506, -1.331543),
      glm::vec3(-0.157118, -0.706380, -1.479818),
      glm::vec3(-0.331543, -0.777506, -1.163194),
      glm::vec3(-0.318952, -0.737422, -1.318952),
      glm::vec3(-0.304941, -0.671748, -1.459834),
      glm::vec3(-0.479818, -0.706380, -1.157118),
      glm::vec3(-0.459835, -0.671748, -1.304941),
      glm::vec3(-0.434173, -0.617766, -1.434173),
      glm::vec3(-0.849175, 0.000000, -1.000000),
      glm::vec3(-0.834635, -0.172309, -1.000000),
      glm::vec3(-0.791151, -0.337484, -1.000000),
      glm::vec3(-0.718859, -0.488390, -1.000000),
      glm::vec3(-0.834635, 0.000000, -0.827691),
      glm::vec3(-0.791151, 0.000000, -0.662516),
      glm::vec3(-0.718859, 0.000000, -0.511610),
      glm::vec3(-0.834635, 0.172309, -1.000000),
      glm::vec3(-0.791151, 0.337484, -1.000000),
      glm::vec3(-0.718858, 0.488390, -1.000000),
      glm::vec3(-0.834635, 0.000000, -1.172309),
      glm::vec3(-0.791151, 0.000000, -1.337484),
      glm::vec3(-0.718858, 0.000000, -1.488390),
      glm::vec3(-0.820313, -0.169271, -1.169271),
      glm::vec3(-0.777507, -0.331543, -1.163194),
      glm::vec3(-0.706380, -0.479818, -1.157118),
      glm::vec3(-0.777507, -0.163194, -1.331543),
      glm::vec3(-0.737422, -0.318951, -1.318952),
      glm::vec3(-0.671749, -0.459834, -1.304941),
      glm::vec3(-0.706380, -0.157118, -1.479818),
      glm::vec3(-0.671749, -0.304940, -1.459834),
      glm::vec3(-0.617766, -0.434172, -1.434172),
      glm::vec3(-0.820313, -0.169271, -0.830729),
      glm::vec3(-0.777507, -0.163194, -0.668457),
      glm::vec3(-0.706380, -0.157118, -0.520182),
      glm::vec3(-0.777507, -0.331543, -0.836806),
      glm::vec3(-0.737422, -0.318952, -0.681048),
      glm::vec3(-0.671749, -0.304941, -0.540166),
      glm::vec3(-0.706380, -0.479818, -0.842882),
      glm::vec3(-0.671749, -0.459834, -0.695059),
      glm::vec3(-0.617766, -0.434172, -0.565827),
      glm::vec3(-0.820312, 0.169271, -0.830729),
      glm::vec3(-0.777506, 0.331543, -0.836806),
      glm::vec3(-0.706380, 0.479818, -0.842882),
      glm::vec3(-0.777506, 0.163195, -0.668457),
      glm::vec3(-0.737422, 0.318952, -0.681048),
      glm::vec3(-0.671748, 0.459834, -0.695059),
      glm::vec3(-0.706380, 0.157118, -0.520182),
      glm::vec3(-0.671748, 0.304941, -0.540166),
      glm::vec3(-0.617766, 0.434173, -0.565828),
      glm::vec3(-0.820312, 0.169271, -1.169271),
      glm::vec3(-0.777506, 0.163195, -1.331543),
      glm::vec3(-0.706380, 0.157118, -1.479818),
      glm::vec3(-0.777506, 0.331543, -1.163194),
      glm::vec3(-0.737422, 0.318952, -1.318952),
      glm::vec3(-0.671748, 0.304941, -1.459834),
      glm::vec3(-0.706380, 0.479818, -1.157118),
      glm::vec3(-0.671748, 0.459835, -1.304941),
      glm::vec3(-0.617766, 0.434173, -1.434173),
      glm::vec3(0.000000, 0.849175, -1.000000),
      glm::vec3(0.172309, 0.834635, -1.000000),
      glm::vec3(0.337484, 0.791151, -1.000000),
      glm::vec3(0.488390, 0.718858, -1.000000),
      glm::vec3(0.000000, 0.834635, -1.172309),
      glm::vec3(0.000000, 0.791151, -1.337484),
      glm::vec3(0.000000, 0.718859, -1.488390),
      glm::vec3(-0.172309, 0.834635, -1.000000),
      glm::vec3(-0.337484, 0.791151, -1.000000),
      glm::vec3(-0.488390, 0.718859, -1.000000),
      glm::vec3(0.000000, 0.834635, -0.827691),
      glm::vec3(0.000000, 0.791151, -0.662516),
      glm::vec3(0.000000, 0.718858, -0.511610),
      glm::vec3(0.169271, 0.820312, -0.830729),
      glm::vec3(0.331543, 0.777506, -0.836806),
      glm::vec3(0.479818, 0.706380, -0.842882),
      glm::vec3(0.163195, 0.777506, -0.668457),
      glm::vec3(0.318952, 0.737422, -0.681048),
      glm::vec3(0.459834, 0.671748, -0.695059),
      glm::vec3(0.157118, 0.706380, -0.520182),
      glm::vec3(0.304941, 0.671748, -0.540166),
      glm::vec3(0.434173, 0.617766, -0.565828),
      glm::vec3(0.169271, 0.820312, -1.169271),
      glm::vec3(0.163195, 0.777506, -1.331543),
      glm::vec3(0.157118, 0.706380, -1.479818),
      glm::vec3(0.331543, 0.777506, -1.163194),
      glm::vec3(0.318952, 0.737422, -1.318952),
      glm::vec3(0.304941, 0.671748, -1.459834),
      glm::vec3(0.479818, 0.706380, -1.157118),
      glm::vec3(0.459835, 0.671748, -1.304941),
      glm::vec3(0.434173, 0.617766, -1.434173),
      glm::vec3(-0.169271, 0.820313, -1.169271),
      glm::vec3(-0.331543, 0.777507, -1.163194),
      glm::vec3(-0.479818, 0.706380, -1.157118),
      glm::vec3(-0.163194, 0.777507, -1.331543),
      glm::vec3(-0.318951, 0.737422, -1.318952),
      glm::vec3(-0.459834, 0.671749, -1.304941),
      glm::vec3(-0.157118, 0.706380, -1.479818),
      glm::vec3(-0.304941, 0.671749, -1.459834),
      glm::vec3(-0.434172, 0.617766, -1.434172),
      glm::vec3(-0.169271, 0.820312, -0.830729),
      glm::vec3(-0.163194, 0.777506, -0.668457),
      glm::vec3(-0.157118, 0.706380, -0.520182),
      glm::vec3(-0.331543, 0.777507, -0.836806),
      glm::vec3(-0.318952, 0.737422, -0.681048),
      glm::vec3(-0.304941, 0.671748, -0.540166),
      glm::vec3(-0.479818, 0.706380, -0.842882),
      glm::vec3(-0.459834, 0.671748, -0.695059),
      glm::vec3(-0.434172, 0.617766, -0.565827),
      glm::vec3(0.540039, 0.419922, -1.540039),
      glm::vec3(0.579536, 0.297580, -1.579536),
      glm::vec3(0.607096, 0.154080, -1.607096),
      glm::vec3(0.617893, -0.000000, -1.617893),
      glm::vec3(0.607096, -0.154080, -1.607096),
      glm::vec3(0.579535, -0.297580, -1.579535),
      glm::vec3(0.540039, -0.419922, -1.540039),
      glm::vec3(0.419922, 0.540039, -1.540039),
      glm::vec3(0.297580, 0.579536, -1.579536),
      glm::vec3(0.154080, 0.607096, -1.607096),
      glm::vec3(0.000000, 0.617893, -1.617893),
      glm::vec3(-0.154080, 0.607096, -1.607096),
      glm::vec3(-0.297580, 0.579536, -1.579536),
      glm::vec3(-0.419922, 0.540039, -1.540039),
      glm::vec3(0.540039, 0.540039, -1.419922),
      glm::vec3(0.579536, 0.579535, -1.297580),
      glm::vec3(0.607096, 0.607096, -1.154080),
      glm::vec3(0.617893, 0.617893, -1.000000),
      glm::vec3(0.607096, 0.607096, -0.845920),
      glm::vec3(0.579536, 0.579535, -0.702420),
      glm::vec3(0.540039, 0.540039, -0.580078),
      glm::vec3(0.419922, -0.540039, -1.540039),
      glm::vec3(0.297580, -0.579536, -1.579536),
      glm::vec3(0.154080, -0.607096, -1.607096),
      glm::vec3(-0.000000, -0.617893, -1.617893),
      glm::vec3(-0.154080, -0.607096, -1.607096),
      glm::vec3(-0.297580, -0.579535, -1.579535),
      glm::vec3(-0.419922, -0.540039, -1.540039),
      glm::vec3(0.540039, -0.540039, -1.419922),
      glm::vec3(0.579535, -0.579536, -1.297580),
      glm::vec3(0.607096, -0.607096, -1.154080),
      glm::vec3(0.617893, -0.617893, -1.000000),
      glm::vec3(0.607096, -0.607097, -0.845920),
      glm::vec3(0.579535, -0.579536, -0.702420),
      glm::vec3(0.540039, -0.540039, -0.580078),
      glm::vec3(-0.540039, -0.419922, -1.540039),
      glm::vec3(-0.579536, -0.297580, -1.579536),
      glm::vec3(-0.607096, -0.154080, -1.607096),
      glm::vec3(-0.617893, 0.000000, -1.617893),
      glm::vec3(-0.607096, 0.154080, -1.607096),
      glm::vec3(-0.579535, 0.297580, -1.579535),
      glm::vec3(-0.540039, 0.419922, -1.540039),
      glm::vec3(-0.540039, -0.540039, -1.419922),
      glm::vec3(-0.579536, -0.579535, -1.297580),
      glm::vec3(-0.607096, -0.607096, -1.154080),
      glm::vec3(-0.617893, -0.617893, -1.000000),
      glm::vec3(-0.607097, -0.607096, -0.845920),
      glm::vec3(-0.579536, -0.579535, -0.702420),
      glm::vec3(-0.540039, -0.540039, -0.580078),
      glm::vec3(-0.540039, 0.540039, -1.419922),
      glm::vec3(-0.579535, 0.579536, -1.297580),
      glm::vec3(-0.607096, 0.607096, -1.154080),
      glm::vec3(-0.617893, 0.617893, -1.000000),
      glm::vec3(-0.607096, 0.607096, -0.845920),
      glm::vec3(-0.579535, 0.579536, -0.702420),
      glm::vec3(-0.540039, 0.540039, -0.580078),
      glm::vec3(0.540039, 0.419922, -0.459961),
      glm::vec3(0.579536, 0.297580, -0.420464),
      glm::vec3(0.607096, 0.154080, -0.392904),
      glm::vec3(0.617893, -0.000000, -0.382107),
      glm::vec3(0.607096, -0.154080, -0.392904),
      glm::vec3(0.579535, -0.297581, -0.420464),
      glm::vec3(0.540039, -0.419922, -0.459961),
      glm::vec3(0.419922, 0.540039, -0.459961),
      glm::vec3(0.297580, 0.579535, -0.420464),
      glm::vec3(0.154080, 0.607096, -0.392904),
      glm::vec3(0.000000, 0.617893, -0.382107),
      glm::vec3(-0.154080, 0.607096, -0.392904),
      glm::vec3(-0.297580, 0.579536, -0.420464),
      glm::vec3(-0.419922, 0.540039, -0.459961),
      glm::vec3(0.419922, -0.540039, -0.459961),
      glm::vec3(0.297580, -0.579536, -0.420464),
      glm::vec3(0.154080, -0.607097, -0.392904),
      glm::vec3(-0.000000, -0.617893, -0.382107),
      glm::vec3(-0.154080, -0.607096, -0.392904),
      glm::vec3(-0.297581, -0.579536, -0.420464),
      glm::vec3(-0.419922, -0.540039, -0.459961),
      glm::vec3(-0.540039, -0.419922, -0.459961),
      glm::vec3(-0.579536, -0.297580, -0.420464),
      glm::vec3(-0.607096, -0.154080, -0.392904),
      glm::vec3(-0.617893, 0.000000, -0.382107),
      glm::vec3(-0.607096, 0.154080, -0.392904),
      glm::vec3(-0.579536, 0.297580, -0.420464),
      glm::vec3(-0.540039, 0.419922, -0.459961),
      glm::vec3(0.501543, 0.501543, -1.501543),
      glm::vec3(0.501543, -0.501543, -1.501543),
      glm::vec3(-0.501543, -0.501543, -1.501543),
      glm::vec3(-0.501543, 0.501543, -1.501543),
      glm::vec3(0.501543, 0.501543, -0.498457),
      glm::vec3(0.501543, -0.501543, -0.498457),
      glm::vec3(-0.501543, -0.501543, -0.498457),
      glm::vec3(-0.501543, 0.501543, -0.498457)
    };
    */
    /*glm::vec3 max(-INFINITY);//min point
    glm::vec3 min(std::numeric_limits<float>::max());//max point
    glm::vec3 averagePosition = glm::vec3(0.0f, 0.0f, 0.0f);

    for (auto& sphereVetex : sphereVertices)
    {
      averagePosition += sphereVetex;

      if (max.x < sphereVetex.x) { max.x = sphereVetex.x; }
      if (max.y < sphereVetex.y) { max.y = sphereVetex.y; }
      if (max.z < sphereVetex.z) { max.z = sphereVetex.z; }

      if (min.x > sphereVetex.x) { min.x = sphereVetex.x; }
      if (min.y > sphereVetex.y) { min.y = sphereVetex.y; }
      if (min.z > sphereVetex.z) { min.z = sphereVetex.z; }
    }

    averagePosition /= sphereVertices.size(); //center object based off of position

    //grab the bounds
    float rangex = max.x - min.x;
    float rangey = max.y - min.y;
    float rangez = max.z - min.z;

    //get the range max
    float rangemax(std::max(std::max(rangex, rangey), rangez));

    //for all the vertices set the offset so it is centered and divide by the range max
    for (auto& vert : sphereVertices)
    {
      vert -= averagePosition;
      vert /= rangemax;
    }    */

    //generate the vao for the cube
    /*glGenVertexArrays(1, &boundingSphereVAOID);

    //generate the vertex and index buffers for the quad
    glGenBuffers(1, &boundingSphereVBOID);
    glGenBuffers(1, &boundingSphereIBOID);

    //bind the VAO for transferring of data to the GPU
    glBindVertexArray(boundingSphereVAOID);

    //enable position data that will be transferred to the GPU from the quad vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, boundingSphereVBOID);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), sphereVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //enable Index buffer object data that will be transferred to the GPU from the quad vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundingSphereIBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(glm::uint), sphereIndices.data(), GL_STATIC_DRAW);*/
  }
  
  /*sphere.info = sphere.calculateBS(type, mesh->getMesh()->getVertices());

  glm::mat4 ObjectToWorld =
    glm::translate(sphere.info.center + mesh->getEntityPtr()->position) *
    glm::scale(glm::vec3(sphere.info.radius) *
      mesh->getEntityPtr()->scale * 1.0f);

  //object to world matrix for a bounding box using the bounding boxes center and size
  SphereObjectToWorld = glGetUniformLocation(sphereDebugDrawID->getProgramID(), "SphereObjectToWorld");
  glUniformMatrix4fv(SphereObjectToWorld, 1, false, &ObjectToWorld[0][0]);

  //bind the buffers the object needs to be used for drawing the cube
  glBindBuffer(GL_ARRAY_BUFFER, boundingSphereVBOID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundingSphereIBOID);

  //send the variables "perspective_matrix" and "view matrix" onto the GPU
  projectionMatrixID = glGetUniformLocation(sphereDebugDrawID->getProgramID(), "perspective_matrix");
  viewMatrixID = glGetUniformLocation(sphereDebugDrawID->getProgramID(), "view_matrix");

  //get the projection and view matrix from the scene set it as a variables for the GPU
  glUniformMatrix4fv(projectionMatrixID, 1, false, &scene.getProjectionMatrix()[0][0]);
  glUniformMatrix4fv(viewMatrixID, 1, false, &scene.getViewMatrix()[0][0]);

  glLineWidth(2);
  glBindVertexArray(boundingSphereVAOID);
  glDrawElements(GL_LINES, sphereIndices.size(), GL_UNSIGNED_INT, 0);*/
}

void DebugRenderingSystem::drawOctree(Octree* child, int level, Scene& scene)
{

  if (level < 0)
  {
    return;
  }

  debugDrawID->setInt("level", level);
  drawAABB(child->boundingVolume, scene);
  octree_draw_calls++;

  for (int i = 0; i < child->children.size(); ++i)
  {
    if (child->children[i] != nullptr)
    {
      drawOctree(child->children[i], level - 1, scene);
    }
  }
}

void DebugRenderingSystem::drawLevelOrderAABB(BoundingVolumeHierarchy* root, int level, Scene& scene)
{
  if (root == nullptr)
  {
    return;
  }

  if (level == 1)
  {
    drawAABB(root->boundingVolume, scene);
    bvh_draw_calls++;
  }
  else if (level > 1)
  {
    drawLevelOrderAABB(root->left_child, level - 1, scene);
    drawLevelOrderAABB(root->right_child, level - 1, scene);
  }
}

void DebugRenderingSystem::printLevelOrderAABB(BoundingVolumeHierarchy* root, Scene& scene)
{
  int h = height(root);

  for (int i = 1; i <= h; ++i)
  {
    drawLevelOrderAABB(root, i, scene);
  }
}

void DebugRenderingSystem::printLevelOrderAABBIter(BoundingVolumeHierarchy* root, Scene& scene)
{
  std::queue<BoundingVolumeHierarchy*> tree_nodes; //create temp queue

  tree_nodes.push(root); //push the root onto the queue

  while (!tree_nodes.empty()) //if queue sint empty
  {
    BoundingVolumeHierarchy* top_node = tree_nodes.front(); //get the first element in the queue

    drawAABB(top_node->boundingVolume, scene); //print it aka draw
    bvh_draw_calls++;

    tree_nodes.pop(); //pop the first elemeney from the queue

    /************************************************************/
    /************************************************************/
    //Note: If you want to print inorder from left to right, you need
    //      to push the left node first, if you want to print right to left
    //      you need to push the right node first
    /************************************************************/
    /************************************************************/

    if (top_node->left_child != nullptr)
    {
      tree_nodes.push(top_node->left_child); //discover left
    }

    if (top_node->right_child != nullptr)
    {
      tree_nodes.push(top_node->right_child); //discover right
    }
  }
}

void DebugRenderingSystem::createBVHTree(BoundingVolumeHierarchy* BVH, std::vector<MeshComponentPtr> meshes, int level)
{
  //base case
  if (level < 0 || meshes.size() <= 2)
  {
    return;
  }
  else
  {
    //partition the meshes equally along the longest axis by midpoint and store those meshes into the left and right children
    //get longest axis from bounding box
    glm::vec3 longestAxis(BVH->boundingVolume.max - BVH->boundingVolume.min);
    float longestAxisValue = std::max(std::max(longestAxis.x, longestAxis.y), longestAxis.z);

    //sort the meshes by the longest axis
    if (longestAxisValue == longestAxis.x)
    {
      std::sort(meshes.begin(), meshes.end(), [](MeshComponentPtr a, MeshComponentPtr b) {return a->getEntityPtr()->position.x < b->getEntityPtr()->position.x; });
    }
    else if (longestAxisValue == longestAxis.y)
    {
      std::sort(meshes.begin(), meshes.end(), [](MeshComponentPtr a, MeshComponentPtr b) {return a->getEntityPtr()->position.y < b->getEntityPtr()->position.y; });
    }
    else if (longestAxisValue == longestAxis.z)
    {
      std::sort(meshes.begin(), meshes.end(), [](MeshComponentPtr a, MeshComponentPtr b) {return a->getEntityPtr()->position.z < b->getEntityPtr()->position.z; });
    }

    //left and right meshes list
    std::vector<MeshComponentPtr> left;
    std::vector<MeshComponentPtr> right;

    //store left side meshes
    for (int i = 0; i < meshes.size() / 2; ++i)
    {
      left.push_back(meshes[i]);
    }

    //store right side meshes
    for (size_t i = meshes.size() / 2; i < meshes.size(); ++i)
    {
      right.push_back(meshes[i]);
    }

    BVH->left_child = BVH->left_child->createNode(left);
    createBVHTree(BVH->left_child, left, level - 1);
    BVH->right_child = BVH->right_child->createNode(right);
    createBVHTree(BVH->right_child, right, level - 1);
  }
}

void DebugRenderingSystem::createBSPTree(BSP* BSP, std::vector<BSP::Polygon> listOfPolygons, int level)
{
  if (level < 0)
  {
    return;
  }
  else
  {

    //create split plane

    //create front and back list
    std::vector<BSP::Polygon> FrontList;
    std::vector<BSP::Polygon> BackList;

    //classify polygons within the plane to see if they are in the front or back list
  }
}

void DebugRenderingSystem::createOctree(Octree* octree, std::vector<MeshComponentPtr> meshes, int level)
{
  //base case
  if (level < 0)
  {
    return;
  }
  else
  {
    //divide space into 8 children

  }
}

void DebugRenderingSystem::createOctree(Octree* octree, const glm::vec3* pointsForOneMesh, const size_t size, int level)
{
  //base case
  if (level < 0 || size < 2)
  {
    return;
  }
  else
  {
    std::array<AABB, 8> boundingBoxes;

    //bottom back left
    //generate the bounding volume
    glm::vec3 half_vector = octree->boundingVolume.getSize() / 2.0f;
    glm::vec3 center = octree->boundingVolume.min + half_vector;
    boundingBoxes[0].min = octree->boundingVolume.min;
    boundingBoxes[0].max = center;
    AABB boundingVolume0 = boundingBoxes[0];
    std::vector<glm::vec3> points0 = AABB::isContained(pointsForOneMesh, size, boundingVolume0);
    if (points0.size() < 2)
    {
      octree->children[0] = nullptr;
    }
    else
    {
      //set th active child
     // octree->active_children |= (1 << 0);

      //create the node for the octree
      octree->children[0] = octree->children[0]->createOctreeNode(boundingVolume0, points0/*, octree*/);
    }

    //bottom back right
    boundingBoxes[1].min = glm::vec3(center.x, octree->boundingVolume.min.y, octree->boundingVolume.min.z);
    boundingBoxes[1].max = glm::vec3(octree->boundingVolume.max.x, center.y, center.z);
    AABB boundingVolume1 = boundingBoxes[1];
    std::vector<glm::vec3> points1 = AABB::isContained(pointsForOneMesh, size, boundingVolume1);
    if (points1.size() < 2)
    {
      octree->children[1] = nullptr;
    }
    else
    {
     // octree->active_children |= (1 << 1);
      //create the node for the first child
      octree->children[1] = octree->children[1]->createOctreeNode(boundingVolume1, points1/*, octree*/);
    }

    //bottom front right
    boundingBoxes[2].min = glm::vec3(center.x, octree->boundingVolume.min.y, center.z);
    boundingBoxes[2].max = glm::vec3(octree->boundingVolume.max.x, center.y, octree->boundingVolume.max.z);
    AABB boundingVolume2 = boundingBoxes[2];
    std::vector<glm::vec3> points2 = AABB::isContained(pointsForOneMesh, size, boundingVolume2);
    if (points2.size() < 2)
    {
      octree->children[2] = nullptr;
    }
    else
    {
     // octree->active_children |= (1 << 2);
      //create the node for the second child
      octree->children[2] = octree->children[2]->createOctreeNode(boundingVolume2, points2/*, octree*/);
    }

    //bottom front left
    boundingBoxes[3].min = glm::vec3(octree->boundingVolume.min.x, octree->boundingVolume.min.y, center.z);
    boundingBoxes[3].max = glm::vec3(center.x, center.y, octree->boundingVolume.max.z);
    AABB boundingVolume3 = boundingBoxes[3];
    std::vector<glm::vec3> points3 = AABB::isContained(pointsForOneMesh, size, boundingVolume3);
    if (points3.size() < 2)
    {
      octree->children[3] = nullptr;
    }
    else
    {
     // octree->active_children |= (1 << 3);
      //create the node for the 3rd child
      octree->children[3] = octree->children[3]->createOctreeNode(boundingVolume3, points3/*, octree*/);
    }

    //top back left
    boundingBoxes[4].min = glm::vec3(octree->boundingVolume.min.x, center.y, octree->boundingVolume.min.z);
    boundingBoxes[4].max = glm::vec3(center.x, octree->boundingVolume.max.y, center.z);
    AABB boundingVolume4 = boundingBoxes[4];
    std::vector<glm::vec3> points4 = AABB::isContained(pointsForOneMesh, size, boundingVolume4);
    if (points4.size() < 2)
    {
      octree->children[4] = nullptr;
    }
    else
    {
     // octree->active_children |= (1 << 4);
      //create the node for the 4th child
      octree->children[4] = octree->children[4]->createOctreeNode(boundingVolume4, points4/*, octree*/);
    }

    //top back right
    boundingBoxes[5].min = glm::vec3(center.x, center.y, octree->boundingVolume.min.z);
    boundingBoxes[5].max = glm::vec3(octree->boundingVolume.max.x, octree->boundingVolume.max.y, center.z);
    AABB boundingVolume5 = boundingBoxes[5];
    std::vector<glm::vec3> points5 = AABB::isContained(pointsForOneMesh, size, boundingVolume5);
    if (points5.size() < 2)
    {
      octree->children[5] = nullptr;
    }
    else
    {
    //  octree->active_children |= (1 << 5);
      //create the node for the 5th child
      octree->children[5] = octree->children[5]->createOctreeNode(boundingVolume5, points5/*, octree*/);
    }

    //top front right
    boundingBoxes[6].min = center;
    boundingBoxes[6].max = octree->boundingVolume.max;
    AABB boundingVolume6 = boundingBoxes[6];
    std::vector<glm::vec3> points6 = AABB::isContained(pointsForOneMesh, size, boundingVolume6);
    if (points6.size() < 2)
    {
      octree->children[6] = nullptr;
    }
    else
    {
     // octree->active_children |= (1 << 6);
      //create the node for the 6th child
      octree->children[6] = octree->children[6]->createOctreeNode(boundingVolume6, points6/*, octree*/);
    }

    //top front left
    boundingBoxes[7].min = glm::vec3(octree->boundingVolume.min.x, center.y, center.z);
    boundingBoxes[7].max = glm::vec3(center.x, octree->boundingVolume.max.y, octree->boundingVolume.max.z);
    AABB boundingVolume7 = boundingBoxes[7];
    std::vector<glm::vec3> points7 = AABB::isContained(pointsForOneMesh, size, boundingVolume7);
    if (points7.size() < 2)
    {
      octree->children[7] = nullptr;
    }
    else
    {
     // octree->active_children |= (1 << 7);
      //create the node for the last child
      octree->children[7] = octree->children[7]->createOctreeNode(boundingVolume7, points7/*, octree*/);
    }

    //create the octree of that child else dont draw the octree
    for (int i = 0; i < octree->children.size(); ++i)
    {
      if (octree->children[i] != nullptr)
      {
        createOctree(octree->children[i], octree->children[i]->points.data(), octree->children[i]->points.size(), level - 1);
      }
    }
  }

}

BoundingVolumeHierarchy* DebugRenderingSystem::createBVHTreeBottomUp(std::vector<MeshComponentPtr> meshes, int level)
{

  BVHDist closest_pair;

  closest_pair = getClosestPair(meshes);

  return closest_pair.parent;
  //for each level
    //find the closest neighbor pairs
    //create a parent from the neighbor pair
    //remove neighbor from mesh list

  //return root node
}

DebugRenderingSystem::BVHDist DebugRenderingSystem::getClosestPair(std::vector<MeshComponentPtr> meshes)
{
  float const epsilon = 0.00016f;
  float min_distance_from_pair_j_k = INFINITY;
  std::vector<MeshComponentPtr> mesh_j; //left
  std::vector<MeshComponentPtr> mesh_k; //right

  //get closest pair
  for (int j = 0; j < meshes.size(); ++j)
  {
    for (int k = 1; k < meshes.size(); ++k)
    {
      //dsitance from object 0 to k objects
      //can do distance squared but you know lets get it to work first
      float distance = glm::distance(meshes[j]->entity->position, meshes[k]->entity->position);

      if (j != k)
      {
        //allow for multiple pairs
        if (min_distance_from_pair_j_k > distance + epsilon)
        {
          min_distance_from_pair_j_k = distance;
          mesh_j.clear();
          mesh_k.clear();
          mesh_j.push_back(meshes[j]);
          mesh_k.push_back(meshes[k]);
        }
      }
    }
  }

  std::vector<MeshComponentPtr> meshes_for_parent;
  meshes_for_parent.push_back(mesh_j[0]);
  meshes_for_parent.push_back(mesh_k[0]);

  //create parent from closest negihbor
  BoundingVolumeHierarchy* sub_parent = new BoundingVolumeHierarchy(meshes_for_parent);
  sub_parent->left_child = sub_parent->left_child->createNode(mesh_j);
  sub_parent->right_child = sub_parent->right_child->createNode(mesh_k);
  sub_parent->boundingVolume = sub_parent->calculateBoundingVolume(meshes_for_parent);

  return BVHDist{ sub_parent, min_distance_from_pair_j_k };
}



