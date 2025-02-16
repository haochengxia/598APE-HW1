#ifndef __PLANE_H__
#define __PLANE_H__

#include "shape.h"

class Plane : public Shape{
public:
  Vector vect, right, up;
  double d;
  Plane(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty);
  double getIntersection(Ray ray);
  bool getLightIntersection(Ray ray, double* toFill);
  void move();
  void getColor(unsigned char* toFill, double* am, double* op, double* ref, Autonoma* r, Ray ray, unsigned int depth);
  Vector getNormal(Vector point);
  unsigned char reversible();
  void setAngles(double yaw, double pitch, double roll);
  void setYaw(double d);
  void setPitch(double d);
  void setRoll(double d);
  void getBoundingBox(Vector& min, Vector& max) const override {
    // 计算四个角点
    Vector corners[4];
    corners[0] = center + (right * textureX * 0.5) + (up * textureY * 0.5);
    corners[1] = center + (right * textureX * 0.5) - (up * textureY * 0.5);
    corners[2] = center - (right * textureX * 0.5) + (up * textureY * 0.5);
    corners[3] = center - (right * textureX * 0.5) - (up * textureY * 0.5);
    
    // 初始化包围盒
    min = corners[0];
    max = corners[0];
    
    // 扩展包围盒以包含所有角点
    for(int i = 1; i < 4; i++) {
        min.x = std::min(min.x, corners[i].x);
        min.y = std::min(min.y, corners[i].y);
        min.z = std::min(min.z, corners[i].z);
        max.x = std::max(max.x, corners[i].x);
        max.y = std::max(max.y, corners[i].z);
        max.z = std::max(max.z, corners[i].z);
    }
  }
};

#endif
