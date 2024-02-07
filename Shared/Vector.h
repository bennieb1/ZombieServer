#pragma once
// Vector.h
#ifndef VECTOR_H
#define VECTOR_H

#include <./BitStream.h>
#include <RakNetTypes.h>
#include <MessageIdentifiers.h>
// For RakNet::BitStream

// For sqrt, pow, and other mathematical operations

class Vector {
public:
    float x, y, z;

    // Default constructor initializes to zero.
    Vector(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    // Vector addition
    Vector operator+(const Vector& rhs) const {
        return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    // Vector subtraction
    Vector operator-(const Vector& rhs) const {
        return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    // Scalar multiplication
    Vector operator*(float scalar) const {
        return Vector(x * scalar, y * scalar, z * scalar);
    }

    // Computes the magnitude (length) of the vector
    float magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalizes the vector
    Vector normalize() const {
        float mag = magnitude();
        if (mag > 0) {
            return *this * (1.0f / mag);
        }
        return *this; // Return the original vector if it's of zero length
    }

    // Serialize vector to bitstream
    void Serialize(RakNet::BitStream& bsOut) const {
        bsOut.Write(x);
        bsOut.Write(y);
        bsOut.Write(z);
    }

    // Deserialize vector from bitstream
    void Deserialize(RakNet::BitStream& bsIn) {
        bsIn.Read(x);
        bsIn.Read(y);
        bsIn.Read(z);
    }
};

#endif // VECTOR_H
