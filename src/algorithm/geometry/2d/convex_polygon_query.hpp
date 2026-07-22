#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_QUERY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_QUERY_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "advanced/detail.hpp"
#include "on_segment.hpp"
#include "side_of_directed_line.hpp"

class ConvexPolygonQuery{
    std::vector<Point> polygon_;

    static int resolved_side(
        const Point& first,
        const Point& second,
        const Point& point
    ){
        const Point direction = second - first;
        const long double value = cross(direction, point - first);
        if(geometry_sign(abs(direction)) != 0){
            const int side = side_of_directed_line({first, second}, point);
            if(side != 0) return side;
        }
        return (value > 0.0L) - (value < 0.0L);
    }

    static std::vector<Point> normalize(std::vector<Point> polygon){
        std::size_t write = 0;
        for(const Point& point: polygon){
            if(write == 0 || !(polygon[write - 1] == point)){
                polygon[write++] = point;
            }
        }
        polygon.resize(write);
        if(polygon.size() >= 2 && polygon.front() == polygon.back()){
            polygon.pop_back();
        }
        if(polygon.size() <= 2) return polygon;

        const int size = static_cast<int>(polygon.size());
        int orientation = 0;
        for(int index = 0; index < size; ++index){
            orientation = advanced_geometry_detail::cross_sign(
                polygon[static_cast<std::size_t>((index + 1) % size)] -
                    polygon[static_cast<std::size_t>(index)],
                polygon[static_cast<std::size_t>((index + 2) % size)] -
                    polygon[static_cast<std::size_t>((index + 1) % size)]
            );
            if(orientation != 0) break;
        }
        if(orientation == 0){
            const auto [minimum, maximum] =
                std::minmax_element(polygon.begin(), polygon.end());
            if(*minimum == *maximum) return {*minimum};
            return {*minimum, *maximum};
        }
        if(orientation < 0) std::reverse(polygon.begin(), polygon.end());

        int start = 0;
        for(int index = 0; index < size; ++index){
            const Point& previous = polygon[
                static_cast<std::size_t>((index + size - 1) % size)
            ];
            const Point& current = polygon[static_cast<std::size_t>(index)];
            const Point& next = polygon[
                static_cast<std::size_t>((index + 1) % size)
            ];
            if(advanced_geometry_detail::cross_sign(
                current - previous, next - current
            ) > 0){
                start = index;
                break;
            }
        }

        std::vector<Point> result;
        result.reserve(polygon.size() + 1);
        for(int offset = 0; offset <= size; ++offset){
            const Point& point = polygon[
                static_cast<std::size_t>((start + offset) % size)
            ];
            while(result.size() >= 2 &&
                advanced_geometry_detail::cross_sign(
                    result.back() - result[result.size() - 2],
                    point - result.back()
                ) == 0){
                result.pop_back();
            }
            result.push_back(point);
        }
        result.pop_back();
        return result;
    }

public:
    explicit ConvexPolygonQuery(std::vector<Point> polygon)
        : polygon_(normalize(std::move(polygon))){}

    int size() const{ return static_cast<int>(polygon_.size()); }
    const std::vector<Point>& vertices() const{ return polygon_; }

    int contains(const Point& point) const{
        const int count = size();
        if(count == 0) return 0;
        if(count == 1) return polygon_[0] == point ? 1 : 0;
        if(count == 2){
            return on_segment({polygon_[0], polygon_[1]}, point) ? 1 : 0;
        }

        const Point& origin = polygon_[0];
        const Segment first_edge{origin, polygon_[1]};
        const Segment last_edge{
            origin, polygon_[static_cast<std::size_t>(count - 1)]
        };
        if(on_segment(first_edge, point) || on_segment(last_edge, point)){
            return 1;
        }
        const int first_side = resolved_side(
            first_edge.a, first_edge.b, point
        );
        const int last_side = resolved_side(
            last_edge.a, last_edge.b, point
        );
        if(first_side < 0 || last_side > 0) return 0;
        if(first_side == 0 || last_side == 0) return 0;

        int low = 1;
        int high = count - 1;
        while(high - low > 1){
            const int middle = low + (high - low) / 2;
            if(resolved_side(
                origin,
                polygon_[static_cast<std::size_t>(middle)],
                point
            ) >= 0){
                low = middle;
            }else{
                high = middle;
            }
        }
        const Segment edge{
            polygon_[static_cast<std::size_t>(low)],
            polygon_[static_cast<std::size_t>(low + 1)]
        };
        if(on_segment(edge, point)) return 1;
        const int side = resolved_side(edge.a, edge.b, point);
        return side > 0 ? 2 : 0;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_QUERY_HPP_INCLUDED
