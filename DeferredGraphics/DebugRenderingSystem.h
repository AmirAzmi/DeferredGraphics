#pragma once
#include "Scene.h"
#include "AABB.h"
#include "BoundingSphere.h"
#include "BoundingVolumeHierarchy.h"
#include "Octree.h"
#include "BSP.h"
#include "Memory.h"

#include <chrono>

class DebugRenderingSystem
{
  //projection and view matrix ID
  GLuint projectionMatrixID;
  GLuint viewMatrixID;

  //AABB Bounding Box data
  GLuint boundingBoxVAOID = 0;
  GLuint boundingBoxVBOID;
  GLuint boundingBoxIBOID;
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

  //Bone data
  //GLuint skeletonVAOID = 0;
  //GLuint skeletonVBOID = 0;

  //Bounding Sphere data
  GLuint boundingSphereVAOID = 0;
  GLuint boundingSphereVBOID;
  GLuint boundingSphereIBOID;
  GLuint SphereObjectToWorld;

  //Bounding Volume Hiearchy info
  BoundingVolumeHierarchy * BVHTree;
  BoundingVolumeHierarchy * BVHTreeBottomUp;

  //octree info
  Octree* OctreeForAllObjects;
  Octree* OctreePerObject;

  //BSP info
  BSP* BSPTree;

  //shader used for rendering the lines
  ShaderHandle debugDrawID;
  ShaderHandle sphereDebugDrawID;
  ShaderHandle gBufferShaderID;

  public:
  DebugRenderingSystem(Scene & scene, int windowWidth, int windowHeight);
  ~DebugRenderingSystem();
  void Update(Scene& scene, int windowWidth, int windowHeight);

  //check for AABB
  bool isAABBOn = false;

  //check for square AABB
  bool isSquareAABBOn = false;

  //draw AABB of part of an object
  bool isSubObjectDrawOn = false;

  //draw the OBJ Vertices once
  bool isBSOn = false;

  //draw bones
  bool isBonesOn = true;

  //draw BVH
  bool isBVHOn = false;
  int numberOfLevels = 0;
  bool isBVHBottomUpOn = false;
  int numberOfLevelsBUOn = 7;
  int bvh_draw_calls = 0;

  //draw Octree for all objects
  bool isOctreeOnForAllObjects = false;
  int levelForAllObjects = 0;
  bool isOctreeOnForSingularObject = false;
  int levelForOneObject = 0;
  int octree_draw_calls = 0;

  //draw BSP
  bool isBSPOn = false;
  int levelForBSP = 0;

  //memory information
  int memory_usage_from_octree = 0;
  int memory_usage_from_BVHTopeDown = 0;
  int memory_usage_from_BSP = 0;

  //timer
  std::chrono::duration<double, std::milli> rendering_sytem_elapsed_time;

  //sphere type
  BoundingSphere::BoundingSphereCalculationType sphereType = BoundingSphere::BoundingSphereCalculationType::Centroid;

  struct BVHDist 
  {
    BoundingVolumeHierarchy* parent;
    float distance;
  };

  void drawAABB(const MeshComponentPtr mesh,Scene& scene, bool isSquareAABB);
  void drawAABB(const AABB bounds, Scene& scene);
  void drawBS(const MeshComponentPtr mesh, Scene& scene, BoundingSphere::BoundingSphereCalculationType type);
  void drawOctree(Octree * root, float level, Scene & scene);
  void drawLevelOrderAABB(BoundingVolumeHierarchy * root, int level, Scene & scene);
  void printLevelOrderAABB(BoundingVolumeHierarchy * root, Scene & scene);
  void printLevelOrderAABBIter(BoundingVolumeHierarchy * root, Scene & scene);
  void createBVHTree(BoundingVolumeHierarchy * BVH, std::vector<MeshComponentPtr> meshes, int level);
  void createBSPTree(BSP * BSP, std::vector<BSP::Polygon> listOfPolygons, int level);
  void createOctree(Octree * octree, std::vector<MeshComponentPtr> meshes, int level);
  void createOctree(Octree* octree, const glm::vec3* pointsForOneMesh, const size_t size, int level);
  BoundingVolumeHierarchy* createBVHTreeBottomUp(const std::vector<MeshComponentPtr> meshes, int level);
  BVHDist getClosestPair(const std::vector<MeshComponentPtr> meshes);

};

//resources for level order drawing
//https://www.geeksforgeeks.org/print-level-order-traversal-line-line/