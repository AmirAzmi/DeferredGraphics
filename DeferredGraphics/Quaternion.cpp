#include "Quaternion.h"

// adding two quaternions
Quaternion Quaternion::operator+(const Quaternion & b) const
{
  Quaternion c;
  c.w = this->w + b.w;
  c.x = this->x + b.x;
  c.y = this->y + b.y;
  c.z = this->z + b.z;

  return c;
}

//subtractiong two qauternions
Quaternion Quaternion::operator-(const Quaternion& b) const
{
  Quaternion c;
  c.w = this->w - b.w;
  c.x = this->x - b.x;
  c.y = this->y - b.y;
  c.z = this->z - b.z;

  return c;
}

//cross product of quaternions
Quaternion Quaternion::operator*(const Quaternion& b) const
{
  Quaternion c;

  c.w = w * b.w - (x * b.x) - (y * b.y) - (z * b.z);
  c.x = w * b.x + (x * b.w) + (z * b.y) - (y * b.z);
  c.y = w * b.y + (y * b.w) + (x * b.z) - (z * b.x);
  c.z = w * b.z + (z * b.w) + (y * b.x) - (x * b.y);

  return c;
}

//scaling a quaternion
Quaternion Quaternion::operator*(float scalar) const
{
  Quaternion b(*this);

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

Quaternion& Quaternion::projection(const Quaternion& b)
{
  const float dot = dotProduct(b);
  w = dot * b.w;
  x = dot * b.x;
  y = dot * b.y;
  z = dot * b.z;

  return *this;
}

glm::mat4 Quaternion::quaternionToMatrix()
{

  //TODO::save by not doing the transpose
  return glm::transpose(glm::mat4(
    1 - (2 * (std::pow(y, 2) + std::pow(z, 2))), 2 * (x * y - w * z)                        , 2 * (x * z + w * y)                        , 0,
    2 * (x * y +  w * z)                       , 1 - (2 * (std::pow(x, 2) + std::pow(z, 2))), 2 * (y * z -  w * x)                       , 0,
    2 * (x * z -  w * y)                       , 2 * (y * z + w * x)                        , 1 - (2 * (std::pow(x, 2) + std::pow(y, 2))), 0,
    0                                          , 0                                          , 0                                          , 1));
}

float Quaternion::magnitudeQuaternionSquared()
{
  return w * w + x * x + y * y + z * z; //sqrtf is a heavy operation but cant be avoided
}

float Quaternion::dotProduct(const Quaternion& b) const
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

Quaternion & Quaternion::setToRotatAboutAxis(const glm::vec3& axis, float theta)
{
  float thetaOver2 = theta * 0.5f;
  this->w = cosf(thetaOver2);

  float sinThetaOver2 = sinf(thetaOver2);
  this->x = axis.x * sinThetaOver2;
  this->y = axis.y * sinThetaOver2;
  this->z = axis.z * sinThetaOver2;

  return *this;
}

Quaternion operator*(float scalar, const Quaternion& a)
{
  return a.operator*(scalar);
}

Quaternion Slerp(const Quaternion& a, const Quaternion& b, float time)
{
  const float omega = acosf(clamp(a.dotProduct(b), -1.0f, 1.0f));
  const float sin_inv = 1.0f / sinf(omega);

  return sinf((1.0f - time) * omega) * sin_inv * a + sin(time * omega) * sin_inv * b;
}

Quaternion Quaternion::Slerp(const Quaternion& a,const Quaternion& b, float time)
{
  const float omega = acosf(clamp(a.dotProduct(b), -1.0f, 1.0f));
  const float sin_inv = 1.0f / sinf(omega);

  return (1.0f - time) * a + time * b;
   
  //return sinf((1.0f - time) * omega) * sin_inv * a + sin(time * omega) * sin_inv * b;
}

float Quaternion::Angle(Quaternion& a, Quaternion& b)
{
  {
    Quaternion inv = a.inverse();
    Quaternion res = b * inv;

    //CLOSE ENOUGH
    if (acosf(res.w) * 2.0f > 3.14f)// * 57.2957795f; // To degrees!;
    {
      return 6.28f - acosf(res.w) * 2.0f;
    }
    else
    {
      return acosf(res.w) * 2.0f;
    }
  }
}

