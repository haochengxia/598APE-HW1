#include "shape.h"

#include <vector>
#include <algorithm>
#include <cstring>

thread_local int Shape::maxRecursionDepth = 0;
thread_local int Shape::currentDepth = 0;

Shape::Shape(const Vector &c, Texture* t, double ya, double pi, double ro): center(c), texture(t), yaw(ya), pitch(pi), roll(ro){
};

void Shape::setAngles(double a, double b, double c){
   yaw =a; pitch = b; roll = c;
   xcos = cos(yaw);
   xsin = sin(yaw);
   ycos = cos(pitch);
   ysin = sin(pitch);
   zcos = cos(roll);
   zsin = sin(roll);
}

void Shape::setYaw(double a){
   yaw =a;
   xcos = cos(yaw);
   xsin = sin(yaw);
}

void Shape::setPitch(double b){
   pitch = b;
   ycos = cos(pitch);
   ysin = sin(pitch);
}

void Shape::setRoll(double c){
   roll = c;
   zcos = cos(roll);
   zsin = sin(roll);
}

typedef struct {
   double time;
   Shape* shape;
} TimeAndShape;

void insertionSort(TimeAndShape *arr, int n) {
    for (int i = 1; i < n; ++i) {
        TimeAndShape key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].time > key.time) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

