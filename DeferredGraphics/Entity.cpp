#include "Entity.h"

Entity::Entity(std::string name, Scene& scene):name(name),  scene(scene), axisOfRotation(0,0,1)
{
}
