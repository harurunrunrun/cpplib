#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUICKHULL_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUICKHULL_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "convex_hull_3d.hpp"
#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "is_finite.hpp"
#include "scalar_triple.hpp"

namespace quickhull_3d_detail{

inline constexpr std::size_t no_index =
    std::numeric_limits<std::size_t>::max();

struct Face{
    std::array<std::size_t, 3> vertices{};
    std::array<std::size_t, 3> neighbors{no_index, no_index, no_index};
    std::vector<std::size_t> outside;
    std::size_t farthest = no_index;
    long double farthest_magnitude = 0.0L;
    std::size_t visit_stamp = 0;
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
    Face result;
    result.vertices = {first, second, third};
    return result;
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

inline std::array<std::size_t, 2> edge_key(
    std::size_t first,
    std::size_t second
){
    if(second < first) std::swap(first, second);
    return {first, second};
}

struct EdgeHash{
    std::size_t operator()(
        const std::array<std::size_t, 2>& edge
    ) const noexcept{
        std::size_t result = edge[0] + 0x9e3779b97f4a7c15ULL;
        result ^= edge[1] + 0x9e3779b97f4a7c15ULL
            + (result << 6) + (result >> 2);
        return result;
    }
};

struct OpenEdge{
    std::size_t face = no_index;
    std::size_t edge = no_index;
};

inline void link_open_edges(
    std::vector<Face>& faces,
    const std::vector<std::size_t>& face_indices
){
    std::unordered_map<std::array<std::size_t, 2>, OpenEdge, EdgeHash> open;
    open.reserve(face_indices.size() * 2 + 1);
    for(const std::size_t face_index: face_indices){
        for(std::size_t edge = 0; edge < 3; ++edge){
            if(faces[face_index].neighbors[edge] != no_index) continue;
            const auto key = edge_key(
                faces[face_index].vertices[edge],
                faces[face_index].vertices[(edge + 1) % 3]
            );
            const auto [iterator, inserted] = open.emplace(
                key, OpenEdge{face_index, edge}
            );
            if(inserted) continue;
            const OpenEdge other = iterator->second;
            faces[face_index].neighbors[edge] = other.face;
            faces[other.face].neighbors[other.edge] = face_index;
            open.erase(iterator);
        }
    }
    if(!open.empty())[[unlikely]]{
        throw std::logic_error("quickhull_3d produced an open surface");
    }
}

inline bool sees(
    const Face& face,
    std::size_t point,
    const std::vector<Point3>& points
){
    return adaptive_orient3d(
        points[face.vertices[0]], points[face.vertices[1]],
        points[face.vertices[2]], points[point]
    ) > 0;
}

inline void refresh_candidate(
    std::vector<Face>& faces,
    std::size_t face_index,
    const std::vector<Point3>& scaled,
    std::vector<std::size_t>& candidates
){
    Face& face = faces[face_index];
    face.farthest = no_index;
    face.farthest_magnitude = 0.0L;
    for(const std::size_t point: face.outside){
        const long double value = magnitude(face, scaled[point], scaled);
        if(face.farthest == no_index || value > face.farthest_magnitude
            || (value == face.farthest_magnitude && point < face.farthest)){
            face.farthest = point;
            face.farthest_magnitude = value;
        }
    }
    if(face.farthest != no_index){
        candidates.push_back(face_index);
    }
}

struct HorizonEdge{
    std::size_t first = no_index;
    std::size_t second = no_index;
    std::size_t neighbor = no_index;
    std::size_t neighbor_edge = no_index;
};

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
    link_open_edges(faces, {0, 1, 2, 3});

    const std::set<std::size_t> initial{first, second, third, fourth};
    std::vector<std::size_t> owner(input.size(), no_index);
    for(std::size_t point = 0; point < input.size(); ++point){
        if(initial.contains(point)) continue;
        for(std::size_t face = 0; face < 4; ++face){
            if(sees(faces[face], point, input)){
                // Keeping one conflict owner per point bounds the total size
                // of all outside sets by the number of input points.
                faces[face].outside.push_back(point);
                owner[point] = face;
                break;
            }
        }
    }

