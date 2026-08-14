#ifndef CPPLIB_TEST_STANDALONE_POLYHEDRON3_TEST_FIXTURE_HPP_INCLUDED
#define CPPLIB_TEST_STANDALONE_POLYHEDRON3_TEST_FIXTURE_HPP_INCLUDED

#include <cstddef>
#include <istream>

#include "../../src/algorithm/geometry/3d/type/definition/polyhedron3.hpp"

inline Polyhedron3 concave_l_prism_fixture(){
    Polyhedron3 mesh;
    mesh.vertices = {
        {0, 0, 0}, {2, 0, 0}, {2, 1, 0},
        {1, 1, 0}, {1, 2, 0}, {0, 2, 0},
        {0, 0, 1}, {2, 0, 1}, {2, 1, 1},
        {1, 1, 1}, {1, 2, 1}, {0, 2, 1},
    };
    mesh.faces = {
        {2, 1, 0}, {3, 2, 0}, {3, 0, 5}, {4, 3, 5},
        {6, 7, 8}, {6, 8, 9}, {11, 6, 9}, {11, 9, 10},
        {0, 1, 7}, {0, 7, 6}, {1, 2, 8}, {1, 8, 7},
        {2, 3, 9}, {2, 9, 8}, {3, 4, 10}, {3, 10, 9},
        {4, 5, 11}, {4, 11, 10}, {5, 0, 6}, {5, 6, 11},
    };
    return mesh;
}

inline bool read_polyhedron3_fixture(std::istream& input, Polyhedron3& mesh){
    std::size_t vertex_count, face_count;
    if(!(input >> vertex_count >> face_count)) return false;
    mesh.vertices.resize(vertex_count);
    mesh.faces.resize(face_count);
    for(Point3& point: mesh.vertices) input >> point.x >> point.y >> point.z;
    for(auto& face: mesh.faces) input >> face[0] >> face[1] >> face[2];
    return true;
}

#endif  // CPPLIB_TEST_STANDALONE_POLYHEDRON3_TEST_FIXTURE_HPP_INCLUDED
