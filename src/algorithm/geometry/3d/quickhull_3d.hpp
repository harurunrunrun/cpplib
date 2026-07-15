#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "convex_hull_3d.hpp"
#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "is_finite.hpp"
#include "scalar_triple.hpp"

namespace quickhull_3d_detail{

struct Face{
    std::array<std::size_t, 3> vertices{};
    bool alive = true;
};

inline Face outward(
    std::size_t first,
    std::size_t second,
    std::size_t third,
    const Point3& interior,
    const std::vector<Point3>& points
){
    if(adaptive_orient3d(
        points[first], points[second], points[third], interior
    ) > 0) std::swap(second, third);
    return {{{first, second, third}}, true};
}

inline long double magnitude(
    const Face& face,
    const Point3& point,
    const std::vector<Point3>& scaled
){
    return std::abs(scalar_triple(
        scaled[face.vertices[1]] - scaled[face.vertices[0]],
        scaled[face.vertices[2]] - scaled[face.vertices[0]],
        point - scaled[face.vertices[0]]
    ));
}

}  // namespace quickhull_3d_detail

inline ConvexPolyhedron3 quickhull_3d(std::vector<Point3> input){
    using namespace quickhull_3d_detail;
    for(const Point3& point: input){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("quickhull_3d requires finite points");
        }
    }
    std::sort(input.begin(), input.end());
    input.erase(std::unique(input.begin(), input.end(), [](const Point3& a,
                                                           const Point3& b){
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }), input.end());
    if(input.size() < 4) return convex_hull_3d(std::move(input));

    long double scale = 0.0L;
    for(const Point3& point: input){
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L) return convex_hull_3d(std::move(input));
    std::vector<Point3> scaled;
    scaled.reserve(input.size());
    for(const Point3& point: input) scaled.push_back(point / scale);

    const std::size_t first = 0;
    const std::size_t second = input.size() - 1;
    std::size_t third = input.size();
    long double best_line = 0.0L;
    for(std::size_t index = 1; index + 1 < input.size(); ++index){
        const Point3 product = cross(
            scaled[second] - scaled[first], scaled[index] - scaled[first]
        );
        const long double value = std::hypot(product.x, product.y, product.z);
        if(value > best_line){
            best_line = value;
            third = index;
        }
    }
    if(third == input.size() || best_line == 0.0L){
        return convex_hull_3d(std::move(input));
    }
    std::size_t fourth = input.size();
    long double best_plane = 0.0L;
    for(std::size_t index = 0; index < input.size(); ++index){
        if(index == first || index == second || index == third) continue;
        if(adaptive_orient3d(
            input[first], input[second], input[third], input[index]
        ) == 0) continue;
        const long double value = std::abs(scalar_triple(
            scaled[second] - scaled[first], scaled[third] - scaled[first],
            scaled[index] - scaled[first]
        ));
        if(value > best_plane){
            best_plane = value;
            fourth = index;
        }
    }
    if(fourth == input.size()) return convex_hull_3d(std::move(input));

    const Point3 interior = (
        scaled[first] + scaled[second] + scaled[third] + scaled[fourth]
    ) * (scale / 4.0L);
    std::vector<Face> faces{
        outward(first, second, third, interior, input),
        outward(first, fourth, second, interior, input),
        outward(first, third, fourth, interior, input),
        outward(second, fourth, third, interior, input),
    };

    while(true){
        std::size_t expansion_point = input.size();
        long double farthest = 0.0L;
        for(const Face& face: faces){
            if(!face.alive) continue;
            for(std::size_t index = 0; index < input.size(); ++index){
                if(adaptive_orient3d(
                    input[face.vertices[0]], input[face.vertices[1]],
                    input[face.vertices[2]], input[index]
                ) <= 0) continue;
                const long double value = magnitude(face, scaled[index], scaled);
                if(expansion_point == input.size() or value > farthest){
                    farthest = value;
                    expansion_point = index;
                }
            }
        }
        if(expansion_point == input.size()) break;

        std::map<std::array<std::size_t, 2>, std::array<std::size_t, 3>> horizon;
        for(Face& face: faces){
            if(!face.alive || adaptive_orient3d(
                input[face.vertices[0]], input[face.vertices[1]],
                input[face.vertices[2]], input[expansion_point]
            ) <= 0) continue;
            face.alive = false;
            for(std::size_t edge_index = 0; edge_index < 3; ++edge_index){
                const std::size_t edge_first = face.vertices[edge_index];
                const std::size_t edge_second = face.vertices[(edge_index + 1) % 3];
                std::array<std::size_t, 2> key{edge_first, edge_second};
                if(key[1] < key[0]) std::swap(key[0], key[1]);
                auto& record = horizon[key];
                if(record[2] == 0){
                    record[0] = edge_first;
                    record[1] = edge_second;
                }
                ++record[2];
            }
        }
        for(const auto& [key, edge]: horizon){
            static_cast<void>(key);
            if(edge[2] == 1){
                faces.push_back(outward(
                    edge[0], edge[1], expansion_point, interior, input
                ));
            }
        }
    }

    std::set<std::size_t> used;
    for(const Face& face: faces){
        if(face.alive) used.insert(face.vertices.begin(), face.vertices.end());
    }
    std::vector<std::size_t> remap(input.size(), input.size());
    std::vector<Point3> vertices;
    for(std::size_t index: used){
        remap[index] = vertices.size();
        vertices.push_back(input[index]);
    }
    std::vector<std::array<std::size_t, 3>> result_faces;
    for(const Face& face: faces){
        if(face.alive) result_faces.push_back({
            remap[face.vertices[0]], remap[face.vertices[1]],
            remap[face.vertices[2]],
        });
    }
    return {3, std::move(vertices), std::move(result_faces)};
}