    std::vector<std::size_t> candidate_faces;
    for(std::size_t face = 0; face < 4; ++face){
        refresh_candidate(faces, face, scaled, candidate_faces);
    }
    std::size_t candidate_head = 0;
    std::size_t visit_stamp = 0;

    while(candidate_head < candidate_faces.size()){
        const std::size_t selected_face = candidate_faces[candidate_head++];
        if(!faces[selected_face].alive
            || faces[selected_face].farthest == no_index) continue;
        const std::size_t expansion_point = faces[selected_face].farthest;

        ++visit_stamp;
        std::vector<std::size_t> visible{selected_face};
        faces[selected_face].visit_stamp = visit_stamp;
        for(std::size_t head = 0; head < visible.size(); ++head){
            const std::size_t face_index = visible[head];
            for(const std::size_t neighbor: faces[face_index].neighbors){
                if(neighbor == no_index || !faces[neighbor].alive
                    || faces[neighbor].visit_stamp == visit_stamp) continue;
                if(sees(faces[neighbor], expansion_point, input)){
                    faces[neighbor].visit_stamp = visit_stamp;
                    visible.push_back(neighbor);
                }
            }
        }

        std::vector<HorizonEdge> horizon;
        for(const std::size_t face_index: visible){
            const Face& face = faces[face_index];
            for(std::size_t edge = 0; edge < 3; ++edge){
                const std::size_t neighbor = face.neighbors[edge];
                if(neighbor == no_index)[[unlikely]]{
                    throw std::logic_error("quickhull_3d lost a face neighbor");
                }
                if(faces[neighbor].visit_stamp == visit_stamp) continue;
                std::size_t neighbor_edge = no_index;
                for(std::size_t index = 0; index < 3; ++index){
                    if(faces[neighbor].neighbors[index] == face_index){
                        neighbor_edge = index;
                        break;
                    }
                }
                if(neighbor_edge == no_index)[[unlikely]]{
                    throw std::logic_error(
                        "quickhull_3d has asymmetric adjacency"
                    );
                }
                horizon.push_back({
                    face.vertices[edge], face.vertices[(edge + 1) % 3],
                    neighbor, neighbor_edge,
                });
            }
        }
        if(horizon.size() < 3)[[unlikely]]{
            throw std::logic_error("quickhull_3d produced an invalid horizon");
        }

        std::vector<std::size_t> candidates;
        for(const std::size_t face_index: visible){
            Face& face = faces[face_index];
            for(const std::size_t point: face.outside){
                if(owner[point] != face_index) continue;
                owner[point] = no_index;
                if(point == expansion_point) continue;
                candidates.push_back(point);
            }
            face.outside.clear();
            face.alive = false;
        }

        std::vector<std::size_t> new_faces;
        new_faces.reserve(horizon.size());
        for(const HorizonEdge& edge: horizon){
            const std::size_t face_index = faces.size();
            faces.push_back(outward(
                edge.first, edge.second, expansion_point, interior, input
            ));
            new_faces.push_back(face_index);

            std::size_t horizon_edge = no_index;
            for(std::size_t index = 0; index < 3; ++index){
                if(edge_key(
                    faces[face_index].vertices[index],
                    faces[face_index].vertices[(index + 1) % 3]
                ) == edge_key(edge.first, edge.second)){
                    horizon_edge = index;
                    break;
                }
            }
            if(horizon_edge == no_index)[[unlikely]]{
                throw std::logic_error("quickhull_3d lost a horizon edge");
            }
            faces[face_index].neighbors[horizon_edge] = edge.neighbor;
            faces[edge.neighbor].neighbors[edge.neighbor_edge] = face_index;
        }
        link_open_edges(faces, new_faces);

        for(const std::size_t point: candidates){
            for(const std::size_t face_index: new_faces){
                if(sees(faces[face_index], point, input)){
                    faces[face_index].outside.push_back(point);
                    owner[point] = face_index;
                    break;
                }
            }
        }
        for(const std::size_t face_index: new_faces){
            refresh_candidate(faces, face_index, scaled, candidate_faces);
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUICKHULL_3D_HPP_INCLUDED