#ifdef ENABLE_OPT_CACHE
void calcColor(unsigned char* toFill, Autonoma* c, Ray ray, unsigned int depth)
{
    // Constants for optimization
    constexpr size_t MAX_STACK_SIZE = 64;
    constexpr double EPSILON = 1e-6;
    
    // Stack frame structure for iterative ray tracing
    struct Frame {
        Ray ray;
        unsigned char color[3];
        double opacity;
        double reflection;
        double ambient;
        Vector intersect;
        Shape* shape;
        int stage;
        
        // Default constructor
        Frame() : ray(Vector(0,0,0), Vector(0,0,0)), 
                 opacity(1.0), reflection(0), ambient(0),
                 intersect(0,0,0), shape(nullptr), stage(0) {
            color[0] = color[1] = color[2] = 0;
        }
        
        // Constructor with ray parameter
        Frame(const Ray& r) : 
            ray(r), opacity(1.0), reflection(0), ambient(0), 
            intersect(0,0,0), shape(nullptr), stage(0) {
            color[0] = color[1] = color[2] = 0;
        }
    };

    Frame stack[MAX_STACK_SIZE];
    size_t stackSize = 1;
    
    stack[0] = Frame(ray);

    while (stackSize > 0) {
        Frame& current = stack[stackSize - 1];
        
        switch (current.stage) {
        case 0: { 
            double minTime = std::numeric_limits<double>::infinity();
            Shape* nearestShape = nullptr;
            
            for (ShapeNode* t = c->listStart; t != nullptr; t = t->next) {
                double time = t->data->getIntersection(current.ray);
                if (time > EPSILON && time < minTime) {
                    minTime = time;
                    nearestShape = t->data;
                }
            }

            if (minTime == std::numeric_limits<double>::infinity()) {
                Vector dir = current.ray.vector.normalize();
                double angle = std::atan2(dir.z, dir.x);
                double me = std::abs(dir.y);
                c->skybox->getColor(current.color, &current.ambient, 
                                  &current.opacity, &current.reflection,
                                  fix(angle/M_TWO_PI), fix(me));
                current.stage = 3;
                break;
            }

            current.shape = nearestShape;
            current.intersect = current.ray.point + current.ray.vector * minTime;
            
            current.shape->getColor(current.color, &current.ambient, 
                                  &current.opacity, &current.reflection,
                                  c, Ray(current.intersect, current.ray.vector), depth);

            double lightData[3];
            getLight(lightData, c, current.intersect, 
                    current.shape->getNormal(current.intersect),
                    current.shape->reversible());
            
            for (int i = 0; i < 3; ++i) {
                double color = current.color[i] * 
                             (current.ambient + lightData[i] * (1 - current.ambient));
                current.color[i] = (unsigned char)std::clamp(color, 0.0, 255.0);
            }

            current.stage = 1;
            break;
        }
        
        case 1: { 
            if (current.opacity < 1 - EPSILON && depth < c->depth && stackSize < MAX_STACK_SIZE) {
                Ray nextRay(current.intersect + current.ray.vector * EPSILON,
                           current.ray.vector);
                new (&stack[stackSize]) Frame(nextRay);
                stackSize++;
            }
            current.stage = 2;
            break;
        }
        
        case 2: { 
            if (current.reflection > EPSILON && depth < c->depth && stackSize < MAX_STACK_SIZE) {
                Vector norm = current.shape->getNormal(current.intersect).normalize();
#ifdef ENABLE_INPLACE
        Vector reflectedVec = current.ray.vector;
        reflectedVec.addScaled(norm, -2 * norm.dot(current.ray.vector));  // Using inplace operation
#else
        Vector reflectedVec = current.ray.vector - norm * (2 * norm.dot(current.ray.vector));
#endif
                Ray nextRay(current.intersect + reflectedVec * EPSILON, reflectedVec);
                new (&stack[stackSize]) Frame(nextRay);
                stackSize++;
            }
            current.stage = 3;
            break;
        }
        
        case 3: { 
            if (stackSize > 1) {
                Frame& parent = stack[stackSize - 2];
                for (int i = 0; i < 3; ++i) {
                    if (current.opacity < 1 - EPSILON) {
                        parent.color[i] = (unsigned char)(
                            parent.color[i] * current.opacity + 
                            current.color[i] * (1 - current.opacity)
                        );
                    }
                    if (current.reflection > EPSILON) {
                        parent.color[i] = (unsigned char)(
                            parent.color[i] * (1 - current.reflection) + 
                            current.color[i] * current.reflection
                        );
                    }
                }
            }
            stackSize--;
            break;
        }
        }
    }

    std::memcpy(toFill, stack[0].color, 3);
}
#else
void calcColor(unsigned char* toFill,Autonoma* c, Ray ray, unsigned int depth){
   Shape::enterRecursion();
   ShapeNode* t = c->listStart;
   TimeAndShape *times = (TimeAndShape*)malloc(0);
   size_t seen = 0;
   while(t!=NULL){
      double time = t->data->getIntersection(ray);

      TimeAndShape *times2 = (TimeAndShape*)malloc(sizeof(TimeAndShape)*(seen + 1));
      for (int i=0; i<seen; i++)
         times2[i] = times[i];
      times2[seen] = (TimeAndShape){ time, t->data };
      free(times);
      times = times2;
      seen ++;
      t = t->next;
   }
   insertionSort(times, seen);
   if (seen == 0 || times[0].time == inf) {
      double opacity, reflection, ambient;
      Vector temp = ray.vector.normalize();
      const double x = temp.x;
      const double z = temp.z;
      const double me = (temp.y<0)?-temp.y:temp.y;
      const double angle = atan2(z, x);
      c->skybox->getColor(toFill, &ambient, &opacity, &reflection, fix(angle/M_TWO_PI),fix(me));
      return;
   }

   double curTime = times[0].time;
   Shape* curShape = times[0].shape;
   free(times);

   Vector intersect = curTime*ray.vector+ray.point;
   double opacity, reflection, ambient;
   curShape->getColor(toFill, &ambient, &opacity, &reflection, c, Ray(intersect, ray.vector), depth);
   
   double lightData[3];
   getLight(lightData, c, intersect, curShape->getNormal(intersect), curShape->reversible());
   toFill[0] = (unsigned char)(toFill[0]*(ambient+lightData[0]*(1-ambient)));
   toFill[1] = (unsigned char)(toFill[1]*(ambient+lightData[1]*(1-ambient)));
   toFill[2] = (unsigned char)(toFill[2]*(ambient+lightData[2]*(1-ambient)));
   if(depth<c->depth && (opacity<1-1e-6 || reflection>1e-6)){
      //printf("depth: %d, opacity: %f, reflection: %f\n", depth, opacity, reflection);
      unsigned char col[4];
      if(opacity<1-1e-6){
         Ray nextRay = Ray(intersect+ray.vector*1E-4, ray.vector);
         calcColor(col, c, nextRay, depth+1);
         toFill[0]= (unsigned char)(toFill[0]*opacity+col[0]*(1-opacity));
         toFill[1]= (unsigned char)(toFill[1]*opacity+col[1]*(1-opacity));
         toFill[2]= (unsigned char)(toFill[2]*opacity+col[2]*(1-opacity));        
      }
      if(reflection>1e-6){

        Vector norm = curShape->getNormal(intersect).normalize();
#ifdef ENABLE_INPLACE
        Vector reflectedVec = ray.vector;
        reflectedVec.addScaled(norm, -2 * norm.dot(ray.vector));  // Using inplace operation
#else
        Vector reflectedVec = ray.vector - norm * (2 * norm.dot(ray.vector));
#endif
        Ray nextRay(intersect + reflectedVec * 1E-4, reflectedVec);
        calcColor(col, c, nextRay, depth+1);
      
         toFill[0]= (unsigned char)(toFill[0]*(1-reflection)+col[0]*(reflection));
         toFill[1]= (unsigned char)(toFill[1]*(1-reflection)+col[1]*(reflection));
         toFill[2]= (unsigned char)(toFill[2]*(1-reflection)+col[2]*(reflection));
      }
   }
   Shape::exitRecursion();
}
#endif
