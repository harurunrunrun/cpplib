#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "aabb3.hpp"
#include "base.hpp"
#include "is_finite.hpp"

class TriangleBVH3D{
    struct Node{
        AABB3 box{};
        std::size_t begin = 0;
        std::size_t end = 0;
        int left = -1;
        int right = -1;

        bool leaf() const noexcept{ return left < 0; }
    };

    std::vector<Triangle3> triangles_;
    std::vector<AABB3> triangle_boxes_;
    std::vector<std::size_t> order_;
    std::vector<Node> nodes_;
    std::size_t leaf_capacity_ = 4;
    int root_ = -1;

    static Point3 subtract(const Point3& left, const Point3& right){
        return {left.x - right.x, left.y - right.y, left.z - right.z};
    }

    static Point3 cross_product(const Point3& left, const Point3& right){
        return {
            left.y * right.z - left.z * right.y,
            left.z * right.x - left.x * right.z,
            left.x * right.y - left.y * right.x,
        };
    }

    static long double dot_product(const Point3& left, const Point3& right){
        return left.x * right.x + left.y * right.y + left.z * right.z;
    }

    static long double coordinate(const Point3& point, std::size_t axis){
        if(axis == 0) return point.x;
        if(axis == 1) return point.y;
        return point.z;
    }

    static AABB3 triangle_box(const Triangle3& triangle){
        return {
            {
                std::min({triangle.a.x, triangle.b.x, triangle.c.x}),
                std::min({triangle.a.y, triangle.b.y, triangle.c.y}),
                std::min({triangle.a.z, triangle.b.z, triangle.c.z}),
            },
            {
                std::max({triangle.a.x, triangle.b.x, triangle.c.x}),
                std::max({triangle.a.y, triangle.b.y, triangle.c.y}),
                std::max({triangle.a.z, triangle.b.z, triangle.c.z}),
            },
        };
    }

    static Point3 triangle_centroid(const Triangle3& triangle){
        return {
            triangle.a.x / 3 + triangle.b.x / 3 + triangle.c.x / 3,
            triangle.a.y / 3 + triangle.b.y / 3 + triangle.c.y / 3,
            triangle.a.z / 3 + triangle.b.z / 3 + triangle.c.z / 3,
        };
    }

    int build_node(std::size_t begin, std::size_t end){
        Point3 minimum{
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
        };
        Point3 maximum{-minimum.x, -minimum.y, -minimum.z};
        Point3 centroid_minimum = minimum;
        Point3 centroid_maximum = maximum;
        for(std::size_t position = begin; position < end; ++position){
            const std::size_t index = order_[position];
            const AABB3& box = triangle_boxes_[index];
            minimum.x = std::min(minimum.x, box.minimum.x);
            minimum.y = std::min(minimum.y, box.minimum.y);
            minimum.z = std::min(minimum.z, box.minimum.z);
            maximum.x = std::max(maximum.x, box.maximum.x);
            maximum.y = std::max(maximum.y, box.maximum.y);
            maximum.z = std::max(maximum.z, box.maximum.z);
            const Point3 centroid = triangle_centroid(triangles_[index]);
            centroid_minimum.x = std::min(centroid_minimum.x, centroid.x);
            centroid_minimum.y = std::min(centroid_minimum.y, centroid.y);
            centroid_minimum.z = std::min(centroid_minimum.z, centroid.z);
            centroid_maximum.x = std::max(centroid_maximum.x, centroid.x);
            centroid_maximum.y = std::max(centroid_maximum.y, centroid.y);
            centroid_maximum.z = std::max(centroid_maximum.z, centroid.z);
        }
        const int node_index = static_cast<int>(nodes_.size());
        nodes_.push_back(Node{});
        nodes_[static_cast<std::size_t>(node_index)].box = {minimum, maximum};
        nodes_[static_cast<std::size_t>(node_index)].begin = begin;
        nodes_[static_cast<std::size_t>(node_index)].end = end;
        if(end - begin <= leaf_capacity_) return node_index;

        const std::array<long double, 3> spread{{
            centroid_maximum.x - centroid_minimum.x,
            centroid_maximum.y - centroid_minimum.y,
            centroid_maximum.z - centroid_minimum.z,
        }};
        std::size_t axis = 0;
        if(spread[1] > spread[axis]) axis = 1;
        if(spread[2] > spread[axis]) axis = 2;
        const std::size_t middle = begin + (end - begin) / 2;
        std::nth_element(
            order_.begin() + static_cast<std::ptrdiff_t>(begin),
            order_.begin() + static_cast<std::ptrdiff_t>(middle),
            order_.begin() + static_cast<std::ptrdiff_t>(end),
            [&](std::size_t left, std::size_t right){
                const long double lhs = coordinate(triangle_centroid(triangles_[left]), axis);
                const long double rhs = coordinate(triangle_centroid(triangles_[right]), axis);
                return lhs != rhs ? lhs < rhs : left < right;
            }
        );
        const int left = build_node(begin, middle);
        const int right = build_node(middle, end);
        nodes_[static_cast<std::size_t>(node_index)].left = left;
        nodes_[static_cast<std::size_t>(node_index)].right = right;
        return node_index;
    }

