/********************************************************************
 * Copyright (C) 2015 Liangliang Nan <liangliang.nan@gmail.com>
 * https://3d.bk.tudelft.nl/liangliang/
 *
 * This file is part of Easy3D. If it is useful in your research/work,
 * I would be grateful if you show your appreciation by citing it:
 * ------------------------------------------------------------------
 *      Liangliang Nan.
 *      Easy3D: a lightweight, easy-to-use, and efficient C++ library
 *      for processing and rendering 3D data.
 *      Journal of Open Source Software, 6(64), 3255, 2021.
 * ------------------------------------------------------------------
 *
 * Easy3D is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3
 * as published by the Free Software Foundation.
 *
 * Easy3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************/

#include <easy3d/viewer/viewer.h>
#include <easy3d/renderer/camera.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/drawable_points.h>
#include <easy3d/renderer/drawable_triangles.h>
#include <easy3d/core/types.h>
#include <easy3d/util/resource.h>
#include <easy3d/util/initializer.h>


using namespace easy3d;


// Drawables are typically for rendering 3D models (e.g., point clouds, meshes, graphs) that are loaded from files or
// generated by some algorithms.
// The use of drawables for visualization is quite flexible. Drawables are normally attached to a 3D model. For example,
// you can attach a TrianglesDrawable to a surface mesh to visualize its surface and a PointsDrawable to visualize its
// vertices.
// Easy3D also allows visualizing stand-alone drawables (without creating a model).
//
// This example shows how to
//      - visualize 3D data without explicitly defining a model (i.e., rendering drawables directly);
//          * a set of triangles;
//          * a set of points;
//          * a set of lines.
//		- create a drawable for a specific rendering purpose;
//		- use the viewer to visualize drawables.

#if 1   // use the built-in drawables of Easy3D.

int main(int argc, char **argv) {
    // Initialize Easy3D.
    initialize();

    //-------------------------------------------------------------

    // Create the default Easy3D viewer.
    // Note: a viewer must be created before creating any drawables.
    Viewer viewer("Tutorial_301_Drawables");

    //-------------------------------------------------------------

    // We visualize the "bunny".

    // The coordinates of the vertices.
    const std::vector<vec3> &points = resource::bunny_vertices;
    // The indices represent how the vertices are connected to form triangles. The "bunny" is a triangle mesh, and thus
    // each consecutive three indices represent a triangle.
    const std::vector<unsigned int> &indices = resource::bunny_indices;

    //-------------------------------------------------------------
    // Create a TrianglesDrawable to visualize the surface of the "bunny".
    // For visualization, the point positions and the vertex indices of the faces have to be sent to the GPU.
    auto surface = new TrianglesDrawable("faces");
    // Upload the vertex positions of the surface to the GPU.
    surface->update_vertex_buffer(points);
    // Upload the vertex indices of the surface to the GPU.
    surface->update_element_buffer(indices);
    // Add the drawable to the viewer
    viewer.add_drawable(surface);

    //-------------------------------------------------------------
    // Create a PointsDrawable to visualize the vertices of the "bunny".
    // Only the vertex positions have to be sent to the GPU for visualization.
    auto vertices = new PointsDrawable("vertices");
    // Upload the vertex positions to the GPU.
    vertices->update_vertex_buffer(points);
    // Set a color for the vertices (here we want a red color).
    vertices->set_uniform_coloring(vec4(1.0f, 0.0f, 0.0f, 1.0f));  // RBGA
    // Three options are available for visualizing points:
    //      - PLAIN: plain points (i.e., each point is a square on the screen);
    //      - SPHERE: each point is visualized a sphere;
    //      - SURFEL: each point is visualized an oriented disk.
    // In this example, let's render the vertices as spheres.
    vertices->set_impostor_type(PointsDrawable::SPHERE);
    // Set the vertices size (here 10 pixels).
    vertices->set_point_size(10);
    // Add the drawable to the viewer
    viewer.add_drawable(vertices);

    //-------------------------------------------------------------
    // Create a LinesDrawable to visualize the bounding box of the "bunny".

    // Compute the bounding box.
    auto bbox_drawable = new LinesDrawable("bbox");
    const Box3 &box = geom::bounding_box<Box3, std::vector<vec3> >(points);
    float xmin = box.min_coord(0);
    float xmax = box.max_coord(0);
    float ymin = box.min_coord(1);
    float ymax = box.max_coord(1);
    float zmin = box.min_coord(2);
    float zmax = box.max_coord(2);
    // The eight vertices of the bounding box.
    const std::vector<vec3> bbox_points = {
            vec3(xmin, ymin, zmax), vec3(xmax, ymin, zmax),
            vec3(xmin, ymax, zmax), vec3(xmax, ymax, zmax),
            vec3(xmin, ymin, zmin), vec3(xmax, ymin, zmin),
            vec3(xmin, ymax, zmin), vec3(xmax, ymax, zmin)
    };
    // The vertex indices of the twelve edges of the bounding box (each consecutive two numbers represent an edge).
    const std::vector<unsigned int> bbox_indices = {
            0, 1, 2, 3, 4, 5, 6, 7,
            0, 2, 4, 6, 1, 3, 5, 7,
            0, 4, 2, 6, 1, 5, 3, 7
    };
    // Upload the vertex positions of the bounding box to the GPU.
    bbox_drawable->update_vertex_buffer(bbox_points);
    // Upload the vertex indices of the bounding box to the GPU.
    bbox_drawable->update_element_buffer(bbox_indices);
    // Set a color for the edges of the bounding box (here we want a blue color).
    bbox_drawable->set_uniform_coloring(vec4(0.0f, 0.0f, 1.0f, 1.0f));    // r, g, b, a
    // Set the width of the edges (here 5 pixels).
    bbox_drawable->set_line_width(5.0f);
    // Add the drawable to the viewer
    viewer.add_drawable(bbox_drawable);

    //-------------------------------------------------------------

    // Make sure everything is within the visible region of the viewer.
    viewer.fit_screen();

    // Run the viewer
    return viewer.run();
}

