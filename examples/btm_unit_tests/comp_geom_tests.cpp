#include <iostream>
#include <chrono>

#include "vector.h"
#include "barycentric.h"
#include "triangle.h"
#include "aabb.h"
#include "closest_point.h"
#include "line.h"
#include "plane.h"
#include "predicates.h"
#include "bvh.h"
#include "tetrahedron.h"
#include "bvh_tetra.h"
#include "tetra_shape_functions.h"

#include "validators.h"

using namespace btm;

// Test function for dvec3 operations
static void testVec3Operations() {
    std::cout << "Running dvec3 operations tests..." << std::endl;
    dvec3 a{ 1, 2, 3 };
    dvec3 b{ 4, 5, 6 };
    // Test dot product
    double dotProduct = dot(a, b);
    validate_results(dotProduct, 32.0, "dot product");
    // Test cross product
    dvec3 crossProduct = cross(a, b);
    validate_results(crossProduct, dvec3{ -3.0, 6.0, -3.0 }, "cross product");
    // Test length and normalization
    double lengthA = a.length();
    validate_results(lengthA, std::sqrt(14.0), "length");
    dvec3 normalizedA = a.norm();
    validate_results(normalizedA.length(), 1.0, "normalization");
    // Test projection
    dvec3 projection = project(a, b);
    validate_results(dot(projection, cross(a, b)), 0.0, "projection");
    std::cout << "All dvec3 operations tests completed.\n\n";
}

static void testPoint3Operations() {
    std::cout << "Running dpoint3 operations tests..." << std::endl;
    dpoint3 a{ 1, 2, 3 };
    dpoint3 b{ 4, 6, 3 };
    // Test distance
    double dist = distance(a, b);
    validate_results(dist, 5.0, "distance");
    // Test point - point = vector
    dvec3 v = a - b;
    validate_results(v, dvec3{ -3.0, -4.0, 0.0 }, "point - point = vector");
    std::cout << "All dpoint3 operations tests completed.\n\n";
}

void testBarycentricCoordinates() {
    std::cout << "Running barycentric coordinates tests..." << std::endl;
    dpoint3 a{ 0,0,0 };
    dpoint3 b{ 1,0,0 };
    dpoint3 c{ 0,1,0 };
    // Test center of triangle
    dpoint3 p{ 1.0 / 3, 1.0 / 3, 0 };
    auto bc = barycentric(p, a, b, c);
    validate_results(bc.u, 1.0 / 3, "barycentric u");
    validate_results(bc.v, 1.0 / 3, "barycentric v");
    validate_results(bc.w, 1.0 / 3, "barycentric w");
    // Test inside triangle
    auto bcInside = barycentric(dpoint3{ 0.25,0.25,0 }, a, b, c);
    bool isInside = isInsideTriangle(bcInside);
    validate_results(isInside, true, "point inside triangle");
    std::cout << "All barycentric coordinates tests completed.\n\n";
}


static void testSegmentOperations() {
    std::cout << "Running segment operations tests..." << std::endl;
    segment s{ dpoint3{0,0,0}, dpoint3{2,0,0} };
    // Test length
    double len = length(s);
    validate_results(len, 2.0, "segment length");
    // Test evaluation at t=0.5
    dpoint3 eval = evaluate(s, 0.5);
    validate_results(eval, dpoint3{ 1.0, 0.0, 0.0 }, "segment evaluation");
    std::cout << "All segment operations tests completed.\n\n";
}

static void testPlaneOperations() {
    std::cout << "Running plane operations tests..." << std::endl;
    Plane<double> plane = Plane<double>::fromPointNormal(dpoint3{ 0,0,0 }, dvec3{ 0,1,0 });
    // Test signed distance
    double dist = signedDistance(plane, dpoint3{ 0,5,0 });
    validate_results(dist, 5.0, "plane signed distance");
    // Test projection
    dpoint3 proj = project(plane, dpoint3{ 0,5,0 });
    validate_results(proj, dpoint3{ 0,0,0 }, "plane projection");
    std::cout << "All plane operations tests completed.\n\n";
}

