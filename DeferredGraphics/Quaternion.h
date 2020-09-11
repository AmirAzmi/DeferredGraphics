#pragma once
#include <glm/glm/glm.hpp>

class Quaternion
{
  float x, y, z, w;

  Quaternion()
  {

  }

  Quaternion(float w, float x, float y, float z):w(w), x(x), y(y), z(z)
  {
  }
  Quaternion(glm::vec3 & axis, float angle)
  {
    angle = glm::radians(angle);
    w = cosf(0.5f * angle);

    float thetaOver2 = sinf(0.5f * angle);
    x = axis.x * thetaOver2;
    y = axis.y * thetaOver2;
    z = axis.z * thetaOver2;
  }

  const Quaternion operator+(const Quaternion& b) const;
  const Quaternion operator-(const Quaternion& b) const;
  const Quaternion operator*(const Quaternion& b); //quaternion cross product
  const Quaternion operator*(float scalar);
  Quaternion operator/(float scalar);

  //operations
  Quaternion& normalized();
  Quaternion& conjugate();
  Quaternion& inverse();
  Quaternion& projection(Quaternion& b);
  float magnitudeQuaternionSquared();
  float dotProduct(Quaternion& b);
  void negateQuaternion();
  void setToRotatAboutX(float theta);
  void setToRotatAboutY(float theta);
  void setToRotatAboutZ(float theta);
  void setToRotatAboutAxis(glm::vec3& axis, float theta);
};

