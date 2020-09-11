#include "Quaternion.h"

// adding two quaternions
const Quaternion Quaternion::operator+(const Quaternion & b) const
{
  Quaternion c;
  c.w = this->w + b.w;
  c.x = this->x + b.x;
  c.y = this->y + b.y;
  c.z = this->z + b.z;

  return c;
}

//subtractiong two qauternions
const Quaternion Quaternion::operator-(const Quaternion& b) const
{
  Quaternion c;
  c.w = this->w - b.w;
  c.x = this->x - b.x;
  c.y = this->y - b.y;
  c.z = this->z - b.z;

  return c;
}

//cross product of quaternions
const Quaternion Quaternion::operator*(const Quaternion& b)
{
  Quaternion c;

  c.w = w * b.w - (x * b.x) - (y * b.y) - (z * b.z);
  c.x = w * b.x + (x * b.w) + (z * b.y) - (y * b.z);
  c.y = w * b.y + (y * b.w) + (x * b.z) - (z * b.x);
  c.z = w * b.z + (z * b.w) + (y * b.x) - (x * b.y);

  return c;
}

//scaling a quaternion
const Quaternion Quaternion::operator*(float scalar)
{
  Quaternion b;

  b.w *= scalar;
  b.x *= scalar;
  b.y *= scalar;
  b.z *= scalar;
  return b;
}

//dividing the componenets of a quaternion
Quaternion Quaternion::operator/(float scalar)
{
  return Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
}

Quaternion& Quaternion::normalized()
{
  // 1 / mag(quat)
  float len_inv = 1.0f / sqrtf(w * w + x * x + y * y + z * z); //sqrtf is a heavy operation but cant be avoided

  w *= len_inv;
  x *= len_inv;
  y *= len_inv;
  z *= len_inv;

  return *this;
}

Quaternion& Quaternion::conjugate()
{
  x *= -1.0f;
  y *= -1.0f;
  z *= -1.0f;

  return *this;
}

Quaternion& Quaternion::inverse()
{
  Quaternion b = *this;

  *this = b.conjugate() / magnitudeQuaternionSquared();

  return *this;

}

Quaternion& Quaternion::projection(Quaternion& b)
{
  const float dot = dotProduct(b);
  w = dot * b.w;
  x = dot * b.x;
  y = dot * b.y;
  z = dot * b.z;

  return *this;
}

float Quaternion::magnitudeQuaternionSquared()
{
  return w * w + x * x + y * y + z * z; //sqrtf is a heavy operation but cant be avoided
}

float Quaternion::dotProduct(Quaternion& b)
{
  return w * b.w + x * b.x + y * b.y + z * b.z;
}

void Quaternion::negateQuaternion()
{
  w *= -1.0f;
  x *= -1.0f;
  y *= -1.0f;
  z *= -1.0f;
}
