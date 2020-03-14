#include "BoundingSphere.h"

struct extremalPoints
{
  glm::vec3 min;
  glm::vec3 max;
};

extremalPoints FindExtremalPoints(std::vector<glm::vec3> points, std::vector<glm::vec3> norms);
float distanceSquaredVec3(glm::vec3 a, glm::vec3 b);
float distanceSquaredFloat(float a, float b);
bool isVec3LessThan(glm::vec3& a, glm::vec3& b);

//mode is the type of calculation we are gonn do on the set of vertices passed in
BoundingSphere::BSInfo BoundingSphere::calculateBS(BoundingSphereCalculationType mode, std::vector<glm::vec3> vertices)
{
  //doing this because I current dont know, I will change it to mode once I know for sure it works
  BoundingSphereCalculationType myType = mode;
  float maxmimum_distance_radius = -INFINITY;
  int size = vertices.size();

  switch (myType)
  {
  case BoundingSphereCalculationType::Centroid:
  {
    //calculate the center
    glm::vec3 center = glm::vec3(0.0f);
    for (auto& vertex : vertices)
    {
      center += vertex;
    }

    //calcualte center point of object
    center = glm::vec3(center.x / size, center.y / size, center.z / size);

    //calculate radius
    for (auto& vertex : vertices)
    {
      float dist = glm::distance(vertex, center); //calcualte distance between each point

      if (dist > maxmimum_distance_radius)
      {
        maxmimum_distance_radius = dist;
      }
    }

    //set the center
    info.center = center;

    //set the radius (note: I only do one sqrt at the very end instead of doing the sqrt in the loop so it isnt to bad here)
    info.radius = maxmimum_distance_radius;

    //return the calculated center and radius
    return info;
  }

  case BoundingSphereCalculationType::Ritters:
  {
    //https://en.wikipedia.org/wiki/Bounding_sphere
    glm::vec3 x_point = vertices[0];

    //weird intializers so this might be a bug
    glm::vec3 y_point;
    glm::vec3 z_point;
    glm::vec3 center_point_between_yz;

    //calculate furthest y point from x point
    for (auto& vertex : vertices)
    {
      //calcualte distances quared between the y value
      float dist = distanceSquaredFloat(vertex.y, x_point.y);

      if (dist > maxmimum_distance_radius)
      {
        y_point = vertex;
      }
    }

    //reset the maximum distance radius for the z point comparison
    maxmimum_distance_radius = -INFINITY;

    //calculate furthest z point from y point
    for (auto& vertex : vertices)
    {
      //calcualte distances quared between the y value
      float dist = distanceSquaredFloat(vertex.z, y_point.z);

      if (dist > maxmimum_distance_radius)
      {
        z_point = vertex;
      }
    }

    //calculate the midpoint from y and z point which is the center
    //this sets up initial ball B with center and radius
    glm::vec3 center = (y_point + z_point) / 2.0f;
    float radiusSqaured = distanceSquaredVec3(y_point, z_point);
    float radius = sqrtf(radiusSqaured) / 2.0f;

    //check if the all the points P are within B
    for (auto& vertex : vertices)
    {
      float dist = distanceSquaredVec3(vertex, center);

      //if the distance between the center and the vertex is less than the radius calculated from the intial ball B
      if (dist > radius)
      {
        radius = dist;
      }

    }

    info.center = center;
    info.radius = radius;
    return info;
  }

  case BoundingSphereCalculationType::ModifiedLarsons:
  {
    /*
    https://en.wikipedia.org/wiki/Bounding_sphere
    Larsson (2008) proposed the "extremal points optimal sphere" method with controllable speed to accuracy approximation to solve the bounding sphere problem.
    This method works by taking a set of s direction vectors and projecting all points onto each vector in s serves as a speed-accuracy trade-off variable.
    An exact solver is applied to the 2s extremal points of these projections. The algorithm then iterates over the remaining points, if any, growing the sphere if necessary.
    For large n this method is orders of magnitude faster than exact methods, while giving comparable results. It has a worst case time of
    O(sn). [1]
    */

    int number_of_points = vertices.size();
    std::vector<glm::vec3> norms =
    {
     glm::vec3(2, 2, 1),
     glm::vec3(1, 2, 2),
     glm::vec3(2, 1, 2),
     glm::vec3(2, -2, 1),
     glm::vec3(2, 2, -1),
     glm::vec3(2, -2, -1),
     glm::vec3(1, -2, 2),
     glm::vec3(1, 2, -2),
     glm::vec3(1, -2, -2),
     glm::vec3(2, -1, 2),
     glm::vec3(2, 1, -2),
     glm::vec3(2, -1, -2) };

    std::vector<glm::vec3> projected_points_onto_vector;

    extremalPoints points; //extraml points struct cointing the min and ma x points
    int s = 3; //it can be 2, 3, 4

    if (number_of_points < 2 * s)
    {
      //get extremal points
      points = FindExtremalPoints(vertices, norms);

      //create minimum sphere with extremal points
      glm::vec3 center = (points.max + points.min) / 2.0f;
      float radius = glm::distance(points.max, center);

      //grow sphere, do i need to change center depending on if the point isnt in the sphere
      for (int i = 0; i < number_of_points; ++i)
      {
        float dist = glm::distance(center, vertices[i]);
        if (dist > radius)
        {
          radius = dist;
        }
      }

      info.center = center;
      info.radius = radius;

    }
    else
    {
      //create minimum sphere with extremal points
      glm::vec3 center = (points.max + points.min) / 2.0f;
      float radius = glm::distance(points.max, center);

      info.center = center;
      info.radius = radius;
    }

    return info;
  }

  case BoundingSphereCalculationType::PCA:
  {

    return info;
  }

  default:
  {

    return info;
  }
  }
}

