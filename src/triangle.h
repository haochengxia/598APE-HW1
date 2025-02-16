#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__
#include "plane.h"

class Triangle : public Plane{
public:
   double thirdX;
   Triangle(Vector c, Vector b, Vector a, Texture* t);
   double getIntersection(Ray ray);
   bool getLightIntersection(Ray ray, double* fill);
   void getBoundingBox(Vector& min, Vector& max) const override {
       // Triangle已经存储了三个点的信息
       Vector p1 = center;
       Vector p2 = center + right * textureX;
       Vector p3 = center + right * thirdX + up * textureY;
       
       min.x = std::min({p1.x, p2.x, p3.x});
       min.y = std::min({p1.y, p2.y, p3.y});
       min.z = std::min({p1.z, p2.z, p3.z});
       
       max.x = std::max({p1.x, p2.x, p3.x});
       max.y = std::max({p1.y, p2.y, p3.y});
       max.z = std::max({p1.z, p2.z, p3.z});
   }
};

#endif
