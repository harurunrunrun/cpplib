// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "../../src/algorithm/geometry/2d/point_collection/enumerate_points_on_polygon_boundary.hpp"
#include "../../src/algorithm/geometry/2d/core/polygon_boundary_points.hpp"
#include "../../src/algorithm/geometry/2d/query/polygon_boundary_point_query.hpp"

std::vector<int> polygon_boundary_points_odr_part_a(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    bool unique
);
std::vector<int> polygon_boundary_points_odr_part_b(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    bool unique
);

void self_test(){
    std::vector<Point> polygon = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    std::vector<Point> points = {
        {0, 0},
        {2, 0},
        {4, 2},
        {1, 1},
        {0, 3},
        {2, 4},
    };
    auto result = enumerate_points_on_polygon_boundary(polygon, points);
    std::vector<int> expected = {0, 1, 2, 5, 4};
    assert(result == expected);
    auto duplicated = enumerate_points_on_polygon_boundary(polygon, points, false);
    std::vector<int> expected_duplicated = {0, 1, 2, 5, 4, 0};
    assert(duplicated == expected_duplicated);
    PolygonBoundaryPointQuery square_query(polygon);
    assert(square_query.uses_convex_fast_path());
    assert(square_query.enumerate(points) == expected);
    assert(square_query.enumerate(points, false) == expected_duplicated);

    std::vector<Point> repeated_vertex = {{0, 0}, {0, 0}, {2, 0}, {2, 2}, {0, 2}};
    std::vector<Point> candidates = {{0, 0}, {1, 0}, {1, 1}, {2, 1}, {0, 2}, {9, 9}};
    std::vector<int> expected_repeated = {0, 1, 3, 4};
    assert(enumerate_points_on_polygon_boundary(repeated_vertex, candidates) == expected_repeated);
    assert(enumerate_points_on_polygon_boundary({}, candidates).empty());
    PolygonBoundaryPointQuery repeated_query(repeated_vertex);
    assert(repeated_query.uses_convex_fast_path());
    assert(repeated_query.enumerate(candidates) == expected_repeated);
    assert(repeated_query.enumerate(candidates, false) ==
           enumerate_points_on_polygon_boundary(repeated_vertex, candidates, false));

    std::vector<Point> weak_convex = {
        {0, 0}, {0, 0}, {2, 0}, {4, 0}, {4, 2}, {4, 4}, {0, 4}, {0, 2}
    };
    std::vector<Point> weak_points = {
        {0, 0}, {1, 0}, {2, 0}, {4, 0}, {4, 3}, {3, 4}, {0, 2}, {2, 2}, {8, 8}
    };
    for(int reverse = 0; reverse < 2; ++reverse){
        PolygonBoundaryPointQuery query(weak_convex);
        assert(query.uses_convex_fast_path());
        for(bool unique: {false, true}){
            assert(query.enumerate(weak_points, unique) ==
                   enumerate_points_on_polygon_boundary(weak_convex, weak_points, unique));
        }
        std::reverse(weak_convex.begin(), weak_convex.end());
    }
    std::vector<Point> nonconvex = {{0, 0}, {4, 0}, {2, 2}, {4, 4}, {0, 4}};
    PolygonBoundaryPointQuery fallback_query(nonconvex);
    assert(!fallback_query.uses_convex_fast_path());
    assert(fallback_query.enumerate(weak_points) ==
           enumerate_points_on_polygon_boundary(nonconvex, weak_points));
    std::vector<Point> collinear = {{0, 0}, {1, 0}, {2, 0}};
    PolygonBoundaryPointQuery collinear_query(collinear);
    assert(!collinear_query.uses_convex_fast_path());
    assert(collinear_query.enumerate(candidates, false) ==
           enumerate_points_on_polygon_boundary(collinear, candidates, false));
    assert(on_segment({{3, 4}, {3, 4}}, {3, 4}));
    assert(!on_segment({{3, 4}, {3, 4}}, {3, 5}));
}

int main(){
    std::string first;
    if(!(std::cin >> first)){
        self_test();
        return 0;
    }
    int n, m;
    bool fast = false;
    bool unique = true;
    if(first == "FAST"){
        int unique_flag;
        std::cin >> n >> m >> unique_flag;
        fast = true;
        unique = unique_flag != 0;
    }else{
        n = std::stoi(first);
        std::cin >> m;
    }
    std::vector<Point> polygon(static_cast<std::size_t>(n));
    for(auto& p: polygon) std::cin >> p.x >> p.y;
    std::vector<Point> points(static_cast<std::size_t>(m));
    for(auto& p: points) std::cin >> p.x >> p.y;
    std::vector<int> result;
    if(fast){
        PolygonBoundaryPointQuery query(polygon);
        if(!query.uses_convex_fast_path()) return 1;
        result = query.enumerate(points, unique);
    }else{
        result = enumerate_points_on_polygon_boundary(polygon, points);
    }
    // The large FAST case verifies the accelerated query path. Running the
    // generic O(NM) implementation on the same input can exceed the checker
    // timeout on slower CI runners; ordinary cases still exercise this ODR
    // part and compare it with the reference result.
    if(!fast && polygon_boundary_points_odr_part_a(polygon, points, unique) != result){
        return 2;
    }
    if(polygon_boundary_points_odr_part_b(polygon, points, unique) != result) return 3;
    std::cout << result.size();
    for(int index: result) std::cout << ' ' << index;
    std::cout << '\n';
}
