#include<string.h>
#include<stdio.h>
#include<limits>
#include<math.h>
#include<stdlib.h>
//#include <printf.h>
#include <stddef.h>
#include "vector.h"

Vector::Vector() : x(0), y(0), z(0) {}
Vector::Vector(double x, double y, double z) : x(x), y(y), z(z) {}

Vector Vector::operator-() const { 
    return Vector(-x, -y, -z); 
}

Vector Vector::cross(const Vector& v) const {
    return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

Vector Vector::normalize() const {
    double l = sqrt(dot(*this));
    return Vector(x/l, y/l, z/l);
}

#ifdef ENABLE_SIMD
Vector Vector::operator+(const Vector& v) const {
    __m256d a = _mm256_setr_pd(x, y, z, 0);
    __m256d b = _mm256_setr_pd(v.x, v.y, v.z, 0);
    __m256d result = _mm256_add_pd(a, b);
    double temp[4];
    _mm256_store_pd(temp, result);
    return Vector(temp[0], temp[1], temp[2]);
}

Vector Vector::operator-(const Vector& v) const {
    __m256d a = _mm256_setr_pd(x, y, z, 0);
    __m256d b = _mm256_setr_pd(v.x, v.y, v.z, 0);
    __m256d result = _mm256_sub_pd(a, b);
    double temp[4];
    _mm256_store_pd(temp, result);
    return Vector(temp[0], temp[1], temp[2]);
}

Vector Vector::operator*(double f) const {
    __m256d a = _mm256_setr_pd(x, y, z, 0);
    __m256d b = _mm256_set1_pd(f);
    __m256d result = _mm256_mul_pd(a, b);
    double temp[4];
    _mm256_store_pd(temp, result);
    return Vector(temp[0], temp[1], temp[2]);
}

double Vector::dot(const Vector& v) const {
    __m256d a = _mm256_setr_pd(x, y, z, 0);
    __m256d b = _mm256_setr_pd(v.x, v.y, v.z, 0);
    __m256d result = _mm256_mul_pd(a, b);
    double temp[4];
    _mm256_store_pd(temp, result);
    return temp[0] + temp[1] + temp[2];
}

Vector Vector::min(const Vector& a, const Vector& b) {
    __m256d va = _mm256_setr_pd(a.x, a.y, a.z, 0);
    __m256d vb = _mm256_setr_pd(b.x, b.y, b.z, 0);
    __m256d result = _mm256_min_pd(va, vb);
    double temp[4];
    _mm256_store_pd(temp, result);
    return Vector(temp[0], temp[1], temp[2]);
}

Vector Vector::max(const Vector& a, const Vector& b) {
    __m256d va = _mm256_setr_pd(a.x, a.y, a.z, 0);
    __m256d vb = _mm256_setr_pd(b.x, b.y, b.z, 0);
    __m256d result = _mm256_max_pd(va, vb);
    double temp[4];
    _mm256_store_pd(temp, result);
    return Vector(temp[0], temp[1], temp[2]);
}

#else
Vector Vector::operator+(const Vector& v) const { 
    return Vector(x + v.x, y + v.y, z + v.z); 
}

Vector Vector::operator-(const Vector& v) const { 
    return Vector(x - v.x, y - v.y, z - v.z); 
}

Vector Vector::operator*(double f) const { 
    return Vector(x * f, y * f, z * f); 
}

double Vector::dot(const Vector& v) const { 
    return x * v.x + y * v.y + z * v.z; 
}

Vector Vector::min(const Vector& a, const Vector& b) {
    return Vector(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
}

Vector Vector::max(const Vector& a, const Vector& b) {
    return Vector(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));
}
#endif

void Vector::operator -= (const Vector rhs) {
   x-=rhs.x; y-=rhs.y; z-=rhs.z;
}
void Vector::operator += (const Vector rhs) {
   x+=rhs.x; y+=rhs.y; z+=rhs.z;
}
void Vector::operator *= (const double rhs) {
   x*=rhs; y*=rhs; z*=rhs;
}
void Vector::operator *= (const float rhs) {
   x*=rhs; y*=rhs; z*=rhs;
}
void Vector::operator *= (const int rhs) {
   x*=rhs; y*=rhs; z*=rhs;
}
void Vector::operator /= (const double rhs) {
   x/=rhs; y/=rhs; z/=rhs;
}
void Vector::operator /= (const float rhs) {
   x/=rhs; y/=rhs; z/=rhs;
}
void Vector::operator /= (const int rhs) {
   x/=rhs; y/=rhs; z/=rhs;
}

Vector Vector::operator * (const double rhs) {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator * (const float rhs) {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator * (const int rhs) {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator / (const double rhs) {
   return Vector(x/rhs, y/rhs, z/rhs);
}
Vector Vector::operator / (const float rhs) {
   return Vector(x/rhs, y/rhs, z/rhs);
}
Vector Vector::operator / (const int rhs) {
   return Vector(x/rhs, y/rhs, z/rhs);
}

double Vector::mag2(){
   return x*x+y*y+z*z; 
}
double Vector::mag(){
   return sqrt(x*x+y*y+z*z); 
}

Vector solveScalers(Vector v1, Vector v2, Vector v3, Vector C){
   double denom = v1.z*v2.y*v3.x-v1.y*v2.z*v3.x-v1.z*v2.x*v3.y+v1.x*v2.z*v3.y+v1.y*v2.x*v3.z-v1.x*v2.y*v3.z;
   double a = C.z*v2.y*v3.x-C.y*v2.z*v3.x-C.z*v2.x*v3.y+C.x*v2.z*v3.y+C.y*v2.x*v3.z-C.x*v2.y*v3.z;
   double b = -C.z*v1.y*v3.x+C.y*v1.z*v3.x+C.z*v1.x*v3.y-C.x*v1.z*v3.y-C.y*v1.x*v3.z+C.x*v1.y*v3.z;
   double c = C.z*v1.y*v2.x-C.y*v1.z*v2.x-C.z*v1.x*v2.y+C.x*v1.z*v2.y+C.y*v1.x*v2.z-C.x*v1.y*v2.z;
   return Vector(a/denom, b/denom, c/denom);
}

Ray::Ray(const Vector& po, const Vector& ve): point(po), vector(ve){}