#elif 1 //  use the built-in drawables of Easy3D, but we provide customized update functions

int main(int argc, char **argv) {
    // Initialize Easy3D.
    initialize();

    //-------------------------------------------------------------

    // Create the default Easy3D viewer.
    // Note: a viewer must be created before creating any drawables.
    Viewer viewer("Tutorial_301_Drawables");

    //-------------------------------------------------------------

    // We visualize the "bunny".

    // The coordinates of the vertices.
    const std::vector<vec3> &points = resource::bunny_vertices;
    // The indices represent how the vertices are connected to form triangles. The "bunny" is a triangle mesh, and thus
    // each consecutive three indices represent a triangle.
    const std::vector<unsigned int> &indices = resource::bunny_indices;

    //-------------------------------------------------------------
    // Create a TrianglesDrawable to visualize the surface of the "bunny".
    // For visualization, the point positions and the vertex indices of the faces have to be sent to the GPU.
    auto surface = new TrianglesDrawable("faces");
    surface->set_update_func([&points, &indices](Model* m, Drawable* d) {
        // Upload the vertex positions of the surface to the GPU.
        d->update_vertex_buffer(points);
        // Upload the vertex indices of the surface to the GPU.
        d->update_element_buffer(indices);
    });

    // Add the drawable to the viewer
    viewer.add_drawable(surface);

    //-------------------------------------------------------------
    // Create a PointsDrawable to visualize the vertices of the "bunny".
    // Only the vertex positions have to be sent to the GPU for visualization.
    auto vertices = new PointsDrawable("vertices");
    vertices->set_update_func([&points](Model* m, Drawable* d) {
        // Upload the vertex positions to the GPU.
        d->update_vertex_buffer(points);
    });

    // Set a color for the vertices (here we want a red color).
    vertices->set_uniform_coloring(vec4(1.0f, 0.0f, 0.0f, 1.0f));  // r, g, b, a
    // Three options are available for visualizing points:
    //      - PLAIN: plain points (i.e., each point is a square on the screen);
    //      - SPHERE: each point is visualized a sphere;
    //      - SURFEL: each point is visualized an oriented disk.
    // In this example, let's render the vertices as spheres.
    vertices->set_impostor_type(PointsDrawable::SPHERE);
    // Set the vertices size (here 10 pixels).
    vertices->set_point_size(10);
    // Add the drawable to the viewer
    viewer.add_drawable(vertices);

    //-------------------------------------------------------------
    // Create a LinesDrawable to visualize the bounding box of the "bunny".

    // Compute the bounding box.
    auto bbox_drawable = new LinesDrawable("bbox");
    bbox_drawable->set_update_func([&points](Model* m, Drawable* d) {
        const Box3 &box = geom::bounding_box<Box3, std::vector<vec3> >(points);
        float xmin = box.min_coord(0);
        float xmax = box.max_coord(0);
        float ymin = box.min_coord(1);
        float ymax = box.max_coord(1);
        float zmin = box.min_coord(2);
        float zmax = box.max_coord(2);
        // The eight vertices of the bounding box.
        const std::vector<vec3> bbox_points = {
                vec3(xmin, ymin, zmax), vec3(xmax, ymin, zmax),
                vec3(xmin, ymax, zmax), vec3(xmax, ymax, zmax),
                vec3(xmin, ymin, zmin), vec3(xmax, ymin, zmin),
                vec3(xmin, ymax, zmin), vec3(xmax, ymax, zmin)
        };
        // The vertex indices of the twelve edges of the bounding box (each consecutive two numbers represent an edge).
        const std::vector<unsigned int> bbox_indices = {
                0, 1, 2, 3, 4, 5, 6, 7,
                0, 2, 4, 6, 1, 3, 5, 7,
                0, 4, 2, 6, 1, 5, 3, 7
        };

        // Upload the vertex positions of the bounding box to the GPU.
        d->update_vertex_buffer(bbox_points);
        // Upload the vertex indices of the bounding box to the GPU.
        d->update_element_buffer(bbox_indices);
    });

    // Set a color for the edges of the bounding box (here we want a blue color).
    bbox_drawable->set_uniform_coloring(vec4(0.0f, 0.0f, 1.0f, 1.0f));    // r, g, b, a
    // Set the width of the edges (here 5 pixels).
    bbox_drawable->set_line_width(5.0f);
    // Add the drawable to the viewer
    viewer.add_drawable(bbox_drawable);

    //-------------------------------------------------------------

    // Make sure everything is within the visible region of the viewer.
    viewer.fit_screen();

    // Run the viewer
    return viewer.run();
}

