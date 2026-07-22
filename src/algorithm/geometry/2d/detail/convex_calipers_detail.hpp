#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DETAIL_CONVEX_CALIPERS_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DETAIL_CONVEX_CALIPERS_DETAIL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "../core/advanced_convex_geometry.hpp"
#include "../scalar/norm.hpp"

inline constexpr std::size_t CONVEX_POLYGON_NPOS =
    std::numeric_limits<std::size_t>::max();

class NormalizedConvexPolygon{
    std::vector<Point> vertices_;
    std::vector<long double> edge_angles_;

public:
    explicit NormalizedConvexPolygon(std::vector<Point> polygon)
        : vertices_(normalize_convex_polygon(std::move(polygon))){
        std::size_t count = vertices_.size();
        if(count >= 3){
            std::size_t start = 0;
            for(std::size_t index = 0; index < count; ++index){
                if(advanced_geometry_detail::cross_sign(
                    vertices_[index] - vertices_[(index + count - 1) % count],
                    vertices_[(index + 1) % count] - vertices_[index]
                ) > 0){
                    start = index;
                    break;
                }
            }

            std::vector<Point> strict;
            strict.reserve(count + 1);
            for(std::size_t offset = 0; offset <= count; ++offset){
                const Point& point = vertices_[(start + offset) % count];
                while(strict.size() >= 2
                    && advanced_geometry_detail::cross_sign(
                        strict.back() - strict[strict.size() - 2],
                        point - strict.back()
                    ) == 0){
                    strict.pop_back();
                }
                strict.push_back(point);
            }
            strict.pop_back();
            advanced_geometry_detail::canonicalize_start(strict);
            vertices_ = std::move(strict);
            count = vertices_.size();
        }
        if(count < 3) return;

        edge_angles_.resize(count);
        const long double two_pi = 2.0L * std::acos(-1.0L);
        for(std::size_t index = 0; index < count; ++index){
            const Point edge = vertices_[(index + 1) % count] - vertices_[index];
            long double angle = std::atan2(edge.y, edge.x);
            if(index != 0){
                while(angle <= edge_angles_[index - 1]) angle += two_pi;
            }
            edge_angles_[index] = angle;
        }
    }

    std::size_t size() const{ return vertices_.size(); }
    bool empty() const{ return vertices_.empty(); }
    const std::vector<Point>& vertices() const{ return vertices_; }
    const std::vector<long double>& edge_angles() const{ return edge_angles_; }
};

namespace rotating_calipers_detail{

inline int scalar_compare(long double first, long double second){
    return (first > second) - (first < second);
}

inline long double squared_distance(const Point& first, const Point& second){
    return norm(first - second);
}

inline long double doubled_triangle_area(
    const Point& first,
    const Point& second,
    const Point& third
){
    return std::abs(cross(second - first, third - first));
}

inline const Point& cyclic_vertex(
    const std::vector<Point>& polygon,
    std::size_t index
){
    return polygon[index % polygon.size()];
}

inline int oriented_side(
    const Point& origin,
    const Point& first,
    const Point& second
){
    return advanced_geometry_detail::cross_sign(first - origin, second - origin);
}

inline int contains_normalized_convex_polygon(
    const std::vector<Point>& polygon,
    const Point& point
){
    const std::size_t count = polygon.size();
    if(count == 0) return 0;
    if(count == 1){
        return advanced_geometry_detail::point_equal(polygon[0], point) ? 1 : 0;
    }
    if(count == 2){
        return advanced_geometry_detail::on_segment_scaled(
            {polygon[0], polygon[1]}, point
        ) ? 1 : 0;
    }

    const Point& origin = polygon[0];
    const int first_side = advanced_geometry_detail::cross_sign(
        polygon[1] - origin, point - origin
    );
    const int last_side = advanced_geometry_detail::cross_sign(
        polygon.back() - origin, point - origin
    );
    if(first_side < 0 || last_side > 0) return 0;
    if(first_side == 0){
        return advanced_geometry_detail::on_segment_scaled(
            {origin, polygon[1]}, point
        ) ? 1 : 0;
    }
    if(last_side == 0){
        return advanced_geometry_detail::on_segment_scaled(
            {origin, polygon.back()}, point
        ) ? 1 : 0;
    }

    std::size_t low = 1;
    std::size_t high = count - 1;
    while(high - low > 1){
        const std::size_t middle = low + (high - low) / 2;
        if(advanced_geometry_detail::cross_sign(
            polygon[middle] - origin, point - origin
        ) >= 0){
            low = middle;
        }else{
            high = middle;
        }
    }
    const int side = advanced_geometry_detail::cross_sign(
        polygon[low + 1] - polygon[low], point - polygon[low]
    );
    if(side == 0) return 1;
    return side > 0 ? 2 : 0;
}

}  // namespace rotating_calipers_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DETAIL_CONVEX_CALIPERS_DETAIL_HPP_INCLUDED