float distanceSquaredVec3(glm::vec3 a, glm::vec3 b)
{
  float distance = (b.x - a.x) * (b.x - a.x) +
    (b.y - a.y) * (b.y - a.y) +
    (b.z - a.z) * (b.z - a.z);

  return distance;
}

float distanceSquaredFloat(float a, float b)
{
  float distance = (b - a) * (b - a);

  return distance;
}

extremalPoints FindExtremalPoints(std::vector<glm::vec3> points, std::vector<glm::vec3> norms)
{
  extremalPoints current_points = { glm::vec3(0), glm::vec3(0) };

  //for all normals
  for (int j = 0; j < norms.size(); ++j)
  {
    extremalPoints temp_points;
    bool once = false;

    //for all points
    for (int i = 0; i < points.size(); ++i)
    {
      //project vector onto another vector in this case: project point onto the normal
      float dot_scalar = glm::dot(points[i], norms[j]);
      //float length = glm::length(norms[j]); //-->
      float correct_length =
        sqrtf(norms[j].x * norms[j].x *
          norms[j].y * norms[j].y *
          norms[j].z * norms[j].z);

      //calculated projected point onto the normal
      glm::vec3 current_extreme_point = (norms[j] * dot_scalar) / correct_length;

      //initally set the temp min and max
      if (once == false)
      {
        temp_points.min = temp_points.max = current_extreme_point;
        once = true;
      }

      //check for next points to set the min and max extrema if current extreme point is less than the temp points
      if (isVec3LessThan(current_extreme_point, temp_points.min) == true)
      {
        temp_points.min = current_extreme_point;
      }
      else
      {
        temp_points.max = current_extreme_point;
      }
    }

    //after getting the min and max extreme points calculate the distance between them and the current extreme points
    //if they are bigger then set the current points to temp points
    if (glm::distance(temp_points.min, temp_points.max) > glm::distance(current_points.min, current_points.max))
    {
      current_points = temp_points;
    }
  }

  return current_points;
}

bool isVec3LessThan(glm::vec3& a, glm::vec3& b)
{
  if (a.x <= b.x && a.y <= b.y && a.z <= b.z)
  {
    return true;
  }

  return false;
}



