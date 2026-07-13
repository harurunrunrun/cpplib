#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "cross.hpp"
#include "on_segment.hpp"

class ConvexPolygonQuery{
    std::vector<Point> polygon_;

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
            orientation = geometry_sign(cross(
                polygon[static_cast<std::size_t>((index + 1) % size)] -
                    polygon[static_cast<std::size_t>(index)],
                polygon[static_cast<std::size_t>((index + 2) % size)] -
                    polygon[static_cast<std::size_t>((index + 1) % size)]
            ));
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
            if(geometry_sign(cross(current - previous, next - current)) > 0){
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
            while(result.size() >= 2 && geometry_sign(cross(
                result.back() - result[result.size() - 2],
                point - result.back()
            )) == 0){
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
        const int first_side = geometry_sign(cross(
            polygon_[1] - origin, point - origin
        ));
        const int last_side = geometry_sign(cross(
            polygon_[static_cast<std::size_t>(count - 1)] - origin,
            point - origin
        ));
        if(first_side < 0 || last_side > 0) return 0;
        if(first_side == 0){
            return on_segment({origin, polygon_[1]}, point) ? 1 : 0;
        }
        if(last_side == 0){
            return on_segment({
                origin, polygon_[static_cast<std::size_t>(count - 1)]
            }, point) ? 1 : 0;
        }

        int low = 1;
        int high = count - 1;
        while(high - low > 1){
            const int middle = low + (high - low) / 2;
            if(geometry_sign(cross(
                polygon_[static_cast<std::size_t>(middle)] - origin,
                point - origin
            )) >= 0){
                low = middle;
            }else{
                high = middle;
            }
        }
        const int side = geometry_sign(cross(
            polygon_[static_cast<std::size_t>(low + 1)] -
                polygon_[static_cast<std::size_t>(low)],
            point - polygon_[static_cast<std::size_t>(low)]
        ));
        if(side == 0) return 1;
        return side > 0 ? 2 : 0;
    }
};
