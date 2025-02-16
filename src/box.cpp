#include "box.h"

Box::Box(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty):Plane(c, t, ya, pi, ro, tx, ty){}
Box::Box(const Vector &c, Texture* t, double ya, double pi, double ro, double tx):Plane(c, t, ya, pi, ro, tx,tx){}

double Box::getIntersection(Ray ray){
   double time = Plane::getIntersection(ray);
   Vector dist = solveScalers(right, up, vect, ray.point+ray.vector*time-center);
   if(time==inf) 
      return time;
   return ( ((dist.x>=0)?dist.x:-dist.x)>textureX/2 || ((dist.y>=0)?dist.y:-dist.y)>textureY/2 )?inf:time;
}

bool Box::getLightIntersection(Ray ray, double* fill){
   const double t = ray.vector.dot(vect);
   const double norm = vect.dot(ray.point)+d;
   const double r = -norm/t;
   if(r<=0. || r>=1.) return false;
   Vector dist = solveScalers(right, up, vect, ray.point+ray.vector*r-center);
   if( ((dist.x>=0)?dist.x:-dist.x)>textureX/2 || ((dist.y>=0)?dist.y:-dist.y)>textureY/2 ) return false;

   if(texture->opacity>1-1E-6) return true;   
   unsigned char temp[4];
   double amb, op, ref;
   texture->getColor(temp, &amb, &op, &ref,fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
   if(op>1-1E-6) return true;
   fill[0]*=temp[0]/255.;
   fill[1]*=temp[1]/255.;
   fill[2]*=temp[2]/255.;
   return false;
}

void Box::getBoundingBox(Vector& min, Vector& max) const {
    // 计算8个角点
    Vector corners[8];
    Vector halfRight = right * (textureX * 0.5);
    Vector halfUp = up * (textureY * 0.5);
    Vector halfNormal = vect * (textureX * 0.5); // 假设深度等于宽度

    corners[0] = center + halfRight + halfUp + halfNormal;
    corners[1] = center + halfRight + halfUp - halfNormal;
    corners[2] = center + halfRight - halfUp + halfNormal;
    corners[3] = center + halfRight - halfUp - halfNormal;
    corners[4] = center - halfRight + halfUp + halfNormal;
    corners[5] = center - halfRight + halfUp - halfNormal;
    corners[6] = center - halfRight - halfUp + halfNormal;
    corners[7] = center - halfRight - halfUp - halfNormal;

    // 初始化包围盒
    min = corners[0];
    max = corners[0];

    // 扩展包围盒以包含所有角点
    for(int i = 1; i < 8; i++) {
        min.x = std::min(min.x, corners[i].x);
        min.y = std::min(min.y, corners[i].y);
        min.z = std::min(min.z, corners[i].z);
        max.x = std::max(max.x, corners[i].x);
        max.y = std::max(max.y, corners[i].y);
        max.z = std::max(max.z, corners[i].z);
    }
}
