// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_facets.hpp"

int main(){
    int affine_dimension;
    std::size_t vertex_count;
    std::size_t face_count;
    if(!(std::cin >> affine_dimension >> vertex_count >> face_count)) return 2;
    ConvexPolyhedron3 polyhedron;
    polyhedron.affine_dimension = affine_dimension;
    polyhedron.vertices.resize(vertex_count);
    polyhedron.faces.resize(face_count);
    for(Point3& point: polyhedron.vertices){
        if(!(std::cin >> point.x >> point.y >> point.z)) return 2;
    }
    for(auto& face: polyhedron.faces){
        if(!(std::cin >> face[0] >> face[1] >> face[2])) return 2;
    }
    try{
        const auto facets = convex_polyhedron_facets(polyhedron);
        std::cout << "OK " << facets.size() << '\n';
    }catch(const std::domain_error&){
        std::cout << "DOMAIN\n";
    }catch(const std::invalid_argument&){
        std::cout << "INVALID\n";
    }catch(const std::out_of_range&){
        std::cout << "RANGE\n";
    }catch(...){
        std::cout << "OTHER\n";
    }
}
