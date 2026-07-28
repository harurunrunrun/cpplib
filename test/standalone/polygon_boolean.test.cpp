// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "../../src/algorithm/geometry/2d/result/point_set_point_set/polygon_boolean.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        const auto result = polygon_boolean(
            {{0, 0}, {3, 0}, {3, 2}, {0, 2}},
            {{1, -1}, {2, -1}, {2, 3}, {1, 3}},
            PolygonBooleanOperation::intersection
        );
        assert(result.size() == 1 && result.front().size() == 4);
        const std::vector<Point> first{{0, 0}, {3, 0}, {3, 2}, {0, 2}};
        const std::vector<Point> second{{1, -1}, {2, -1}, {2, 3}, {1, 3}};
        const auto total_area = [](const std::vector<std::vector<Point>>& polygons){
            long double answer = 0.0L;
            for(const auto& polygon: polygons){
                answer += std::abs(
                    plus_geometry_detail::signed_area_twice(polygon)
                ) / 2.0L;
            }
            return answer;
        };
        assert(std::abs(total_area(polygon_boolean(
            first, second, PolygonBooleanOperation::set_union
        )) - 8.0L) < 1.0e-8L);
        assert(std::abs(total_area(polygon_boolean(
            first, second, PolygonBooleanOperation::difference
        )) - 4.0L) < 1.0e-8L);
        assert(std::abs(total_area(polygon_boolean(
            first, second, PolygonBooleanOperation::symmetric_difference
        )) - 6.0L) < 1.0e-8L);
        std::cout << "OK\n";
    }
        const std::vector<std::vector<Point>> donut{
            {{0, 0}, {6, 0}, {6, 6}, {0, 6}},
            {{2, 2}, {4, 2}, {4, 4}, {2, 4}},
        };
        const std::vector<std::vector<Point>> window{
            {{1, 1}, {5, 1}, {5, 5}, {1, 5}},
        };
        const auto clipped = polygon_boolean(
            donut, window, PolygonBooleanOperation::intersection
        );
        long double signed_area = 0.0L;
        for(const auto& component: clipped){
            signed_area +=
                plus_geometry_detail::signed_area_twice(component) / 2.0L;
        }
        assert(std::abs(signed_area - 12.0L) < 1.0e-8L);

        const std::vector<Point> left{
            {0, 0}, {1, 0}, {1, 1}, {0, 1}
        };
        const std::vector<Point> right{
            {1, 0}, {2, 0}, {2, 1}, {1, 1}
        };
        const auto area_of = [](const std::vector<std::vector<Point>>& polygons){
            long double answer = 0.0L;
            for(const auto& polygon: polygons){
                answer += std::abs(plus_geometry_detail::signed_area_twice(polygon)) / 2.0L;
            }
            return answer;
        };
        assert(std::abs(area_of(polygon_boolean(
            left, right, PolygonBooleanOperation::set_union
        )) - 2.0L) < 1.0e-8L);
        assert(area_of(polygon_boolean(
            left, right, PolygonBooleanOperation::intersection
        )) < 1.0e-8L);
}