static void testTriangleOperations() {
    std::cout << "Running triangle operations tests..." << std::endl;
    Triangle t{ dpoint3{0,0,0}, dpoint3{1,0,0}, dpoint3{0,1,0} };
    // Test area
    double a = area(t);
    validate_results(a, 0.5, "triangle area");
    // Test normal
    dvec3 n = normal(t);
    validate_results(n, dvec3{ 0.0, 0.0, 1.0 }, "triangle normal");
    std::cout << "All triangle operations tests completed.\n\n";
}

static void testClosestPointOperations() {
    std::cout << "Running closest point operations tests..." << std::endl;
    segment s{ dpoint3{0,0,0}, dpoint3{10,0,0} };
    dpoint3 p{ 3,4,0 };
    dpoint3 cp = closestPoint(s, p);
    validate_results(cp, dpoint3{ 3.0, 0.0, 0.0 }, "closest point on segment");
    Triangle t{ dpoint3{0,0,0}, dpoint3{1,0,0}, dpoint3{0,1,0} };
    dpoint3 p2{ 0.2f, 0.2f, 1.0f };
    dpoint3 cp2 = closestPoint(t, p2);
    validate_results(cp2.z(), 0.0f, "closest point on triangle");
    std::cout << "All closest point operations tests completed.\n\n";
}

static void testOrientationPredicates() {
    std::cout << "Running orientation predicates tests..." << std::endl;
    dpoint3 a{ 0,0,0 };
    dpoint3 b{ 1,0,0 };
    dpoint3 c{ 0,1,0 };
    double orient2DResult = orient2D(a, b, c);
    validate_results(orient2DResult > 0.0, true, "orient2D positive");
    dpoint3 d{ 0,0,1 };
    double orient3DResult = orient3D(a, b, c, d);
    validate_results(orient3DResult != 0.0, true, "orient3D non-zero");
    std::cout << "All orientation predicates tests completed.\n\n";
}

static void testPointInTriangle() {
    std::cout << "Running point in triangle tests..." << std::endl;
    Triangle t{ dpoint3{0,0,0}, dpoint3{1,0,0}, dpoint3{0,1,0} };
    bool inside = pointInTriangle(dpoint3{ 0.2f,0.2f,0 }, t.a, t.b, t.c);
    validate_results(inside, true, "point inside triangle");
    std::cout << "All point in triangle tests completed.\n\n";
}

static void testAABBOperations() {
    std::cout << "Running AABB operations tests..." << std::endl;
    AABB a{ dpoint3{0,0,0}, dpoint3{1,1,1} };
    AABB b{ dpoint3{0.5f,0.5f,0.5f}, dpoint3{2,2,2} };
    bool doesIntersect = intersects(a, b);
    validate_results(doesIntersect, true, "AABB intersection");
    AABB m = merge(a, b);
    validate_results(m.max_p.x(), 2.0f, "AABB merge max x");
    validate_results(m.min_p.x(), 0.0f, "AABB merge min x");
    double sa = surfaceArea(a);
    validate_results(sa, 6.0f, "AABB surface area");
    std::cout << "All AABB operations tests completed.\n\n";
}

static void testBVHOperations() {
    std::cout << "Running BVH operations tests..." << std::endl;
    std::vector<Triangle<double>> tris = {
        { dpoint3{0,0,0}, dpoint3{1,0,0}, dpoint3{0,1,0} }
    };
    BVH bvh = build(tris);
    validate_results(!bvh.nodes.empty(), true, "BVH build nodes not empty");
    validate_results(bvh.triangles.size(), 1, "BVH build triangle count");
    ray _ray{ dpoint3{0.2f,0.2f,1}, dvec3{0,0,-1} };
    HitInfo<double> hit;
    bool hitResult = intersect(bvh, _ray, hit);
    validate_results(hitResult, true, "BVH ray intersection");
    validate_results(hit.triangleIndex, 0, "BVH hit triangle index");
    validate_results(hit.position.z(), 0.0f, "BVH hit position z");
    std::cout << "All BVH operations tests completed.\n\n";
}

