// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/geometry/3d/predicate/convex_polyhedron_intersects.hpp"

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

ConvexPolyhedron3 axis_box(const Point3& center){
    return {
        3,
        {
            center + Point3{-1, -1, -1},
            center + Point3{ 1, -1, -1},
            center + Point3{-1,  1, -1},
            center + Point3{ 1,  1, -1},
            center + Point3{-1, -1,  1},
            center + Point3{ 1, -1,  1},
            center + Point3{-1,  1,  1},
            center + Point3{ 1,  1,  1},
        },
        {
            {0, 4, 6}, {0, 6, 2},
            {1, 3, 7}, {1, 7, 5},
            {0, 1, 5}, {0, 5, 4},
            {2, 6, 7}, {2, 7, 3},
            {0, 2, 3}, {0, 3, 1},
            {4, 5, 7}, {4, 7, 6},
        },
    };
}

bool checked_exact_answer(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    const bool expected = convex_polyhedron_intersects_exact_sat(
        first, second
    );
    constexpr std::array<std::uint64_t, 12> seeds{
        0, 1, 2, 3,
        0x243F6A8885A308D3ULL,
        0x9E3779B97F4A7C15ULL,
        0xD1B54A32D192ED03ULL,
        0x94D049BB133111EBULL,
        0xBF58476D1CE4E5B9ULL,
        0xDEADBEEFCAFEBABEULL,
        0x8000000000000000ULL,
        std::numeric_limits<std::uint64_t>::max(),
    };
    for(const std::uint64_t seed: seeds){
        if(convex_polyhedron_intersects_exact_with_seed(
            first, second, seed
        ) != expected){
            throw std::runtime_error("seeded exact LP/SAT mismatch");
        }
    }
    if(convex_polyhedron_intersects_exact(first, second) != expected){
        throw std::runtime_error("random exact LP/SAT mismatch");
    }
    return expected;
}

