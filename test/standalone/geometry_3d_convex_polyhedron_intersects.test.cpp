// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/geometry/3d/convex_polyhedron_intersects.hpp"

namespace{

ConvexPolyhedron3 read_polyhedron(){
    ConvexPolyhedron3 result;
    std::size_t vertex_count;
    std::size_t face_count;
    if(!(std::cin >> result.affine_dimension >> vertex_count >> face_count)){
        throw std::runtime_error("invalid polyhedron header");
    }
    result.vertices.resize(vertex_count);
    for(Point3& point: result.vertices){
        if(!(std::cin >> point.x >> point.y >> point.z)){
            throw std::runtime_error("invalid polyhedron vertex");
        }
    }
    result.faces.resize(face_count);
    for(auto& face: result.faces){
        if(!(std::cin >> face[0] >> face[1] >> face[2])){
            throw std::runtime_error("invalid polyhedron face");
        }
    }
    return result;
}

template<class Exception, class Function>
bool throws_exception(Function&& function){
    try{
        function();
    }catch(const Exception&){
        return true;
    }
    return false;
}

void self_check(){
    const ConvexPolyhedron3 point{0, {{0, 0, 0}}, {}};
    if(!throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects(ConvexPolyhedron3{}, point);
    })) throw std::runtime_error("empty fast input was accepted");
    if(!throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects(point, point, 0.0L);
    })) throw std::runtime_error("zero tolerance was accepted");
    if(!throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects(point, point, 1.0e-12L, 0);
    })) throw std::runtime_error("zero iteration limit was accepted");
    if(!throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects_exact(point, point);
    })) throw std::runtime_error("non-spatial exact input was accepted");

    ConvexPolyhedron3 invalid{
        3,
        {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
        {{0, 1, 7}},
    };
    if(!throws_exception<std::out_of_range>([&]{
        (void)convex_polyhedron_intersects_exact(invalid, invalid);
    })) throw std::runtime_error("invalid face index was accepted");

    ConvexPolyhedron3 non_finite = point;
    non_finite.vertices.front().x =
        std::numeric_limits<long double>::infinity();
    if(!throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects(non_finite, point);
    })) throw std::runtime_error("non-finite fast input was accepted");

    const long double base = 1.0e3000L;
    const long double next = std::nextafter(
        base, std::numeric_limits<long double>::infinity()
    );
    const long double next_next = std::nextafter(
        next, std::numeric_limits<long double>::infinity()
    );
    const long double next_next_next = std::nextafter(
        next_next, std::numeric_limits<long double>::infinity()
    );
    const auto tetrahedron = [&](long double minimum_x, long double maximum_x){
        return ConvexPolyhedron3{
            3,
            {
                {minimum_x, base, base}, {maximum_x, base, base},
                {minimum_x, next, base}, {minimum_x, base, next},
            },
            {{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}},
        };
    };
    const ConvexPolyhedron3 extreme = tetrahedron(base, next);
    const ConvexPolyhedron3 separated = tetrahedron(
        next_next, next_next_next
    );
    if(!convex_polyhedron_intersects_exact(extreme, extreme)
        || convex_polyhedron_intersects_exact(extreme, separated)){
        throw std::runtime_error("extreme exact projection failed");
    }
}

void stress_query(std::size_t point_count){
    if(point_count == 0 || point_count > 500000){
        throw std::runtime_error("invalid stress size");
    }
    ConvexPolyhedron3 first;
    ConvexPolyhedron3 second;
    first.affine_dimension = second.affine_dimension = 3;
    first.vertices.reserve(point_count);
    second.vertices.reserve(point_count);
    for(std::size_t index = 0; index < point_count; ++index){
        const long double y = (index & 2U) == 0 ? -1.0L : 1.0L;
        const long double z = (index & 4U) == 0 ? -1.0L : 1.0L;
        first.vertices.push_back({
            (index & 1U) == 0 ? -1.0L : 1.0L, y, z
        });
        second.vertices.push_back({
            (index & 1U) == 0 ? 3.0L : 5.0L, y, z
        });
    }
    std::cout << convex_polyhedron_intersects(first, second) << '\n';
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_check();
    std::size_t query_count;
    if(!(std::cin >> query_count)) return 2;
    for(std::size_t query = 0; query < query_count; ++query){
        char operation;
        std::cin >> operation;
        if(operation == 'B'){
            const ConvexPolyhedron3 first = read_polyhedron();
            const ConvexPolyhedron3 second = read_polyhedron();
            std::cout
                << convex_polyhedron_intersects(first, second) << ' '
                << convex_polyhedron_intersects_exact(first, second) << '\n';
        }else if(operation == 'G'){
            const ConvexPolyhedron3 first = read_polyhedron();
            const ConvexPolyhedron3 second = read_polyhedron();
            std::cout << convex_polyhedron_intersects(first, second) << '\n';
        }else if(operation == 'S'){
            std::size_t point_count;
            if(!(std::cin >> point_count)){
                throw std::runtime_error("invalid stress query");
            }
            stress_query(point_count);
        }else{
            throw std::runtime_error("unknown operation");
        }
    }
}
