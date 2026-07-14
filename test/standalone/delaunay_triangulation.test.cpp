// competitive-verifier: STANDALONE

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/delaunay_triangulation.hpp"

namespace{

using IntegerPoint = std::pair<long long, long long>;

__int128 cross(
    const IntegerPoint& first,
    const IntegerPoint& second,
    const IntegerPoint& third
){
    const __int128 first_x = static_cast<__int128>(second.first) - first.first;
    const __int128 first_y = static_cast<__int128>(second.second) - first.second;
    const __int128 second_x = static_cast<__int128>(third.first) - first.first;
    const __int128 second_y = static_cast<__int128>(third.second) - first.second;
    return first_x * second_y - first_y * second_x;
}

bool in_circumcircle_exact(
    const IntegerPoint& first,
    const IntegerPoint& second,
    const IntegerPoint& third,
    const IntegerPoint& query
){
    using boost::multiprecision::int256_t;
    const int256_t first_x = int256_t(first.first) - query.first;
    const int256_t first_y = int256_t(first.second) - query.second;
    const int256_t second_x = int256_t(second.first) - query.first;
    const int256_t second_y = int256_t(second.second) - query.second;
    const int256_t third_x = int256_t(third.first) - query.first;
    const int256_t third_y = int256_t(third.second) - query.second;
    const int256_t first_norm = first_x * first_x + first_y * first_y;
    const int256_t second_norm = second_x * second_x + second_y * second_y;
    const int256_t third_norm = third_x * third_x + third_y * third_y;
    const int256_t determinant =
        (second_x * third_y - second_y * third_x) * first_norm
        + (third_x * first_y - third_y * first_x) * second_norm
        + (first_x * second_y - first_y * second_x) * third_norm;
    return determinant > 0;
}

bool proper_intersection(
    const IntegerPoint& first,
    const IntegerPoint& second,
    const IntegerPoint& third,
    const IntegerPoint& fourth
){
    const __int128 first_side = cross(first, second, third);
    const __int128 second_side = cross(first, second, fourth);
    const __int128 third_side = cross(third, fourth, first);
    const __int128 fourth_side = cross(third, fourth, second);
    return ((first_side < 0 && second_side > 0) || (first_side > 0 && second_side < 0))
        && ((third_side < 0 && fourth_side > 0)
            || (third_side > 0 && fourth_side < 0));
}

std::size_t hull_boundary_size(std::vector<IntegerPoint> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if(points.size() <= 2) return points.size();
    bool collinear = true;
    for(std::size_t index = 2; index < points.size(); ++index){
        if(cross(points[0], points[1], points[index]) != 0) collinear = false;
    }
    if(collinear) return points.size();

    std::vector<IntegerPoint> hull;
    for(const IntegerPoint& point: points){
        while(hull.size() >= 2
            && cross(hull[hull.size() - 2], hull.back(), point) < 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    const std::size_t lower = hull.size();
    for(std::size_t index = points.size() - 1; index-- > 0;){
        const IntegerPoint& point = points[index];
        while(hull.size() > lower
            && cross(hull[hull.size() - 2], hull.back(), point) < 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    hull.pop_back();
    return hull.size();
}

void verify(const std::vector<IntegerPoint>& integer_points){
    std::vector<Point> points;
    points.reserve(integer_points.size());
    for(const auto& [x, y]: integer_points){
        points.emplace_back(static_cast<long double>(x), static_cast<long double>(y));
    }
    const DelaunayTriangulationResult result = delaunay_triangulation(points);
    assert(result.representative.size() == points.size());

    std::map<IntegerPoint, std::size_t> first_occurrence;
    for(std::size_t index = 0; index < integer_points.size(); ++index){
        first_occurrence.try_emplace(integer_points[index], index);
        assert(result.representative[index] == first_occurrence[integer_points[index]]);
    }
    assert(result.sites.size() == first_occurrence.size());
    for(std::size_t index = 1; index < result.sites.size(); ++index){
        assert(integer_points[result.sites[index - 1]]
            < integer_points[result.sites[index]]);
    }

    assert(std::is_sorted(result.edges.begin(), result.edges.end()));
    assert(std::adjacent_find(result.edges.begin(), result.edges.end())
        == result.edges.end());
    std::set<std::pair<std::size_t, std::size_t>> edge_set;
    for(const auto& edge: result.edges){
        assert(edge.first < edge.second);
        assert(edge.second < points.size());
        assert(result.representative[edge.first] == edge.first);
        assert(result.representative[edge.second] == edge.second);
        edge_set.insert(edge);
    }

    assert(std::is_sorted(result.triangles.begin(), result.triangles.end()));
    for(const auto& triangle: result.triangles){
        assert(triangle[0] == std::min({triangle[0], triangle[1], triangle[2]}));
        assert(triangle[0] < points.size()
            && triangle[1] < points.size()
            && triangle[2] < points.size());
        assert(cross(
            integer_points[triangle[0]],
            integer_points[triangle[1]],
            integer_points[triangle[2]]
        ) > 0);
        for(int edge = 0; edge < 3; ++edge){
            std::size_t first = triangle[static_cast<std::size_t>(edge)];
            std::size_t second = triangle[static_cast<std::size_t>((edge + 1) % 3)];
            if(first > second) std::swap(first, second);
            assert(edge_set.contains({first, second}));
        }
        const std::size_t sample_count = result.sites.size() <= 120
            ? result.sites.size()
            : 32;
        for(std::size_t sample = 0; sample < sample_count; ++sample){
            const std::size_t site_index = result.sites.size() <= 120
                ? sample
                : (
                    triangle[0] * std::size_t{1'000'003}
                    + triangle[1] * std::size_t{97'409}
                    + sample * std::size_t{65'537}
                ) % result.sites.size();
            const std::size_t site = result.sites[site_index];
            assert(!in_circumcircle_exact(
                integer_points[triangle[0]],
                integer_points[triangle[1]],
                integer_points[triangle[2]],
                integer_points[site]
            ));
        }
    }

    if(result.sites.size() <= 120){
        for(std::size_t first = 0; first < result.edges.size(); ++first){
            for(std::size_t second = first + 1; second < result.edges.size(); ++second){
                const auto& left = result.edges[first];
                const auto& right = result.edges[second];
                if(left.first == right.first || left.first == right.second
                    || left.second == right.first || left.second == right.second){
                    continue;
                }
                assert(!proper_intersection(
                    integer_points[left.first],
                    integer_points[left.second],
                    integer_points[right.first],
                    integer_points[right.second]
                ));
            }
        }
    }

    const std::size_t unique_count = result.sites.size();
    if(unique_count <= 1){
        assert(result.edges.empty() && result.triangles.empty());
        return;
    }
    std::vector<IntegerPoint> unique_points;
    for(std::size_t site: result.sites) unique_points.push_back(integer_points[site]);
    const std::size_t boundary = hull_boundary_size(unique_points);
    bool collinear = true;
    for(std::size_t index = 2; index < unique_points.size(); ++index){
        if(cross(unique_points[0], unique_points[1], unique_points[index]) != 0){
            collinear = false;
        }
    }
    if(collinear){
        assert(result.edges.size() == unique_count - 1);
        assert(result.triangles.empty());
    }else{
        assert(result.edges.size() == 3 * unique_count - 3 - boundary);
        assert(result.triangles.size() == 2 * unique_count - 2 - boundary);
    }
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool invalid_thrown = false;
    try{
        static_cast<void>(delaunay_triangulation({
            {std::numeric_limits<long double>::infinity(), 0.0L}
        }));
    }catch(const std::invalid_argument&){
        invalid_thrown = true;
    }
    assert(invalid_thrown);

    bool overflow_thrown = false;
    try{
        static_cast<void>(delaunay_triangulation({
            {std::numeric_limits<long double>::max(), 0.0L}
        }));
    }catch(const std::overflow_error&){
        overflow_thrown = true;
    }
    assert(overflow_thrown);

    const DelaunayTriangulationResult separated_near_duplicates =
        delaunay_triangulation({
            {0.0L, 0.0L},
            {5.0e-11L, 100.0L},
            {9.0e-11L, 0.0L},
        });
    assert(separated_near_duplicates.sites.size() == 2);
    assert(separated_near_duplicates.representative[0] == 0);
    assert(separated_near_duplicates.representative[2] == 0);

    const DelaunayTriangulationResult coordinate_order_representative =
        delaunay_triangulation({{5.0e-11L, 0.0L}, {0.0L, 0.0L}});
    assert(coordinate_order_representative.sites.size() == 1);
    assert(coordinate_order_representative.sites[0] == 1);
    assert(coordinate_order_representative.representative[0] == 1);
    assert(coordinate_order_representative.representative[1] == 1);

    const DelaunayTriangulationResult small_scale = delaunay_triangulation({
        {0.0L, 0.0L},
        {0.001L, 0.0L},
        {0.001L, 0.002L},
        {0.0L, 0.001L},
    });
    const DelaunayTriangulationResult unit_scale = delaunay_triangulation({
        {0.0L, 0.0L},
        {1.0L, 0.0L},
        {1.0L, 2.0L},
        {0.0L, 1.0L},
    });
    assert(std::find(small_scale.edges.begin(), small_scale.edges.end(),
        std::pair<std::size_t, std::size_t>{1, 3}) != small_scale.edges.end());
    assert(std::find(small_scale.edges.begin(), small_scale.edges.end(),
        std::pair<std::size_t, std::size_t>{0, 2}) == small_scale.edges.end());
    assert(small_scale.edges == unit_scale.edges);
    assert(small_scale.triangles == unit_scale.triangles);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<IntegerPoint> points(size);
        for(auto& [x, y]: points) std::cin >> x >> y;
        verify(points);
        std::cout << "OK\n";
    }
}