ConvexPolyhedron3 subdivided_axis_box(
    const Point3& center,
    std::size_t division
){
    if(division == 0) throw std::invalid_argument("zero box subdivision");
    const std::size_t width = division + 1;
    ConvexPolyhedron3 result;
    result.affine_dimension = 3;
    result.vertices.reserve(width * width * width);
    for(std::size_t first = 0; first < width; ++first){
        for(std::size_t second = 0; second < width; ++second){
            for(std::size_t third = 0; third < width; ++third){
                const auto coordinate = [&](std::size_t index){
                    return -1.0L
                        + 2.0L * static_cast<long double>(index)
                        / static_cast<long double>(division);
                };
                result.vertices.push_back(center + Point3{
                    coordinate(first), coordinate(second), coordinate(third)
                });
            }
        }
    }
    const auto vertex = [&](std::size_t first, std::size_t second,
                            std::size_t third){
        return (first * width + second) * width + third;
    };
    for(std::size_t fixed_axis = 0; fixed_axis < 3; ++fixed_axis){
        const std::size_t first_axis = (fixed_axis + 1) % 3;
        const std::size_t second_axis = (fixed_axis + 2) % 3;
        for(const std::size_t fixed: {std::size_t{0}, division}){
            for(std::size_t first = 0; first < division; ++first){
                for(std::size_t second = 0; second < division; ++second){
                    const auto index = [&](std::size_t first_value,
                                           std::size_t second_value){
                        std::array<std::size_t, 3> coordinate{};
                        coordinate[fixed_axis] = fixed;
                        coordinate[first_axis] = first_value;
                        coordinate[second_axis] = second_value;
                        return vertex(
                            coordinate[0], coordinate[1], coordinate[2]
                        );
                    };
                    const std::size_t lower_left = index(first, second);
                    const std::size_t lower_right = index(first + 1, second);
                    const std::size_t upper_right =
                        index(first + 1, second + 1);
                    const std::size_t upper_left = index(first, second + 1);
                    result.faces.push_back({
                        lower_left, lower_right, upper_right
                    });
                    result.faces.push_back({
                        lower_left, upper_right, upper_left
                    });
                }
            }
        }
    }
    return result;
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
    }) || !throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects_exact_with_seed(point, point, 0);
    }) || !throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects_exact_sat(point, point);
    })) throw std::runtime_error("non-spatial exact input was accepted");

    ConvexPolyhedron3 invalid{
        3,
        {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
        {{0, 1, 7}},
    };
    if(!throws_exception<std::out_of_range>([&]{
        (void)convex_polyhedron_intersects_exact(invalid, invalid);
    }) || !throws_exception<std::out_of_range>([&]{
        (void)convex_polyhedron_intersects_exact_with_seed(
            invalid, invalid, 0
        );
    }) || !throws_exception<std::out_of_range>([&]{
        (void)convex_polyhedron_intersects_exact_sat(invalid, invalid);
    })) throw std::runtime_error("invalid face index was accepted");

    ConvexPolyhedron3 non_finite = point;
    non_finite.vertices.front().x =
        std::numeric_limits<long double>::infinity();
    if(!throws_exception<std::invalid_argument>([&]{
        (void)convex_polyhedron_intersects(non_finite, point);
    })) throw std::runtime_error("non-finite fast input was accepted");

    const ConvexPolyhedron3 cube = axis_box({});
    if(!checked_exact_answer(cube, cube)
        || !checked_exact_answer(cube, axis_box({2, 0, 0}))
        || !checked_exact_answer(cube, axis_box({2, 2, 0}))
        || !checked_exact_answer(cube, axis_box({2, 2, 2}))
        || checked_exact_answer(cube, axis_box({3, 0, 0}))){
        throw std::runtime_error("face/edge/vertex contact failed");
    }

    ConvexPolyhedron3 reordered = cube;
    std::reverse(reordered.faces.begin(), reordered.faces.end());
    for(auto& face: reordered.faces){
        std::rotate(face.begin(), face.begin() + 1, face.end());
        std::swap(face[1], face[2]);
    }
    if(!checked_exact_answer(cube, reordered)){
        throw std::runtime_error("face order/orientation changed result");
    }

    ConvexPolyhedron3 duplicated = cube;
    const auto cube_faces = cube.faces;
    for(std::size_t repetition = 0; repetition < 32; ++repetition){
        duplicated.faces.insert(
            duplicated.faces.end(), cube_faces.begin(), cube_faces.end()
        );
    }
    if(!convex_polyhedron_intersects_exact(cube, duplicated)){
        throw std::runtime_error("duplicate coplanar constraints failed");
    }
    for(const std::uint64_t seed: {
        std::uint64_t{0}, std::numeric_limits<std::uint64_t>::max()
    }){
        if(!convex_polyhedron_intersects_exact_with_seed(
            cube, duplicated, seed
        )){
            throw std::runtime_error("duplicate coplanar constraints failed");
        }
    }

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
    if(!checked_exact_answer(extreme, extreme)
        || checked_exact_answer(extreme, separated)){
        throw std::runtime_error("extreme exact projection failed");
    }

    const ConvexPolyhedron3 dense = subdivided_axis_box({}, 16);
    const ConvexPolyhedron3 dense_separated =
        subdivided_axis_box({4, 0, 0}, 16);
    for(const std::uint64_t seed: {
        std::uint64_t{0}, std::uint64_t{0x9E3779B97F4A7C15ULL}
    }){
        if(!convex_polyhedron_intersects_exact_with_seed(
            dense, dense, seed
        ) || convex_polyhedron_intersects_exact_with_seed(
            dense, dense_separated, seed
        )){
            throw std::runtime_error("large subdivided exact LP failed");
        }
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
            const bool exact = checked_exact_answer(first, second);
            std::cout
                << convex_polyhedron_intersects(first, second) << ' '
                << exact << '\n';
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