static void testTetraVolumeAndOrientation() {
    std::cout << "Running tetrahedron volume and orientation tests..." << std::endl;
    Tetrahedron<double> t{
        basepoint3<double>{0,0,0},
        basepoint3<double>{1,0,0},
        basepoint3<double>{0,1,0},
        basepoint3<double>{0,0,1}
    };
    float v6 = signedVolume6(t);
    validate_results(v6 != 0.0f, true, "tetrahedron signed volume non-zero");
    validate_results(volume(t), 1.0f / 6.0f, "tetrahedron volume");
    validate_results(orient3D(t), v6, "tetrahedron orientation");
    std::cout << "All tetrahedron volume and orientation tests completed.\n\n";
}

static void testTetrahedronPointContainment() {
    std::cout << "Running tetrahedron point containment tests..." << std::endl;
    Tetrahedron<double> t{
        basepoint3<double>{0,0,0},
        basepoint3<double>{1,0,0},
        basepoint3<double>{0,1,0},
        basepoint3<double>{0,0,1}
    };
    bool inside = containsPoint(t, basepoint3<double>{ 0.1f, 0.2f, 0.3f });
    validate_results(inside, true, "point inside tetrahedron");
    bool outside = containsPoint(t, basepoint3<double>{ 1.0f, 1.0f, 1.0f });
    validate_results(outside, false, "point outside tetrahedron");
    std::cout << "All tetrahedron point containment tests completed.\n\n";
}

static void testTetraClosestPoint() {
    std::cout << "Running tetrahedron closest point tests..." << std::endl;
    Tetrahedron<double> t{
        basepoint3<double>{0,0,0},
        basepoint3<double>{1,0,0},
        basepoint3<double>{0,1,0},
        basepoint3<double>{0,0,1}
    };
    basepoint3<double> p{ 0.1f, 0.2f, 0.3f };
    basepoint3<double> cp = closestPoint(t, p);
    validate_results(containsPoint(t, cp), true, "closest point inside tetrahedron");
    std::cout << "All tetrahedron closest point tests completed.\n\n";
}

static void testTetraBarycentricCoordinates() {
    std::cout << "Running tetrahedron barycentric coordinates tests..." << std::endl;
    Tetrahedron<double> t{
        basepoint3<double>{0,0,0},
        basepoint3<double>{1,0,0},
        basepoint3<double>{0,1,0},
        basepoint3<double>{0,0,1}
    };

    Barycentric4<double> b0 = barycentric(t, t.a);
    validate_results(b0.w0, 1.0, "vertex a barycentric w0");
    validate_results(b0.w1, 0.0, "vertex a barycentric w1");
    validate_results(b0.w2, 0.0, "vertex a barycentric w2");
    validate_results(b0.w3, 0.0, "vertex a barycentric w3");

    // Test barycentric coordinates at the center of the tetrahedron
    Barycentric4<double> bc = barycentric(t, basepoint3<double>{ 0.25f, 0.25f, 0.25f });
    validate_results(bc.w0, 0.25, "center point barycentric w0");
    validate_results(bc.w1, 0.25, "center point barycentric w1");
    validate_results(bc.w2, 0.25, "center point barycentric w2");
    validate_results(bc.w3, 0.25, "center point barycentric w3");
    std::cout << "All tetrahedron barycentric coordinates tests completed.\n\n";
}

static void testBvh_tetra_build_and_ray_intersection() {
    std::cout << "Running BVH_Tetra build and ray intersection tests..." << std::endl;
    // Simple tetrahedron
    std::vector<Tetrahedron<double>> tets = {
        {
            basepoint3<double>{0,0,0},
            basepoint3<double>{1,0,0},
            basepoint3<double>{0,1,0},
            basepoint3<double>{0,0,1}
        }
    };
    BVH_Tetra bvh = build(tets);
    validate_results(!bvh.nodes.empty(), true, "BVH_Tetra build nodes not empty");
    validate_results(bvh.tets.size(), 1, "BVH_Tetra build tetrahedron count");
    ray<double> ray{ basepoint3<double>{0.2f,0.2f,2.0f}, dvec3{0,0,-1} };
    TetraHitInfo<double> hit;
    bool hitResult = intersect(bvh, ray, hit);
    validate_results(hitResult, true, "BVH_Tetra ray intersection");
    validate_results(hit.tetraIndex, 0, "BVH_Tetra hit tetrahedron index");
    validate_results(hit.position.z(), 0.6, "BVH_Tetra hit position z");
//     REQUIRE(hit.position.z == Approx(0.6f).margin(1e-4f));
    std::cout << "All BVH_Tetra build and ray intersection tests completed.\n\n";
}

