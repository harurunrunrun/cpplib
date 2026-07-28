// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "../../src/algorithm/geometry/2d/point_set/point_set_scalar/polygon_offset.hpp"

namespace{

Point midpoint(const PolygonOffsetEdge& edge){
    if(edge.kind == PolygonOffsetEdgeKind::segment){
        return (edge.first + edge.second) / 2.0L;
    }
    long double first = edge.first_angle;
    long double second = edge.second_angle;
    if(edge.counterclockwise){
        while(second < first) second += 2.0L * polygon_offset_detail::pi;
    }else{
        while(first < second) first += 2.0L * polygon_offset_detail::pi;
    }
    const long double angle = (first + second) / 2.0L;
    return edge.center + Point{
        std::cos(angle), std::sin(angle)
    } * edge.radius;
}

void verify(
    const std::vector<Point>& polygon,
    long double distance,
    const PolygonOffsetResult& result
){
    for(const PolygonOffsetEdge& edge: result.edges){
        assert(std::abs(
            polygon_offset_detail::signed_distance(
                polygon, midpoint(edge)
            ) - distance
        ) < 1.0e-7L);
    }
    for(const auto& contour: result.contours){
        assert(!contour.empty());
        for(std::size_t index = 0; index < contour.size(); ++index){
            const auto& current = result.edges[contour[index]];
            const auto& next = result.edges[
                contour[(index + 1) % contour.size()]
            ];
            assert(plus_geometry_detail::close(
                current.second, next.first
            ));
        }
    }
}

}  // namespace

int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        const std::vector<Point> square{
            {0, 0}, {2, 0}, {2, 2}, {0, 2}
        };
        const auto square_result = polygon_offset(square, 1.0L);
        assert(square_result.edges.size() == 8);
        assert(square_result.contours.size() == 1);
        verify(square, 1.0L, square_result);
        for(const PolygonOffsetEdge& edge: square_result.edges){
            if(edge.kind == PolygonOffsetEdgeKind::segment){
                assert(edge.source_kind == PolygonOffsetSourceKind::edge);
            }else{
                assert(edge.source_kind == PolygonOffsetSourceKind::vertex);
            }
            assert(edge.source_index < square.size());
        }

        const std::vector<Point> concave{
            {0, 0}, {4, 0}, {4, 1},
            {1, 1}, {1, 4}, {0, 4},
        };
        const auto outward = polygon_offset(concave, 0.5L);
        assert(!outward.edges.empty());
        assert(!outward.contours.empty());
        verify(concave, 0.5L, outward);

        const auto inward = polygon_offset(concave, -0.4L);
        const std::vector<Point> dumbbell{
            {0, 0}, {4, 0}, {4, 1.5L}, {6, 1.5L},
            {6, 0}, {10, 0}, {10, 4}, {6, 4},
            {6, 2.5L}, {4, 2.5L}, {4, 4}, {0, 4},
        };
        const auto split = polygon_offset(dumbbell, -0.6L);
        assert(split.contours.size() == 2);
        verify(dumbbell, -0.6L, split);
        const auto expanded = polygon_offset(dumbbell, 0.75L);
        assert(expanded.contours.size() == 1);
        verify(dumbbell, 0.75L, expanded);

        const std::vector<Point> unit_square{
            {0, 0}, {1, 0}, {1, 1}, {0, 1}
        };
        const auto vanished = polygon_offset(unit_square, -0.6L);
        assert(vanished.edges.empty());
        assert(vanished.contours.empty());
        const std::vector<Point> large_square{
            {1.0e9L, 1.0e9L}, {1.0e9L + 1000, 1.0e9L},
            {1.0e9L + 1000, 1.0e9L + 1000},
            {1.0e9L, 1.0e9L + 1000},
        };
        const auto large_offset = polygon_offset(large_square, 2.0L);
        assert(large_offset.contours.size() == 1);
        verify(large_square, 2.0L, large_offset);

        const auto make_edge = [](Point first, Point second){
            PolygonOffsetEdge edge;
            edge.kind = PolygonOffsetEdgeKind::segment;
            edge.first = first;
            edge.second = second;
            return edge;
        };
        std::vector<PolygonOffsetEdge> close_components;
        for(const long double left: {0.0L, 1.0L + 1.0e-10L}){
            close_components.push_back(make_edge({left, 0}, {left + 1, 0}));
            close_components.push_back(make_edge({left + 1, 0}, {left + 1, 1}));
            close_components.push_back(make_edge({left + 1, 1}, {left, 1}));
            close_components.push_back(make_edge({left, 1}, {left, 0}));
        }
        assert(polygon_offset_detail::stitch(close_components).size() == 2);
        assert(!inward.edges.empty());
        verify(concave, -0.4L, inward);
        std::cout << "OK\n";
    }
}