    void aabb_search(
        int node_index,
        const AABB3& range,
        std::vector<std::size_t>& result
    ) const{
        if(node_index < 0) return;
        const Node& node = nodes_[static_cast<std::size_t>(node_index)];
        if(!node.box.intersects(range)) return;
        if(node.leaf()){
            for(std::size_t position = node.begin; position < node.end; ++position){
                const std::size_t triangle_index = order_[position];
                if(triangle_boxes_[triangle_index].intersects(range)){
                    result.push_back(triangle_index);
                }
            }
            return;
        }
        aabb_search(node.left, range, result);
        aabb_search(node.right, range, result);
    }

    static std::optional<long double> ray_box_entry(
        const Point3& origin,
        const Point3& direction,
        const AABB3& box,
        long double lower,
        long double upper
    ){
        for(std::size_t axis = 0; axis < 3; ++axis){
            const long double value = coordinate(origin, axis);
            const long double delta = coordinate(direction, axis);
            const long double minimum = coordinate(box.minimum, axis);
            const long double maximum = coordinate(box.maximum, axis);
            if(delta == 0){
                if(value < minimum || maximum < value) return std::nullopt;
                continue;
            }
            long double first = (minimum - value) / delta;
            long double second = (maximum - value) / delta;
            if(second < first) std::swap(first, second);
            lower = std::max(lower, first);
            upper = std::min(upper, second);
            if(upper < lower) return std::nullopt;
        }
        return lower;
    }

    static std::optional<std::pair<long double, Point3>> ray_triangle_hit(
        const Point3& origin,
        const Point3& direction,
        const Triangle3& triangle,
        long double lower,
        long double upper
    ){
        const Point3 edge1 = subtract(triangle.b, triangle.a);
        const Point3 edge2 = subtract(triangle.c, triangle.a);
        const Point3 cross_direction = cross_product(direction, edge2);
        const long double determinant = dot_product(edge1, cross_direction);
        const long double edge1_length = std::hypot(edge1.x, edge1.y, edge1.z);
        const long double cross_length = std::hypot(
            cross_direction.x, cross_direction.y, cross_direction.z
        );
        const long double tolerance = 64 * std::numeric_limits<long double>::epsilon()
            * edge1_length * cross_length;
        if(!std::isfinite(determinant) || std::abs(determinant) <= tolerance){
            return std::nullopt;
        }
        const Point3 offset = subtract(origin, triangle.a);
        const long double u = dot_product(offset, cross_direction) / determinant;
        const long double barycentric_tolerance = 128
            * std::numeric_limits<long double>::epsilon();
        if(u < -barycentric_tolerance || u > 1 + barycentric_tolerance){
            return std::nullopt;
        }
        const Point3 offset_cross = cross_product(offset, edge1);
        const long double v = dot_product(direction, offset_cross) / determinant;
        if(v < -barycentric_tolerance || u + v > 1 + barycentric_tolerance){
            return std::nullopt;
        }
        const long double parameter = dot_product(edge2, offset_cross) / determinant;
        if(parameter < lower || upper < parameter || !std::isfinite(parameter)){
            return std::nullopt;
        }
        const Point3 point = triangle.a + edge1 * u + edge2 * v;
        if(!geometry3d_is_finite(point)) return std::nullopt;
        return std::pair{parameter, point};
    }