static void testBvh_tetra_miss_test() {
    std::cout << "Running BVH_Tetra miss test..." << std::endl;
    std::vector<Tetrahedron<double>> tets = {
        {
            basepoint3<double>{0,0,0},
            basepoint3<double>{1,0,0},
            basepoint3<double>{0,1,0},
            basepoint3<double>{0,0,1}
        }
    };
    BVH_Tetra bvh = build(tets);
    ray<double> ray{ basepoint3<double>{5,5,5}, dvec3{-1,0,0} };
    TetraHitInfo<double> hit;
    bool hitResult = intersect(bvh, ray, hit);
    validate_results(hitResult, false, "BVH_Tetra ray miss");
    std::cout << "All BVH_Tetra miss tests completed.\n\n";
}

static void testBvh_tetra_multiple_tetrahedra() {
    std::cout << "Running BVH_Tetra multiple tetrahedra tests..." << std::endl;
    std::vector<Tetrahedron<double>> tets = {
        {
            basepoint3<double>{0,0,0},
            basepoint3<double>{1,0,0},
            basepoint3<double>{0,1,0},
            basepoint3<double>{0,0,1}
        },
        {
            basepoint3<double>{1,1,1},
            basepoint3<double>{2,1,1},
            basepoint3<double>{1,2,1},
            basepoint3<double>{1,1,2}
        }
    };
    BVH_Tetra bvh = build(tets);
    validate_results(bvh.tets.size(), 2, "BVH_Tetra multiple tetrahedra count");
    ray<double> ray{ basepoint3<double>{0.5f,0.5f,-1.0f}, dvec3{0,0,1} };
    TetraHitInfo<double> hit;
    bool hitResult = intersect(bvh, ray, hit);
    validate_results(hitResult, true, "BVH_Tetra multiple tetrahedra ray intersection");
    validate_results(hit.tetraIndex, 0, "BVH_Tetra multiple tetrahedra hit index");
    std::cout << "All BVH_Tetra multiple tetrahedra tests completed.\n\n";
}

static void testBvh_tetra_edge_case() {
    std::cout << "Running BVH_Tetra edge case tests..." << std::endl;
    std::vector<Tetrahedron<double>> tets = {
        {
            basepoint3<double>{0,0,0},
            basepoint3<double>{1,0,0},
            basepoint3<double>{0,1,0},
            basepoint3<double>{0,0,1}
        }
    };
    BVH_Tetra bvh = build(tets);
    ray<double> ray{ basepoint3<double>{0.5f,0.5f,1.0f}, dvec3{0,0,-1} };
    TetraHitInfo<double> hit;
    bool hitResult = intersect(bvh, ray, hit);
    validate_results(hitResult, true, "BVH_Tetra edge case ray intersection");
    validate_results(hit.tetraIndex, 0, "BVH_Tetra edge case hit index");
    std::cout << "All BVH_Tetra edge case tests completed.\n\n";
}


/*
Running BVH_Tetra performance tests...
BVH_Tetra build time for 10000 tetrahedra: 0.488029 seconds.
Test failed : BVH_Tetra performance ray intersection
Expected : 1
Got : 0
Test failed : BVH_Tetra performance hit index valid
Expected : 1
Got : 0
BVH_Tetra intersection time : 3.31e-05 seconds.
All BVH_Tetra performance tests completed.
*/


