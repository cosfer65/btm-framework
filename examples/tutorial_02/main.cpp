#include <iostream>
#include "matrix.h"
#include "vector.h"
#include "curv_geom.h"

using namespace btm;

// C/C++ application entry point
int main(){
    std::cout << "Tutorial 02: Vectors, Points, and Spaces" << std::endl;

    fvec3 v(1.0f, 2.0f, 0.0f);   // Create a 3D vector with components (1, 2, 0)
    fvec3 u(0.0f, 1.0f, 0.0f);   // Create another 3D vector with components (0, 1, 0)

    auto sum = v + u;            // Vector addition
    auto scaled = 2.0f * v;      // Scalar multiplication (using the operator* defined for basevector)
    auto length = v.length();    // Compute the length (magnitude) of the vector
    auto normalized = v.norm();  // Normalize the vector (make it unit length)

    std::cout << "v: (" << v.x() << ", " << v.y() << ", " << v.z() << ")" << std::endl;
    std::cout << "u: (" << u.x() << ", " << u.y() << ", " << u.z() << ")" << std::endl;
    std::cout << "v + u: (" << sum.x() << ", " << sum.y() << ", " << sum.z() << ")" << std::endl;
    std::cout << "2 * v: (" << scaled.x() << ", " << scaled.y() << ", " << scaled.z() << ")" << std::endl;
    std::cout << "length of v: " << length << std::endl;
    std::cout << "normalized v: (" << normalized.x() << ", " << normalized.y() << ", " << normalized.z() << ")" << std::endl;

    fpoint3 A(1.0f, 1.0f, 0.0f); // Create a point A at coordinates (1, 1, 0)
    fpoint3 B(4.0f, 1.0f, 0.0f); // Create another point B at coordinates (4, 1, 0)

    fvec3 AB = B - A;   // Create a vector AB from point A to point B (B - A)
    fpoint3 C = A + AB; // Create a point C by adding vector AB to point A (returns B)
    std::cout << "\n";
    std::cout << "A: (" << A.x() << ", " << A.y() << ", " << A.z() << ")" << std::endl;
    std::cout << "B: (" << B.x() << ", " << B.y() << ", " << B.z() << ")" << std::endl;
    std::cout << "AB: (" << AB.x() << ", " << AB.y() << ", " << AB.z() << ")" << std::endl;
    std::cout << "C: (" << C.x() << ", " << C.y() << ", " << C.z() << ")" << std::endl;

    fmat4 modelMatrix = translation_matrix<float>(5.0f, 6.0f, 7.0f); // Create a translation matrix that translates by (5, 6, 7)
    fpoint3 localPos(1.0f, 2.0f, 3.0f); // Define a local position (point) with coordinates (1, 2, 3)
    fpoint3 worldPos = modelMatrix * localPos; // Transform the local position to world space by multiplying it with the model matrix

    std::cout << "\n";
    std::cout << "Model Matrix (Translation by (5, 6, 7)):" << std::endl;
    for (size_t i = 0; i < modelMatrix.rows(); ++i) {
        for (size_t j = 0; j < modelMatrix.cols(); ++j) {
            std::cout << modelMatrix(i, j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Local Position: (" << localPos.x() << ", " << localPos.y() << ", " << localPos.z() << ")" << std::endl;
    std::cout << "World Position: (" << worldPos.x() << ", " << worldPos.y() << ", " << worldPos.z() << ")" << std::endl;

    {
    std::cout << "\n";
    // Define a vector u along the x-axis
    fvec3 u(1.0f, 0.0f, 0.0f); 
    // Define a vector v along the y-axis
    fvec3 v(0.0f, 1.0f, 0.0f); 
    // Compute the cross product of u and v, which should yield a vector along the z-axis
    fvec3 crossProduct = u.cross(v); 
    // Compute the dot product of u and v, which should be 0 since they are perpendicular
    float dotProduct = u.dot(v); 
    std::cout << "u: (" << u.x() << ", " << u.y() << ", " << u.z() << ")" << std::endl;
    std::cout << "v: (" << v.x() << ", " << v.y() << ", " << v.z() << ")" << std::endl;
    std::cout << "u x v (cross product): (" << crossProduct.x() << ", " << crossProduct.y() << ", " << crossProduct.z() << ")" << std::endl;
    std::cout << "u . v (dot product): " << dotProduct << std::endl;
    }

    {
        fpoint3 p(1.0f, .0f, .0f); // Create a point p at coordinates (1, 0, 0)
        fvec3 velocity(0.5f, 0.0f, 0.0f); // Define a velocity vector along the x-axis
        float deltaTime = 1.0f; // Define a time step of 1 second
        // Update the point's position by adding the velocity scaled by the time step
        for (int second = 0; second < 5; second += deltaTime) {
            p = p + velocity * deltaTime; // Move the point along the x-axis over time
            std::cout << "Time: " << second + deltaTime << "s, Position: (" << p.x() << ", " << p.y() << ", " << p.z() << ")" << std::endl;
        }

    }

    basepoint3<double> p1(1.0, 2.0, 3.0); // Create a point p1 at coordinates (1, 2, 3)
    basepoint3<double> p2(4.0, 5.0, 6.0); // Create another point p2 at coordinates (4, 5, 6)
    dvec3 direction = p2 - p1; // Compute the direction vector from p1 to p2 by subtracting the two points
    std::cout << "\n";  
    std::cout << "p1: (" << p1.x() << ", " << p1.y() << ", " << p1.z() << ")" << std::endl;
    std::cout << "p2: (" << p2.x() << ", " << p2.y() << ", " << p2.z() << ")" << std::endl;
    std::cout << "Direction from p1 to p2: (" << direction.x() << ", " << direction.y() << ", " << direction.z() << ")" << std::endl;


    return 0;
}
