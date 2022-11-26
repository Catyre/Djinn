/**
 * @file core.h
 * @brief Tools for vector math and other useful functions
 * @author Catyre
 * @date 11-10-2022
*/

#ifndef CORE_H
#define CORE_H
#define EPSILON 1e-15

#include "precision.h"
#include "raylib.h"
#include <string>
#include <iostream>
#include <sstream>

namespace engine {
    class Vec3 {
        public:
            // Spatial coordinates
            real x;
            real y;
            real z;

        private:
            // Padding to ensure four word alignment
            real pad;

        public:
            // Default constructor creates a 0 vector
            Vec3() : x(0), y(0), z(0) {}

            // Constructor for when values are passed
            Vec3(const real x, const real y, const real z) : x(x), y(y), z(z) {}

            void invert() {
                x = -x;
                y = -y;
                z = -z;
            }

            std::string toString() {
                std::stringstream ss;

                ss << std::scientific << "<" << x << ", " << y << ", " << z << ">";

                return ss.str();
            }

            // Get magnitude of vector
            real magnitude() const {
                return real_sqrt(x*x + y*y + z*z);
            }

            // Sometimes it is useful and faster to just have the square of the magnitude
            real squareMagnitude() const {
                return x * x + y * y + z * z;
            }

            // Normalize a non-zero vector
            void normalize() {
                real l = magnitude();
                if (l > 0) {
                    (*this) *= ((real)1)/l;
                }
            }

            // Multiplies vector by given scalar
            void operator *= (const real scalar) {
                x *= scalar;
                y *= scalar;
                z *= scalar;
            }

            // Returns vector scaled by value
            Vec3 operator * (const real value) const {
                return Vec3(x * value, y * value, z * value);
            }

            Vec3 operator / (const real value) const {
                return Vec3(x / value, y / value, z / value);
            }

            void operator /= (const real value) {
                x /= value;
                y /= value;
                z /= value;
            }

            // Adds given vector
            Vec3 operator + (const Vec3& v) const {
                return Vec3(x + v.x, y + v.y, z + v.z);
            }

            void operator += (const Vec3& v) {
                x += v.x;
                y += v.y;
                z += v.z;
            }

            // Subtracts given vector
            void operator -= (const Vec3& v) {
                x -= v.x;
                y -= v.y;
                z -= v.z;
            }

            bool operator == (const Vec3& v) const {
                return (*this - v).isZero();
            }

            bool operator != (const Vec3& v) {
                return !(*this == v);
            }

            Vec3 operator - (const Vec3& v) const {
                return Vec3(x - v.x, y - v.y, z - v.z);
            }

            // Adds a given scaled vector
            void addScaledVector(const Vec3& v, real scale) {
                x += (v.x * scale);
                y += (v.y * scale);
                z += (v.z * scale);
            }

            // Calculate COMPONENT product of this vector with a given one
            Vec3 componentProduct(const Vec3& v) const {
                return Vec3(x * v.x, y * v.y, z * v.z);
            }

            void componentProductUpdate(const Vec3& v) {
                x *= v.x;
                y *= v.y;
                z *= v.z;
            }

            // Calculates and returns scalar product of this vector with given vector
            real scalarProduct(const Vec3& v) const {
                return x * v.x + y * v.y + z * v.z;
            }

            real operator * (const Vec3& v) const {
                return x * v.x + y * v.y + z * v.z;
            }

            // Calculate vector product of this vector and a given vector
            Vec3 vectorProduct(const Vec3& v) const {
                return Vec3(y * v.z - z * v.y,
                            z * v.x - x * v.z,
                            x * v.y - y * v.x);
            }

            // Updates this vector to be the vector product of its current value and the given vector
            void operator %= (const Vec3& v) {
                *this = vectorProduct(v);
            }

            // Calculates and returns the vector product of this vector with the given vector
            Vec3 operator % (const Vec3& v) const {
                return Vec3(y * v.z - z * v.y,
                            z * v.x - x * v.z,
                            x * v.y - y * v.x);
            }

            void operator = (const Vec3& v) {
                x = v.x;
                y = v.y;
                z = v.z;
            }

            void clear() {
                x = 0.0f;
                y = 0.0f;
                z = 0.0f;
            }

            bool isZero() const {
                return this->magnitude() < EPSILON;
            }

            Vector3 toVector3() {
                return (Vector3){static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
            }
    }; // class Vec3
}; // namespace engine

#endif