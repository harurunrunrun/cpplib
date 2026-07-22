#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM3_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <optional>
#include <vector>

#include "base.hpp"
#include "convex_polyhedron3.hpp"
struct VoronoiRay3{
    Point3 origin;
    Point3 direction;
};


struct VoronoiCell3{
    std::size_t site = 0;
    std::vector<std::size_t> finite_vertices;
    std::vector<std::size_t> neighbors;
    std::vector<std::size_t> ridge_indices;
    std::vector<Plane3> boundary_halfspaces;
    std::optional<ConvexPolyhedron3> bounded_polyhedron;
    bool unbounded = false;
};

struct VoronoiRidge3{
    std::array<std::size_t, 2> sites{};
    // Cyclic order for bounded ridges and path order for unbounded ridges.
    std::vector<std::size_t> finite_vertices;
    std::vector<std::size_t> edge_indices;
    std::vector<VoronoiRay3> unbounded_rays;
    bool unbounded = false;
};

struct VoronoiEdge3{
    std::array<std::size_t, 3> sites{};
    std::vector<std::size_t> finite_vertices;
    std::optional<Segment3> segment;
    std::optional<VoronoiRay3> ray;
    bool unbounded = false;
};

struct VoronoiDiagram3{
    int affine_dimension = -1;
    std::vector<Point3> sites;
    std::vector<Point3> finite_vertices;
    std::vector<VoronoiCell3> cells;
    std::vector<VoronoiRidge3> ridges;
    std::vector<VoronoiEdge3> edges;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM3_HPP_INCLUDED
