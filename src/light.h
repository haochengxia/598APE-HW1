#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "vector.h"
#include "camera.h"
#include "Textures/texture.h"
#include "Textures/colortexture.h"

#ifdef ENABLE_MPOOL
#include "mempool.h"
#endif

class Light{
  public:
   unsigned char* color;
   unsigned char* getColor(unsigned char a, unsigned char b, unsigned char c);
   Vector center;
   Light(const Vector & cente, unsigned char* colo);
};

struct LightNode{
   Light* data;
   LightNode* prev, *next;
};

class Shape;
struct ShapeNode{
   Shape* data;
   ShapeNode* prev, *next;
};

#ifdef ENABLE_BVH
struct AABB {
    Vector min, max;
    AABB() : min(INFINITY, INFINITY, INFINITY), max(-INFINITY, -INFINITY, -INFINITY) {}
    
    bool intersect(const Ray& ray) const {
        Vector invDir(1.0/ray.direction.x, 1.0/ray.direction.y, 1.0/ray.direction.z);
        Vector t0 = (min - ray.origin) * invDir;
        Vector t1 = (max - ray.origin) * invDir;
        double tmin = fmax(fmax(fmin(t0.x, t1.x), fmin(t0.y, t1.y)), fmin(t0.z, t1.z));
        double tmax = fmin(fmin(fmax(t0.x, t1.x), fmax(t0.y, t1.y)), fmax(t0.z, t1.z));
        return tmax >= tmin && tmax > 0;
    }
};

class BVHNode {
public:
    AABB bounds;
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;
    std::vector<ShapeNode*> shapes;
};
#endif

class Autonoma{
public:
   Camera camera;
   Texture* skybox;
   unsigned int depth;
   ShapeNode *listStart, *listEnd;
   LightNode *lightStart, *lightEnd;
   
#ifdef ENABLE_BVH
   BVHNode* bvh_root = nullptr;
   
   void buildBVH();
#endif

   Shape* getIntersection(Ray r, double& time);

   Autonoma(const Camera &c);
   Autonoma(const Camera &c, Texture* tex);
   void addShape(Shape* s);
   void removeShape(ShapeNode* s);
   void addLight(Light* s);
   void removeLight(LightNode* s);

#ifdef ENABLE_MPOOL
   static SimplePool nodePool;  // 一个pool同时处理shape和light节点
#endif
};

#ifdef ENABLE_MPOOL
SimplePool Autonoma::nodePool;
#endif

void getLight(double* toFill, Autonoma* aut, Vector point, Vector norm, unsigned char r);

#endif