#else  // inherit customized drawables from Easy3D drawables, and we reimplement "void update_buffers_internal()"

class MyTrianglesDrawable : public TrianglesDrawable {
public:
    MyTrianglesDrawable(const std::string& name = "") : TrianglesDrawable(name) {}

protected:
    void update_buffers_internal() {
        // The coordinates of the vertices.
        const std::vector<vec3> &points = resource::bunny_vertices;
        // The indices represent how the vertices are connected to form triangles. The "bunny" is a triangle mesh, and thus
        // each consecutive three indices represent a triangle.
        const std::vector<unsigned int> &indices = resource::bunny_indices;

        // Upload the vertex positions of the surface to the GPU.
        update_vertex_buffer(points);
        // Upload the vertex indices of the surface to the GPU.
        update_element_buffer(indices);
    }
};


class MyLinesDrawable : public LinesDrawable {
public:
    MyLinesDrawable(const std::string& name = "") : LinesDrawable(name) {}

protected:
    void update_buffers_internal() {
        // The coordinates of the vertices.
        const std::vector<vec3> &points = resource::bunny_vertices;
        const Box3 &box = geom::bounding_box<Box3, std::vector<vec3> >(points);
        float xmin = box.min_coord(0);
        float xmax = box.max_coord(0);
        float ymin = box.min_coord(1);
        float ymax = box.max_coord(1);
        float zmin = box.min_coord(2);
        float zmax = box.max_coord(2);
        // The eight vertices of the bounding box.
        const std::vector<vec3> bbox_points = {
                vec3(xmin, ymin, zmax), vec3(xmax, ymin, zmax),
                vec3(xmin, ymax, zmax), vec3(xmax, ymax, zmax),
                vec3(xmin, ymin, zmin), vec3(xmax, ymin, zmin),
                vec3(xmin, ymax, zmin), vec3(xmax, ymax, zmin)
        };
        // The vertex indices of the twelve edges of the bounding box (each consecutive two numbers represent an edge).
        const std::vector<unsigned int> bbox_indices = {
                0, 1, 2, 3, 4, 5, 6, 7,
                0, 2, 4, 6, 1, 3, 5, 7,
                0, 4, 2, 6, 1, 5, 3, 7
        };
        // Upload the vertex positions of the bounding box to the GPU.
        update_vertex_buffer(bbox_points);
        // Upload the vertex indices of the bounding box to the GPU.
        update_element_buffer(bbox_indices);
    }
};