    void ray_search(
        int node_index,
        const Point3& origin,
        const Point3& direction,
        long double lower,
        long double& best_parameter,
        std::size_t& best_index,
        Point3& best_point
    ) const{
        if(node_index < 0) return;
        const Node& node = nodes_[static_cast<std::size_t>(node_index)];
        if(!ray_box_entry(origin, direction, node.box, lower, best_parameter)) return;
        if(node.leaf()){
            for(std::size_t position = node.begin; position < node.end; ++position){
                const std::size_t triangle_index = order_[position];
                const auto hit = ray_triangle_hit(
                    origin, direction, triangles_[triangle_index], lower, best_parameter
                );
                if(hit && (hit->first < best_parameter
                   || (hit->first == best_parameter && triangle_index < best_index))){
                    best_parameter = hit->first;
                    best_index = triangle_index;
                    best_point = hit->second;
                }
            }
            return;
        }
        const auto left_entry = ray_box_entry(
            origin, direction, nodes_[static_cast<std::size_t>(node.left)].box,
            lower, best_parameter
        );
        const auto right_entry = ray_box_entry(
            origin, direction, nodes_[static_cast<std::size_t>(node.right)].box,
            lower, best_parameter
        );
        int first = node.left;
        int second = node.right;
        if(!left_entry || (right_entry && *right_entry < *left_entry)){
            std::swap(first, second);
        }
        ray_search(first, origin, direction, lower, best_parameter, best_index, best_point);
        ray_search(second, origin, direction, lower, best_parameter, best_index, best_point);
    }

public:
    TriangleBVH3D() = default;

    explicit TriangleBVH3D(
        const std::vector<Triangle3>& triangles,
        std::size_t leaf_capacity = 4
    ): leaf_capacity_(leaf_capacity){
        if(leaf_capacity_ == 0){
            throw std::invalid_argument("TriangleBVH3D leaf capacity must be positive");
        }
        build(triangles);
    }

    void build(const std::vector<Triangle3>& triangles){
        for(const Triangle3& triangle: triangles){
            if(!geometry3d_is_finite(triangle)){
                throw std::invalid_argument("TriangleBVH3D triangles must be finite");
            }
        }
        triangles_ = triangles;
        triangle_boxes_.clear();
        triangle_boxes_.reserve(size());
        for(const Triangle3& triangle: triangles_) triangle_boxes_.push_back(triangle_box(triangle));
        order_.resize(size());
        std::iota(order_.begin(), order_.end(), std::size_t{0});
        nodes_.clear();
        nodes_.reserve(size() * 2);
        root_ = empty() ? -1 : build_node(0, size());
    }

    std::size_t size() const noexcept{ return triangles_.size(); }
    bool empty() const noexcept{ return triangles_.empty(); }
    const Triangle3& triangle(std::size_t index) const{ return triangles_.at(index); }

    std::vector<std::size_t> aabb_query(const AABB3& range) const{
        range.require_valid();
        std::vector<std::size_t> result;
        aabb_search(root_, range, result);
        std::sort(result.begin(), result.end());
        return result;
    }

    std::optional<std::tuple<std::size_t, long double, Point3>> ray_cast(
        const Ray3& ray,
        long double minimum_parameter = 0,
        long double maximum_parameter = std::numeric_limits<long double>::infinity()
    ) const{
        if(!geometry3d_is_finite(ray)){
            throw std::invalid_argument("TriangleBVH3D ray must be finite");
        }
        const Point3 direction = subtract(ray.through, ray.origin);
        if(std::hypot(direction.x, direction.y, direction.z) == 0){
            throw std::invalid_argument("TriangleBVH3D ray must be nondegenerate");
        }
        if(!std::isfinite(minimum_parameter) || minimum_parameter < 0
           || std::isnan(maximum_parameter) || maximum_parameter < minimum_parameter){
            throw std::invalid_argument("invalid TriangleBVH3D ray parameter range");
        }
        if(empty()) return std::nullopt;
        long double best_parameter = maximum_parameter;
        std::size_t best_index = size();
        Point3 best_point{};
        ray_search(
            root_, ray.origin, direction, minimum_parameter,
            best_parameter, best_index, best_point
        );
        if(best_index == size()) return std::nullopt;
        return std::tuple{best_index, best_parameter, best_point};
    }
};

