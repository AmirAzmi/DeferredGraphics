#pragma once
#include <glm/glm/glm.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>

class Quaternion
{
public:
  float x, y, z, w;

  Quaternion()
  {

  }

  Quaternion(float w, float x, float y, float z) :w(w), x(x), y(y), z(z)
  {
  }

  Quaternion(const glm::vec3& axis, float angle)
  {
    w = cosf(0.5f * angle);

    float thetaOver2 = sinf(0.5f * angle);
    x = axis.x * thetaOver2;
    y = axis.y * thetaOver2;
    z = axis.z * thetaOver2;
  }

  Quaternion operator+(const Quaternion& b) const;
  Quaternion operator-(const Quaternion& b) const;
  Quaternion operator*(const Quaternion& b) const; //quaternion cross product
  Quaternion operator*(float scalar) const;
  Quaternion operator/(float scalar);

  //operations
  Quaternion& normalized();
  Quaternion& conjugate();
  Quaternion& inverse();
  Quaternion& projection(const Quaternion& b);
  glm::mat4 quaternionToMatrix();
  float magnitudeQuaternionSquared();
  float dotProduct(const Quaternion& b) const;
  void negateQuaternion();
  void setToRotatAboutX(float theta);
  void setToRotatAboutY(float theta);
  void setToRotatAboutZ(float theta);
  Quaternion& setToRotatAboutAxis(const glm::vec3& axis, float theta);
  static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float time);
  float Angle(Quaternion& a, Quaternion& b);


};

template<class T>
const T& clamp(const T& x, const T& upper, const T& lower);

template<class T>
const T& clamp(const T& x, const T& upper, const T& lower)
{
  return std::min(upper, std::max(x, lower));
}

static std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
{
  os << "w: " << quat.w << " x: " << quat.x << " y: " << quat.y << " z: " << quat.z;
  return os;
}

Quaternion operator*(float scalar, const Quaternion& a);
Quaternion Slerp(const Quaternion& a, const Quaternion& b, float time);

