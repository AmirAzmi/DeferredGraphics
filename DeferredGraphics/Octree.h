#pragma once
#include <array>
#include "AABB.h"
#include "MeshComponent.h"
class Octree
{
public:
  Octree* parent;                       //refrence to the parent
  std::array<Octree *, 8> children;     //numbe of children for each node of the octree node
  std::vector<MeshComponentPtr> meshes; //each contains its enclosed list of objects
  std::vector<glm::vec3> points;        //specifically for a singular object
  AABB boundingVolume;                  //bouding volume of enclosed meshes
  unsigned int active_children = 0;     //checking which children are "active" aka valid nodes with thier own children

  //takes in a boundingVolume calculate by the meshes total AABB????
  //or a cubic box??? -> we goin for the cubic box
  Octree(AABB boundingBox, std::vector<MeshComponentPtr> meshes);
  Octree(std::vector<glm::vec3> mesh_points);
  Octree* createOctreeNode(AABB bv, std::vector<glm::vec3> points, Octree * parent);
  ~Octree();
};

/*

//should we insert a node into the tree or recalculate the whole tree
answer for this project: calculate the whole tree

What should happen if an object moves outside of the bounding volume of the root node? 
Do we want to resize the entire octree so that all objects are enclosed? 
If we do, we'll have to completely rebuild the octree from scratch. 
If we don't, we'll need to have some way to either handle out of bounds objects or ensure that objects never go out of bounds.

//should we have a predefined space for the octree, like we create a space and say this is the octree in this location
or create some cube of the octree
answer for this project: I am thinking we can use the total AABB space of all objects in the scene as the octree size

How do we want to create the enclosing region for our octree?
Do we want to use a preset dimension, such as a 200x400x200 (X,Y,Z) rectangle? 
Or do we want to use a cubic dimension which is a power of 2? 

//I don't know
What should be the smallest allowable enclosing region which cannot be subdivided?
*/

