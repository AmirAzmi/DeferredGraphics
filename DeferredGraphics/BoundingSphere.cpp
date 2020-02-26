#include "BoundingSphere.h"
BoundingSphere::BSInfo BoundingSphere::calculateBS(BoundingSphereCalculationType mode)
{
  //doing this because I current dont know, I will chang it to mode once I know for sure it works
  BoundingSphereCalculationType myType = mode;

  switch (myType)
  {
    case BoundingSphereCalculationType::Centroid:

    { type = mode; //set the type as well
      return info;
    }

    case BoundingSphereCalculationType::Ritters:
    {
      type = mode; //set the type as well
      return info;
    }

    case BoundingSphereCalculationType::ModifiedLarsons:
    {
      type = mode; //set the type as well
      return info;
    }

    case BoundingSphereCalculationType::PCA:
    {
      type = mode; //set the type as well
      return info;
    }

    default:
    {
      type = mode; //set the type as well
      return info;
    }
  }
  
}
