#ifndef __mesh_renderer_h__
#define __mesh_renderer_h__

#include "mesh_explicit.h"

namespace btm_framework
{
    // A simple mesh renderer that uses OpenGL immediate mode to render the triangles of a MeshExplicit.
    // This is a very basic implementation for demonstration purposes. In a real application, 
    // we will use modern OpenGL techniques such as vertex buffers and shaders for better performance and flexibility.
    // The MeshRenderer class is templated on the type T, which should match the type used in MeshExplicit (e.g., float).
    // The render function iterates over the triangles in the mesh, retrieves the vertex positions, and issues OpenGL commands to draw them.
    template <typename T>
    class MeshRenderer
    {
        const MeshExplicit<T>& mesh;
    public:
        MeshRenderer(const MeshExplicit<T>& mesh) : mesh(mesh) {}

        void render () const
        {
            // For simplicity, we will just render the triangles in a solid color.
            basevector<float, 3> blue(0.0f, 0.0f, 1.0f);
            basevector<float, 3> red(1.f, 0.f, 0.f);
            basevector<float, 3> colors[] = { blue, red };
            glBegin(GL_TRIANGLES);
            for (int i = 0; i < mesh.triangle_count(); ++i)
            {
                auto tri = mesh.get_triangle(i);

                // Retrieve vertex positions for the triangle
                // We assume that the vertex indices in the triangle are valid and correspond to vertices in the mesh.
                // here we use the themplate type T to retrieve the vertex positions, which should be consistent with the type used in MeshExplicit!
                basevector<T, 3> v1 = (mesh.get_vertex(tri.v0)).position;
                basevector<T, 3> v2 = (mesh.get_vertex(tri.v1)).position;
                basevector<T, 3> v3 = (mesh.get_vertex(tri.v2)).position;
                glColor3f(colors[i % 2].x(), colors[i % 2].y(), colors[i % 2].z());
                // Issue OpenGL commands to draw the triangle using the vertex positions.
                // In a real application, we would also set up vertex normals, texture coordinates, and other attributes as needed.
                // Here we are using the x and y components of the vertex positions for 2D rendering.
                // for OpenGL drawing we use float type, so we need to cast the vertex positions to float if T is not already float.
                glVertex2f(static_cast<float>(v1.x()), static_cast<float>(v1.y()));
                glVertex2f(static_cast<float>(v2.x()), static_cast<float>(v2.y()));
                glVertex2f(static_cast<float>(v3.x()), static_cast<float>(v3.y()));
            }
            glEnd();
        }
    
    };

} // namespace btm_framework

#endif //__mesh_renderer_h__
