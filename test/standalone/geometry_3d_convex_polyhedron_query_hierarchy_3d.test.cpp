// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/shape/convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/point/convex_polyhedron_closest_point.hpp"
#include "../../src/algorithm/geometry/3d/predicate/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/query/convex_polyhedron_query_hierarchy_3d.hpp"
#include "../../src/algorithm/geometry/3d/point/convex_polyhedron_support_point.hpp"
#include "../../src/algorithm/geometry/3d/scalar/distance.hpp"

namespace{

bool close(long double left, long double right){
    return std::abs(left - right) <= 2.0e-9L
        * std::max({1.0L, std::abs(left), std::abs(right)});
}

bool strict_distance_equal(long double left, long double right){
    if(left == right) return true;
    return std::abs(left - right)
        <= 256 * std::numeric_limits<long double>::epsilon()
            * std::max({
                std::abs(left), std::abs(right),
                std::numeric_limits<long double>::min(),
            });
}

bool same_point(const Point3& left, const Point3& right){
    return left.x == right.x && left.y == right.y && left.z == right.z;
}

Point3 random_point(std::mt19937_64& random, long double radius){
    std::uniform_real_distribution<long double> coordinate(-radius, radius);
    return {coordinate(random), coordinate(random), coordinate(random)};
}

std::vector<Point3> spherical_points(std::size_t count){
    std::vector<Point3> points;
    points.reserve(count);
    constexpr long double golden_angle =
        2.399963229728653322231555506633613853L;
    for(std::size_t index = 0; index < count; ++index){
        const long double z = 1.0L
            - 2.0L * (static_cast<long double>(index) + 0.5L)
                / static_cast<long double>(count);
        const long double radius = std::sqrt(std::max(0.0L, 1.0L - z * z));
        const long double angle = golden_angle * static_cast<long double>(index);
        points.push_back({radius * std::cos(angle), radius * std::sin(angle), z});
    }
    return points;
}

template<class Exception, class Function>
bool rejects(Function function){
    try{
        function();
    }catch(const Exception&){
        return true;
    }catch(...){
    }
    return false;
}

std::size_t hierarchy_depth_bound(std::size_t size){
    std::size_t logarithm = 0;
    while(size > 1){
        size = size / 2 + size % 2;
        ++logarithm;
    }
    return 2 + logarithm * 20;
}

bool compare_queries(
    const ConvexPolyhedron3& polyhedron,
    std::mt19937_64& random,
    std::size_t count
){
    const ConvexPolyhedronQueryHierarchy3D hierarchy(polyhedron, 4);
    if(polyhedron.affine_dimension == 3){
        if(hierarchy.support_hierarchy_depth() == 0
            || hierarchy.contains_hierarchy_depth() == 0
            || hierarchy.hierarchy_maximum_branching() > 11
            || hierarchy.support_hierarchy_depth()
                > hierarchy_depth_bound(polyhedron.faces.size())
            || hierarchy.contains_hierarchy_depth()
                > hierarchy_depth_bound(polyhedron.vertices.size())
            || (polyhedron.vertices.size() > 24
                && hierarchy.contains_hierarchy_depth() < 2)){
            return false;
        }
    }
    for(std::size_t query = 0; query < count; ++query){
        Point3 direction = random_point(random, 4);
        if(direction.x == 0 && direction.y == 0 && direction.z == 0){
            direction.x = 1;
        }
        const Point3 fast_support = hierarchy.support_point(direction);
        const Point3 slow_support = convex_polyhedron_support_point(
            hierarchy.polyhedron(), direction
        );
        if(!same_point(fast_support, slow_support)) return false;

        const Point3 point = random_point(random, 3);
        if(hierarchy.contains(point)
           != convex_polyhedron_contains(hierarchy.polyhedron(), point)){
            return false;
        }
        const Point3 fast_closest = hierarchy.closest_point(point);
        const Point3 slow_closest = convex_polyhedron_closest_point(
            hierarchy.polyhedron(), point
        );
        if(!close(distance(fast_closest, point), distance(slow_closest, point))){
            return false;
        }
    }
    return true;
}

}  // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    std::size_t large_vertices;
    std::size_t large_queries;
    if(!(std::cin >> seed >> rounds >> large_vertices >> large_queries)
       || rounds == 0 || rounds > 256 || large_vertices < 8
       || large_vertices > 512 || large_queries > 10000){
        return 2;
    }
    std::mt19937_64 random(seed);

    const ConvexPolyhedron3 point = convex_hull_3d({{2, -3, 5}});
    const ConvexPolyhedron3 segment = convex_hull_3d({{-2, 1, 4}, {3, 1, 4}});
    const ConvexPolyhedron3 polygon = convex_hull_3d({
        {-2, -1, 3}, {2, -1, 3}, {2, 1, 3}, {-2, 1, 3}, {0, 0, 3},
    });
    if(!compare_queries(point, random, 4)
       || !compare_queries(segment, random, 8)
       || !compare_queries(polygon, random, 12)) return 1;
    {
        const ConvexPolyhedronQueryHierarchy3D line_hierarchy(segment, 1);
        const Point3 direction{0, 1, 0};
        if(!same_point(
            line_hierarchy.support_point(direction),
            convex_polyhedron_support_point(segment, direction)
        )) return 1;
        if(!same_point(
            line_hierarchy.closest_point(segment.vertices.front()),
            segment.vertices.front()
        )) return 1;
    }

    std::vector<Point3> cube_points;
    for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
        cube_points.push_back({
            1.0e12L + static_cast<long double>(x) * 1.0e3L,
            -2.0e12L + static_cast<long double>(y) * 1.0e3L,
            3.0e12L + static_cast<long double>(z) * 1.0e3L,
        });
    }
    ConvexPolyhedron3 cube = convex_hull_3d(cube_points);
    for(auto& face: cube.faces) std::swap(face[1], face[2]);
    const ConvexPolyhedronQueryHierarchy3D inward_cube(cube, 2);
    if(!inward_cube.contains({1.0e12L, -2.0e12L, 3.0e12L})
       || inward_cube.contains({1.0e12L + 1001, -2.0e12L, 3.0e12L})){
        return 1;
    }
    const long double boundary_x = 1.0e12L + 1.0e3L;
    const long double outside_x = std::nextafter(
        boundary_x, std::numeric_limits<long double>::infinity()
    );
    if(!inward_cube.contains({boundary_x, -2.0e12L, 3.0e12L})
       || inward_cube.contains({outside_x, -2.0e12L, 3.0e12L})) return 1;
    if(!compare_queries(inward_cube.polyhedron(), random, rounds)) return 1;
    for(const Point3 direction: std::vector<Point3>{
        {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, 0, -1},
        {1, 1, 0}, {1, 0, -1}, {0, -1, 1},
        {1, 1, 1}, {-1, 1, -1},
    }){
        if(!same_point(
            inward_cube.support_point(direction),
            convex_polyhedron_support_point(
                inward_cube.polyhedron(), direction
            )
        )) return 1;
    }
    {
        const ConvexPolyhedron3 octahedron = convex_hull_3d({
            {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
            {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
        });
        const ConvexPolyhedronQueryHierarchy3D octahedron_hierarchy(
            octahedron, 3
        );
        for(const Point3 direction: std::vector<Point3>{
            {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
            {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
            {1, 1, 0}, {1, 1, 1},
        }){
            if(!same_point(
                octahedron_hierarchy.support_point(direction),
                convex_polyhedron_support_point(octahedron, direction)
            )) return 1;
        }
        if(!compare_queries(octahedron, random, 16)) return 1;
    }
    {
        const ConvexPolyhedron3 tetrahedron = convex_hull_3d({
            {0, 0, 0}, {8, 0, 0}, {0, 8, 0}, {0, 0, 8},
        });
        const auto selected = tetrahedron.faces.front();
        const Point3 first = tetrahedron.vertices[selected[0]];
        const Point3 second = tetrahedron.vertices[selected[1]];
        const Point3 third = tetrahedron.vertices[selected[2]];
        const Point3 interior{
            (first.x + second.x + 2 * third.x) / 4,
            (first.y + second.y + 2 * third.y) / 4,
            (first.z + second.z + 2 * third.z) / 4,
        };
        ConvexPolyhedron3 subdivided;
        subdivided.affine_dimension = 3;
        subdivided.vertices.push_back(interior);
        subdivided.vertices.insert(
            subdivided.vertices.end(),
            tetrahedron.vertices.begin(), tetrahedron.vertices.end()
        );
        for(std::size_t index = 0;
            index < tetrahedron.faces.size(); ++index){
            const auto face = tetrahedron.faces[index];
            if(index != 0){
                subdivided.faces.push_back({
                    face[0] + 1, face[1] + 1, face[2] + 1
                });
                continue;
            }
            const std::size_t a = face[0] + 1;
            const std::size_t b = face[1] + 1;
            const std::size_t c = face[2] + 1;
            subdivided.faces.push_back({a, b, 0});
            subdivided.faces.push_back({b, c, 0});
            subdivided.faces.push_back({c, a, 0});
        }
        const Point3 first_edge = second - first;
        const Point3 second_edge = third - first;
        const Point3 facet_normal{
            first_edge.y * second_edge.z - first_edge.z * second_edge.y,
            first_edge.z * second_edge.x - first_edge.x * second_edge.z,
            first_edge.x * second_edge.y - first_edge.y * second_edge.x,
        };
        const ConvexPolyhedronQueryHierarchy3D subdivided_hierarchy(
            subdivided, 2
        );
        if(!same_point(
            subdivided_hierarchy.support_point(facet_normal),
            subdivided.vertices[0]
        ) || !same_point(
            subdivided_hierarchy.support_point(facet_normal),
            convex_polyhedron_support_point(subdivided, facet_normal)
        ) || !compare_queries(subdivided, random, 16)){
            return 1;
        }
    }
    {
        const ConvexPolyhedron3 tetrahedron = convex_hull_3d({
            {0, 0, 0}, {8, 0, 0}, {0, 8, 0}, {0, 0, 8},
        });
        const std::size_t edge_first = tetrahedron.faces[0][0];
        const std::size_t edge_second = tetrahedron.faces[0][1];
        const Point3 midpoint{
            (tetrahedron.vertices[edge_first].x
                + tetrahedron.vertices[edge_second].x) / 2,
            (tetrahedron.vertices[edge_first].y
                + tetrahedron.vertices[edge_second].y) / 2,
            (tetrahedron.vertices[edge_first].z
                + tetrahedron.vertices[edge_second].z) / 2,
        };
        std::vector<std::size_t> incident_faces;
        ConvexPolyhedron3 subdivided;
        subdivided.affine_dimension = 3;
        subdivided.vertices.push_back(midpoint);
        subdivided.vertices.insert(
            subdivided.vertices.end(),
            tetrahedron.vertices.begin(), tetrahedron.vertices.end()
        );
        for(std::size_t index = 0;
            index < tetrahedron.faces.size(); ++index){
            const auto face = tetrahedron.faces[index];
            std::size_t edge_position = 3;
            for(std::size_t position = 0; position < 3; ++position){
                const std::size_t first = face[position];
                const std::size_t second = face[(position + 1) % 3];
                if(std::minmax(first, second)
                    == std::minmax(edge_first, edge_second)){
                    edge_position = position;
                    break;
                }
            }
            if(edge_position == 3){
                subdivided.faces.push_back({
                    face[0] + 1, face[1] + 1, face[2] + 1
                });
                continue;
            }
            incident_faces.push_back(index);
            const std::size_t first = face[edge_position] + 1;
            const std::size_t second = face[(edge_position + 1) % 3] + 1;
            const std::size_t third = face[(edge_position + 2) % 3] + 1;
            subdivided.faces.push_back({first, 0, third});
            subdivided.faces.push_back({0, second, third});
        }
        if(incident_faces.size() != 2) return 1;
        const auto outward_normal =
            [&](const std::array<std::size_t, 3>& face) -> Point3{
                const Point3 first = tetrahedron.vertices[face[0]];
                const Point3 first_edge =
                    tetrahedron.vertices[face[1]] - first;
                const Point3 second_edge =
                    tetrahedron.vertices[face[2]] - first;
                return {
                    first_edge.y * second_edge.z
                        - first_edge.z * second_edge.y,
                    first_edge.z * second_edge.x
                        - first_edge.x * second_edge.z,
                    first_edge.x * second_edge.y
                        - first_edge.y * second_edge.x,
                };
            };
        const Point3 edge_normal =
            outward_normal(tetrahedron.faces[incident_faces[0]])
            + outward_normal(tetrahedron.faces[incident_faces[1]]);
        const ConvexPolyhedronQueryHierarchy3D subdivided_hierarchy(
            subdivided, 2
        );
        if(!same_point(
            subdivided_hierarchy.support_point(edge_normal),
            subdivided.vertices[0]
        ) || !same_point(
            subdivided_hierarchy.support_point(edge_normal),
            convex_polyhedron_support_point(subdivided, edge_normal)
        ) || !compare_queries(subdivided, random, 16)){
            return 1;
        }
    }
    for(const Point3 boundary: std::vector<Point3>{
        {boundary_x, -2.0e12L, 3.0e12L},
        {boundary_x, -2.0e12L + 1.0e3L, 3.0e12L},
        {boundary_x, -2.0e12L + 1.0e3L, 3.0e12L + 1.0e3L},
    }){
        if(!inward_cube.contains(boundary)
           || !same_point(inward_cube.closest_point(boundary), boundary)){
            return 1;
        }
    }

    {
        const Point3 center{1.0e12L, -2.0e12L, 3.0e12L};
        const long double infinity =
            std::numeric_limits<long double>::infinity();
        for(const Point3& vertex: inward_cube.polyhedron().vertices){
            const Point3 query{
                std::nextafter(
                    vertex.x, vertex.x < center.x ? -infinity : infinity
                ),
                std::nextafter(
                    vertex.y, vertex.y < center.y ? -infinity : infinity
                ),
                std::nextafter(
                    vertex.z, vertex.z < center.z ? -infinity : infinity
                ),
            };
            const Point3 fast = inward_cube.closest_point(query);
            const Point3 slow = convex_polyhedron_closest_point(
                inward_cube.polyhedron(), query
            );
            if(!strict_distance_equal(
                distance(fast, query), distance(slow, query)
            )) return 1;
        }
    }
    {
        const long double thin = std::ldexp(1.0L, -60);
        const ConvexPolyhedron3 skinny = convex_hull_3d({
            {0, 0, 0}, {1, 0, 0}, {0, thin, 0}, {0, 0, thin},
        });
        const ConvexPolyhedronQueryHierarchy3D skinny_hierarchy(skinny, 1);
        for(const Point3 query: std::vector<Point3>{
            {0.25L, thin / 4, -std::numeric_limits<long double>::denorm_min()},
            {std::nextafter(0.5L, 1.0L), thin / 3, thin / 3},
            {-std::numeric_limits<long double>::denorm_min(), thin / 4, thin / 4},
        }){
            const Point3 fast = skinny_hierarchy.closest_point(query);
            const Point3 slow = convex_polyhedron_closest_point(skinny, query);
            if(!strict_distance_equal(
                distance(fast, query), distance(slow, query)
            )) return 1;
        }
    }
    {
        const long double center = std::ldexp(1.0L, 8000);
        const long double step = std::ldexp(1.0L, 7940);
        std::vector<Point3> extreme_points;
        for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
            extreme_points.push_back({
                center + static_cast<long double>(x) * step,
                -center + static_cast<long double>(y) * step,
                center + static_cast<long double>(z) * step,
            });
        }
        const ConvexPolyhedronQueryHierarchy3D extreme(
            convex_hull_3d(std::move(extreme_points)), 1
        );
        const Point3 tiny_direction{
            std::numeric_limits<long double>::denorm_min(), 0, 0
        };
        if(!same_point(
            extreme.support_point(tiny_direction),
            convex_polyhedron_support_point(
                extreme.polyhedron(), tiny_direction
            )
        )) return 1;
        const Point3 extreme_center{center, -center, center};
        if(!extreme.contains(extreme_center)
           || !same_point(extreme.closest_point(extreme_center), extreme_center)){
            return 1;
        }
    }

    for(std::size_t iteration = 0;
        iteration < std::min<std::size_t>(rounds, 24); ++iteration){
        std::vector<Point3> cloud;
        const std::size_t point_count = 12 + iteration % 17;
        cloud.reserve(point_count);
        for(std::size_t index = 0; index < point_count; ++index){
            cloud.push_back(random_point(random, 10));
        }
        const ConvexPolyhedron3 random_hull = convex_hull_3d(std::move(cloud));
        if(random_hull.affine_dimension != 3
           || !compare_queries(random_hull, random, 8)) return 1;
    }
    {
        ConvexPolyhedron3 unused = inward_cube.polyhedron();
        unused.vertices.push_back({7, 11, 13});
        if(!rejects<std::domain_error>([&]{
            static_cast<void>(
                ConvexPolyhedronQueryHierarchy3D(std::move(unused))
            );
        })) return 1;
    }
    {
        ConvexPolyhedron3 mixed = inward_cube.polyhedron();
        std::swap(mixed.faces.front()[1], mixed.faces.front()[2]);
        if(!rejects<std::domain_error>([&]{
            static_cast<void>(
                ConvexPolyhedronQueryHierarchy3D(std::move(mixed))
            );
        })) return 1;
    }
    {
        ConvexPolyhedron3 invalid = inward_cube.polyhedron();
        invalid.faces.front()[0] = invalid.vertices.size();
        if(!rejects<std::out_of_range>([&]{
            static_cast<void>(
                ConvexPolyhedronQueryHierarchy3D(std::move(invalid))
            );
        })) return 1;
    }
    {
        ConvexPolyhedron3 degenerate = inward_cube.polyhedron();
        degenerate.vertices[degenerate.faces.front()[2]] =
            degenerate.vertices[degenerate.faces.front()[1]];
        if(!rejects<std::domain_error>([&]{
            static_cast<void>(
                ConvexPolyhedronQueryHierarchy3D(std::move(degenerate))
            );
        })) return 1;
    }
    {
        ConvexPolyhedron3 nonfinite = inward_cube.polyhedron();
        nonfinite.vertices.front().x =
            std::numeric_limits<long double>::infinity();
        if(!rejects<std::invalid_argument>([&]{
            static_cast<void>(
                ConvexPolyhedronQueryHierarchy3D(std::move(nonfinite))
            );
        })) return 1;
    }
    {
        ConvexPolyhedron3 flat{
            3,
            {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}},
            {{0, 2, 1}, {0, 3, 2}, {0, 1, 3}, {1, 2, 3}},
        };
        if(!rejects<std::domain_error>([&]{
            static_cast<void>(
                ConvexPolyhedronQueryHierarchy3D(std::move(flat))
            );
        })) return 1;
    }
    {
        const ConvexPolyhedron3 first = convex_hull_3d({
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
        });
        const ConvexPolyhedron3 second = convex_hull_3d({
            {10, 0, 0}, {11, 0, 0}, {10, 1, 0}, {10, 0, 1},
        });
        ConvexPolyhedron3 disconnected = first;
        const std::size_t offset = disconnected.vertices.size();
        disconnected.vertices.insert(
            disconnected.vertices.end(),
            second.vertices.begin(), second.vertices.end()
        );
        for(auto face: second.faces){
            for(std::size_t& vertex: face) vertex += offset;
            disconnected.faces.push_back(face);
        }
        if(!rejects<std::domain_error>([&]{
            static_cast<void>(
                ConvexPolyhedronQueryHierarchy3D(std::move(disconnected))
            );
        })) return 1;
    }
    if(!rejects<std::invalid_argument>([]{
        static_cast<void>(ConvexPolyhedronQueryHierarchy3D(
            convex_hull_3d({{0, 0, 0}}), 0
        ));
    })) return 1;
    if(!rejects<std::invalid_argument>([&]{
        static_cast<void>(inward_cube.support_point({
            std::numeric_limits<long double>::infinity(), 0, 0
        }));
    }) || !rejects<std::invalid_argument>([&]{
        static_cast<void>(inward_cube.contains({
            0, std::numeric_limits<long double>::quiet_NaN(), 0
        }));
    }) || !rejects<std::invalid_argument>([&]{
        static_cast<void>(inward_cube.closest_point({
            0, 0, std::numeric_limits<long double>::infinity()
        }));
    })) return 1;

    bool rejected = false;
    try{
        static_cast<void>(ConvexPolyhedronQueryHierarchy3D({}));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 1;
    rejected = false;
    try{
        ConvexPolyhedron3 open = inward_cube.polyhedron();
        open.faces.pop_back();
        static_cast<void>(ConvexPolyhedronQueryHierarchy3D(std::move(open)));
    }catch(const std::domain_error&){
        rejected = true;
    }
    if(!rejected) return 1;
    rejected = false;
    try{
        static_cast<void>(inward_cube.support_point({0, 0, 0}));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 1;

    const ConvexPolyhedron3 large = convex_hull_3d(
        spherical_points(large_vertices)
    );
    const ConvexPolyhedronQueryHierarchy3D hierarchy(large, 8);
    long double checksum = 0;
    for(std::size_t query = 0; query < large_queries; ++query){
        Point3 direction = random_point(random, 2);
        if(direction.x == 0 && direction.y == 0 && direction.z == 0){
            direction.z = 1;
        }
        const Point3 support = hierarchy.support_point(direction);
        const Point3 point = random_point(random, 2);
        const bool inside = hierarchy.contains(point);
        const Point3 closest = hierarchy.closest_point(point);
        checksum += support.x * 0.125L + support.y * 0.25L + support.z * 0.5L;
        checksum += distance(closest, point) + (inside ? 1.0L : -1.0L);
    }
    if(!std::isfinite(checksum)) return 1;

    std::cout << "OK\n";
    return 0;
}
