#include "vector.h"

#ifdef ENABLE_SIMD
// 辅助函数：安全地加载和存储向量
inline __m256d safe_load(const Vector& v) {
    alignas(32) double temp[4] = {v.x, v.y, v.z, 0.0};
    return _mm256_load_pd(temp);
}

inline void safe_store(Vector& v, __m256d result) {
    alignas(32) double temp[4];
    _mm256_store_pd(temp, result);
    v.x = temp[0];
    v.y = temp[1];
    v.z = temp[2];
}
#endif

void Vector::operator+=(const Vector v) {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = safe_load(v);
    __m256d result = _mm256_add_pd(a, b);
    safe_store(*this, result);
#else
    x += v.x; y += v.y; z += v.z;
#endif
}

void Vector::operator-=(const Vector v) {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = safe_load(v);
    __m256d result = _mm256_sub_pd(a, b);
    safe_store(*this, result);
#else
    x -= v.x; y -= v.y; z -= v.z;
#endif
}

void Vector::operator*=(const double v) {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = _mm256_set1_pd(v);
    __m256d result = _mm256_mul_pd(a, b);
    safe_store(*this, result);
#else
    x *= v; y *= v; z *= v;
#endif
}

void Vector::operator*=(const float v){ operator*=((double)v); }
void Vector::operator*=(const int v){ operator*=((double)v); }

void Vector::operator/=(const double v) {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = _mm256_set1_pd(1.0/v);
    __m256d result = _mm256_mul_pd(a, b);
    safe_store(*this, result);
#else
    x /= v; y /= v; z /= v;
#endif
}

void Vector::operator/=(const float v){ operator/=((double)v); }
void Vector::operator/=(const int v){ operator/=((double)v); }

Vector Vector::operator+(const Vector v) const {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = safe_load(v);
    __m256d result = _mm256_add_pd(a, b);
    Vector ret;
    safe_store(ret, result);
    return ret;
#else
    return Vector(x + v.x, y + v.y, z + v.z);
#endif
}

Vector Vector::operator-(const Vector v) const {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = safe_load(v);
    __m256d result = _mm256_sub_pd(a, b);
    Vector ret;
    safe_store(ret, result);
    return ret;
#else
    return Vector(x - v.x, y - v.y, z - v.z);
#endif
}

Vector Vector::operator*(const double v) const {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = _mm256_set1_pd(v);
    __m256d result = _mm256_mul_pd(a, b);
    Vector ret;
    safe_store(ret, result);
    return ret;
#else
    return Vector(x * v, y * v, z * v);
#endif
}

Vector Vector::operator*(const float v) const { return operator*((double)v); }
Vector Vector::operator*(const int v) const { return operator*((double)v); }

Vector Vector::operator/(const double v) const {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = _mm256_set1_pd(1.0/v);
    __m256d result = _mm256_mul_pd(a, b);
    Vector ret;
    safe_store(ret, result);
    return ret;
#else
    return Vector(x / v, y / v, z / v);
#endif
}

Vector Vector::operator/(const float v) const { return operator/((double)v); }
Vector Vector::operator/(const int v) const { return operator/((double)v); }

double Vector::dot(const Vector v) const {
#ifdef ENABLE_SIMD
    __m256d a = safe_load(*this);
    __m256d b = safe_load(v);
    __m256d result = _mm256_mul_pd(a, b);
    alignas(32) double temp[4];
    _mm256_store_pd(temp, result);
    return temp[0] + temp[1] + temp[2];
#else
    return x * v.x + y * v.y + z * v.z;
#endif
}

Vector Vector::cross(const Vector v) const {
    return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

double Vector::mag2() const {
    return dot(*this);
}

double Vector::mag() const {
    return sqrt(mag2());
}

Vector Vector::normalize() const {
    return (*this) / mag();
}

Vector solveScalers(Vector v1, Vector v2, Vector v3, Vector solve){
    double a = ((v2.y * v3.z - v2.z * v3.y) * solve.x + (v1.z * v3.y - v1.y * v3.z) * solve.y + (v1.y * v2.z - v1.z * v2.y) * solve.z) /
        (v1.x * (v2.y * v3.z - v2.z * v3.y) + v2.x * (v1.z * v3.y - v1.y * v3.z) + v3.x * (v1.y * v2.z - v1.z * v2.y));
    double b = ((v2.z * v3.x - v2.x * v3.z) * solve.x + (v1.x * v3.z - v1.z * v3.x) * solve.y + (v1.z * v2.x - v1.x * v2.z) * solve.z) /
        (v1.y * (v2.z * v3.x - v2.x * v3.z) + v2.y * (v1.x * v3.z - v1.z * v3.x) + v3.y * (v1.z * v2.x - v1.x * v2.z));
    double c = ((v2.x * v3.y - v2.y * v3.x) * solve.x + (v1.y * v3.x - v1.x * v3.y) * solve.y + (v1.x * v2.y - v1.y * v2.x) * solve.z) /
        (v1.z * (v2.x * v3.y - v2.y * v3.x) + v2.z * (v1.y * v3.x - v1.x * v3.y) + v3.z * (v1.x * v2.y - v1.y * v2.x));
    return Vector(a,b,c);
}