class MyPointsDrawable : public PointsDrawable {
public:
    MyPointsDrawable(const std::string& name = "") : PointsDrawable(name) {}

protected:
    void update_buffers_internal() {
        // The coordinates of the vertices.
        const std::vector<vec3> &points = resource::bunny_vertices;
        // Upload the vertex positions to the GPU.
        update_vertex_buffer(points);
    }
};


int main(int argc, char **argv) {
    // Initialize Easy3D.
    initialize();

    //-------------------------------------------------------------

    // Create the default Easy3D viewer.
    // Note: a viewer must be created before creating any drawables.
    Viewer viewer("Tutorial_301_Drawables");

    //-------------------------------------------------------------

    // We visualize the "bunny".

    //-------------------------------------------------------------
    // Create a TrianglesDrawable to visualize the surface of the "bunny".
    // For visualization, the point positions and the vertex indices of the faces have to be sent to the GPU.
    auto surface = new MyTrianglesDrawable("faces");
    // Add the drawable to the viewer
    viewer.add_drawable(surface);

    //-------------------------------------------------------------
    // Create a PointsDrawable to visualize the vertices of the "bunny".
    // Only the vertex positions have to be sent to the GPU for visualization.
    auto vertices = new MyPointsDrawable("vertices");
    // Add the drawable to the viewer
    viewer.add_drawable(vertices);
    // Set a color for the vertices (here we want a red color).
    vertices->set_uniform_coloring(vec4(1.0f, 0.0f, 0.0f, 1.0f));  // r, g, b, a
    // Three options are available for visualizing points:
    //      - PLAIN: plain points (i.e., each point is a square on the screen);
    //      - SPHERE: each point is visualized a sphere;
    //      - SURFEL: each point is visualized an oriented disk.
    // In this example, let's render the vertices as spheres.
    vertices->set_impostor_type(PointsDrawable::SPHERE);
    // Set the vertices size (here 10 pixels).
    vertices->set_point_size(10);

    //-------------------------------------------------------------
    // Create a LinesDrawable to visualize the bounding box of the "bunny".

    // Compute the bounding box.
    auto bbox_drawable = new MyLinesDrawable("bbox");
    // Add the drawable to the viewer
    viewer.add_drawable(bbox_drawable);
    // Set a color for the edges of the bounding box (here we want a blue color).
    bbox_drawable->set_uniform_coloring(vec4(0.0f, 0.0f, 1.0f, 1.0f));    // r, g, b, a
    // Set the width of the edges (here 5 pixels).
    bbox_drawable->set_line_width(5.0f);

    //-------------------------------------------------------------

    // Make sure everything is within the visible region of the viewer.
    viewer.fit_screen();

    // Run the viewer
    return viewer.run();
}

#endif