static void testBvh_tetra_performance() {
    std::cout << "Running BVH_Tetra performance tests..." << std::endl;
    const int numTets = 10000;
    std::vector<Tetrahedron<double>> tets;
    tets.reserve(numTets);
    for (int i = 0; i < numTets; ++i) {
        double x = static_cast<double>(i % 100);
        double y = static_cast<double>((i / 100) % 100);
        double z = static_cast<double>(i / 10000);
        tets.push_back({
            basepoint3<double>{x,y,z},
            basepoint3<double>{x + 1,y,z},
            basepoint3<double>{x,y + 1,z},
            basepoint3<double>{x,y,z + 1}
            });
    }
    auto startBuild = std::chrono::high_resolution_clock::now();
    BVH_Tetra bvh = build(tets);
    auto endBuild = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> buildDuration = endBuild - startBuild;
    std::cout << "BVH_Tetra build time for " << numTets << " tetrahedra: "
        << buildDuration.count() << " seconds.\n";
    ray<double> ray{ basepoint3<double>{50.5,50.5,-10.0}, dvec3{0,0,1} };
    TetraHitInfo<double> hit;
    auto startIntersect = std::chrono::high_resolution_clock::now();
    bool hitResult = intersect(bvh, ray, hit);
    auto endIntersect = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> intersectDuration = endIntersect - startIntersect;
    validate_results(hitResult, true, "BVH_Tetra performance ray intersection");
    validate_results(hit.tetraIndex >= 0 && hit.tetraIndex < numTets, true, "BVH_Tetra performance hit index valid");
    std::cout << "BVH_Tetra intersection time: "
        << intersectDuration.count() << " seconds.\n";
    std::cout << "All BVH_Tetra performance tests completed.\n\n";
}

static void testBvh_tetra_randomized() {
    std::cout << "Running BVH_Tetra randomized tests..." << std::endl;
    const int numTets = 1000;
    std::vector<Tetrahedron<double>> tets;
    tets.reserve(numTets);
    for (int i = 0; i < numTets; ++i) {
        double x = static_cast<double>(rand() % 100);
        double y = static_cast<double>(rand() % 100);
        double z = static_cast<double>(rand() % 100);
        tets.push_back({
            basepoint3<double>{x,y,z},
            basepoint3<double>{x + 1,y,z},
            basepoint3<double>{x,y + 1,z},
            basepoint3<double>{x,y,z + 1}
            });
    }
    BVH_Tetra bvh = build(tets);
    for (int i = 0; i < 10; ++i) {
        double x = static_cast<double>(rand() % 100);
        double y = static_cast<double>(rand() % 100);
        double z = static_cast<double>(rand() % 100);
        ray<double> ray{ basepoint3<double>{x,y,-10.0}, dvec3{0,0,1} };
        TetraHitInfo<double> hit;
        bool hitResult = intersect(bvh, ray, hit);
        if (hitResult) {
            validate_results(hit.tetraIndex >= 0 && hit.tetraIndex < numTets, true, "BVH_Tetra randomized hit index valid");
        }
    }
    std::cout << "All BVH_Tetra randomized tests completed.\n\n";
}

static void testTetraShapeFunctions() {
    std::cout << "Running tetrahedron shape functions tests..." << std::endl;
    Tetrahedron<double> t{
        basepoint3<double>{0,0,0},
        basepoint3<double>{1,0,0},
        basepoint3<double>{0,1,0},
        basepoint3<double>{0,0,1}
    };
    basepoint3<double> p{ 0.2f, 0.3f, 0.1f };
    Barycentric4<double> N = shapeFunctions(t, p);
    double sum = N.w0 + N.w1 + N.w2 + N.w3;
    validate_results(sum, 1.0, "tetrahedron shape functions sum to 1");
    ShapeFunctionGradients<double> g = shapeFunctionGradients(t);
    basevec3<double> gradSum = g.gradN0 + g.gradN1 + g.gradN2 + g.gradN3;
    validate_results(gradSum.x(), 0.0, "tetrahedron shape function gradients sum x");
    validate_results(gradSum.y(), 0.0, "tetrahedron shape function gradients sum y");
    validate_results(gradSum.z(), 0.0, "tetrahedron shape function gradients sum z");
    std::cout << "All tetrahedron shape functions tests completed.\n\n";
}

