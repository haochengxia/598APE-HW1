#ifndef __SHAPE_H__
#define __SHAPE_H__
#include "light.h"

class Shape {
private:
    static thread_local int maxRecursionDepth;
    static thread_local int currentDepth;

public:
    Shape(const Vector &c, Texture* t, double ya, double pi, double ro);
    double yaw, pitch, roll, xsin, xcos, ysin, ycos, zsin, zcos;
    Vector center;
    Texture* texture;
    double textureX, textureY, mapX, mapY, mapOffX, mapOffY;
    Texture* normalMap;

    // Recursion depth tracking methods
    static int getMaxDepth() { return maxRecursionDepth; }
    static void resetDepth() { 
        maxRecursionDepth = 0; 
        currentDepth = 0; 
    }
    static void enterRecursion() {
        currentDepth++;
        if (currentDepth > maxRecursionDepth) {
            maxRecursionDepth = currentDepth;
        }
    }
    static void exitRecursion() {
        currentDepth--;
    }

    // Original virtual functions
    virtual double getIntersection(Ray ray) = 0;
    virtual bool getLightIntersection(Ray ray, double* fill) = 0;
    virtual void move() = 0;
    virtual unsigned char reversible() = 0;
    virtual void getColor(unsigned char* toFill, double* am, double* op, double* ref, Autonoma* r, Ray ray, unsigned int depth) = 0;
    virtual Vector getNormal(Vector point) = 0;
    virtual void setAngles(double yaw, double pitch, double roll) = 0;
    virtual void setYaw(double d) = 0;
    virtual void setPitch(double d) = 0;
    virtual void setRoll(double d) = 0;
};

void calcColor(unsigned char* toFill, Autonoma*, Ray ray, unsigned int depth);

#endif