static void testTetraShapeFunctionsAtVertices() {
    std::cout << "Running tetrahedron shape functions at vertices tests..." << std::endl;
    Tetrahedron<double> t{
        basepoint3<double>{0,0,0},
        basepoint3<double>{1,0,0},
        basepoint3<double>{0,1,0},
        basepoint3<double>{0,0,1}
    };
    Barycentric4<double> N_a = shapeFunctions(t, t.a);
    validate_results(N_a.w0, 1.0, "shape function at vertex a w0");
    validate_results(N_a.w1, 0.0, "shape function at vertex a w1");
    validate_results(N_a.w2, 0.0, "shape function at vertex a w2");
    validate_results(N_a.w3, 0.0, "shape function at vertex a w3");
    Barycentric4<double> N_b = shapeFunctions(t, t.b);
    validate_results(N_b.w0, 0.0, "shape function at vertex b w0");
    validate_results(N_b.w1, 1.0, "shape function at vertex b w1");
    validate_results(N_b.w2, 0.0, "shape function at vertex b w2");
    validate_results(N_b.w3, 0.0, "shape function at vertex b w3");
    Barycentric4<double> N_c = shapeFunctions(t, t.c);
    validate_results(N_c.w0, 0.0, "shape function at vertex c w0");
    validate_results(N_c.w1, 0.0, "shape function at vertex c w1");
    validate_results(N_c.w2, 1.0, "shape function at vertex c w2");
    validate_results(N_c.w3, 0.0, "shape function at vertex c w3");
    Barycentric4<double> N_d = shapeFunctions(t, t.d);
    validate_results(N_d.w0, 0.0, "shape function at vertex d w0");
    validate_results(N_d.w1, 0.0, "shape function at vertex d w1");
    validate_results(N_d.w2, 0.0, "shape function at vertex d w2");
    validate_results(N_d.w3, 1.0, "shape function at vertex d w3");
    std::cout << "All tetrahedron shape functions at vertices tests completed.\n\n";
}

static void testTetraShapeFunctionGradients() {
    std::cout << "Running tetrahedron shape function gradients tests..." << std::endl;
    Tetrahedron<double> t{
        basepoint3<double>{0,0,0},
        basepoint3<double>{1,0,0},
        basepoint3<double>{0,1,0},
        basepoint3<double>{0,0,1}
    };
    ShapeFunctionGradients<double> g = shapeFunctionGradients(t);
    basevec3<double> gradSum = g.gradN0 + g.gradN1 + g.gradN2 + g.gradN3;
    validate_results(gradSum.x(), 0.0, "tetrahedron shape function gradients sum x");
    validate_results(gradSum.y(), 0.0, "tetrahedron shape function gradients sum y");
    validate_results(gradSum.z(), 0.0, "tetrahedron shape function gradients sum z");
    std::cout << "All tetrahedron shape function gradients tests completed.\n\n";
}



int testCompGeom() {
    // Initialize Catch2
    int result = 0;// Catch::Session().run(argc, argv);
    testVec3Operations();
    testPoint3Operations();
    testBarycentricCoordinates();
    testSegmentOperations();
    testPlaneOperations();
    testTriangleOperations();
    testClosestPointOperations();
    testOrientationPredicates();
    testPointInTriangle();
    testAABBOperations();
    testBVHOperations();

    testTetraVolumeAndOrientation();
    testTetrahedronPointContainment();
    testTetraClosestPoint();
    testTetraBarycentricCoordinates();

    testBvh_tetra_build_and_ray_intersection();
    testBvh_tetra_miss_test();
    testBvh_tetra_multiple_tetrahedra();
    testBvh_tetra_edge_case();
    testBvh_tetra_performance();
    testBvh_tetra_randomized();
    testTetraShapeFunctions();
    testTetraShapeFunctionsAtVertices();
    testTetraShapeFunctionGradients();


    return result;
}