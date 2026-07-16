#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../2d/convex_hull.hpp"
#include "adaptive_orient3d.hpp"
#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "norm.hpp"
#include "plane_coordinate_system.hpp"
#include "to_plane_coordinates.hpp"

namespace convex_hull_3d_detail{

inline constexpr std::size_t no_index =
    std::numeric_limits<std::size_t>::max();
inline constexpr std::uint64_t default_random_seed =
    0x6a09e667f3bcc909ULL;

struct Face{
    std::array<std::size_t, 3> vertices{};
    bool alive = true;
    std::size_t conflict_head = no_index;
};

struct ConflictGraph{
    struct Entry{
        std::size_t point = no_index;
        std::size_t face = no_index;
        std::size_t previous_point = no_index;
        std::size_t next_point = no_index;
        std::size_t previous_face = no_index;
        std::size_t next_face = no_index;
    };

    explicit ConflictGraph(std::size_t point_count):
        point_heads(point_count, no_index){}

    void add(
        std::size_t point,
        std::size_t face,
        std::vector<Face>& faces
    ){
        std::size_t index;
        if(free_entries.empty()){
            index = entries.size();
            entries.push_back({});
        }else{
            index = free_entries.back();
            free_entries.pop_back();
        }
        entries[index] = {
            point, face,
            no_index, point_heads[point],
            no_index, faces[face].conflict_head,
        };
        if(point_heads[point] != no_index){
            entries[point_heads[point]].previous_point = index;
        }
        if(faces[face].conflict_head != no_index){
            entries[faces[face].conflict_head].previous_face = index;
        }
        point_heads[point] = index;
        faces[face].conflict_head = index;
    }

    template<class Function>
    void for_each_face_point(
        std::size_t face,
        const std::vector<Face>& faces,
        Function function
    ) const{
        for(std::size_t entry = faces[face].conflict_head;
            entry != no_index; entry = entries[entry].next_face){
            function(entries[entry].point);
        }
    }

    std::vector<std::size_t> incident_faces(std::size_t point) const{
        std::vector<std::size_t> result;
        for(std::size_t entry = point_heads[point]; entry != no_index;
            entry = entries[entry].next_point){
            result.push_back(entries[entry].face);
        }
        return result;
    }

    void remove_face(std::size_t face, std::vector<Face>& faces){
        while(faces[face].conflict_head != no_index){
            remove(faces[face].conflict_head, faces);
        }
    }

private:
    void remove(std::size_t index, std::vector<Face>& faces){
        const Entry entry = entries[index];
        if(entry.previous_point == no_index){
            point_heads[entry.point] = entry.next_point;
        }else{
            entries[entry.previous_point].next_point = entry.next_point;
        }
        if(entry.next_point != no_index){
            entries[entry.next_point].previous_point = entry.previous_point;
        }
        if(entry.previous_face == no_index){
            faces[entry.face].conflict_head = entry.next_face;
        }else{
            entries[entry.previous_face].next_face = entry.next_face;
        }
        if(entry.next_face != no_index){
            entries[entry.next_face].previous_face = entry.previous_face;
        }
        entries[index] = {};
        free_entries.push_back(index);
    }

    std::vector<Entry> entries;
    std::vector<std::size_t> free_entries;
    std::vector<std::size_t> point_heads;
};

using EdgeKey = std::pair<std::size_t, std::size_t>;

inline EdgeKey edge_key(std::size_t first, std::size_t second){
    if(second < first) std::swap(first, second);
    return {first, second};
}

struct EdgeIncidence{
    std::size_t first = no_index;
    std::size_t second = no_index;
};

inline void attach_face(
    std::map<EdgeKey, EdgeIncidence>& incidences,
    const Face& face,
    std::size_t face_index
){
    for(std::size_t edge = 0; edge < 3; ++edge){
        EdgeIncidence& incidence = incidences[edge_key(
            face.vertices[edge], face.vertices[(edge + 1) % 3]
        )];
        if(incidence.first == no_index){
            incidence.first = face_index;
        }else if(incidence.second == no_index){
            incidence.second = face_index;
        }else{
            throw std::logic_error("convex hull edge has more than two faces");
        }
    }
}

inline void detach_face(
    std::map<EdgeKey, EdgeIncidence>& incidences,
    const Face& face,
    std::size_t face_index
){
    for(std::size_t edge = 0; edge < 3; ++edge){
        const EdgeKey key = edge_key(
            face.vertices[edge], face.vertices[(edge + 1) % 3]
        );
        auto iterator = incidences.find(key);
        if(iterator == incidences.end())[[unlikely]]{
            throw std::logic_error("convex hull edge incidence is missing");
        }
        EdgeIncidence& incidence = iterator->second;
        if(incidence.first == face_index){
            incidence.first = incidence.second;
            incidence.second = no_index;
        }else if(incidence.second == face_index){
            incidence.second = no_index;
        }else[[unlikely]]{
            throw std::logic_error("convex hull face incidence is missing");
        }
        if(incidence.first == no_index) incidences.erase(iterator);
    }
}

inline std::size_t other_incident_face(
    const EdgeIncidence& incidence,
    std::size_t face
){
    if(incidence.first == face) return incidence.second;
    if(incidence.second == face) return incidence.first;
    return no_index;
}

inline std::uint64_t splitmix64(std::uint64_t& state){
    state += 0x9e3779b97f4a7c15ULL;
    std::uint64_t value = state;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

inline std::size_t random_below(std::uint64_t& state, std::size_t bound){
    const std::uint64_t modulus = static_cast<std::uint64_t>(bound);
    const std::uint64_t threshold = (std::uint64_t{0} - modulus) % modulus;
    std::uint64_t value;
    do{
        value = splitmix64(state);
    }while(value < threshold);
    return static_cast<std::size_t>(value % modulus);
}

inline void shuffle_indices(
    std::vector<std::size_t>& indices,
    std::uint64_t seed
){
    for(std::size_t size = indices.size(); size > 1; --size){
        std::swap(indices[size - 1], indices[random_below(seed, size)]);
    }
}

inline std::vector<Point3> unique_points(std::vector<Point3> points){
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("convex_hull_3d requires finite points");
        }
    }
    std::sort(points.begin(), points.end());
    points.erase(std::unique(
        points.begin(), points.end(),
        [](const Point3& left, const Point3& right){
            return left.x == right.x && left.y == right.y && left.z == right.z;
        }
    ), points.end());
    return points;
}

inline bool exactly_non_collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    using namespace geometry3d_adaptive_detail;
    const std::array<ExactDyadic, 3> first_direction{
        subtract(exact_dyadic(second.x), exact_dyadic(first.x)),
        subtract(exact_dyadic(second.y), exact_dyadic(first.y)),
        subtract(exact_dyadic(second.z), exact_dyadic(first.z)),
    };
    const std::array<ExactDyadic, 3> second_direction{
        subtract(exact_dyadic(third.x), exact_dyadic(first.x)),
        subtract(exact_dyadic(third.y), exact_dyadic(first.y)),
        subtract(exact_dyadic(third.z), exact_dyadic(first.z)),
    };
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        const std::size_t next = (coordinate + 1) % 3;
        const std::size_t last = (coordinate + 2) % 3;
        if(sign(subtract(
            multiply(first_direction[next], second_direction[last]),
            multiply(first_direction[last], second_direction[next])
        )) != 0) return true;
    }
    return false;
}

inline std::vector<Point3> locally_normalized_points(
    const std::vector<Point3>& points,
    const Point3& anchor
){
    std::vector<Geometry3DNormalizedDifference> differences;
    differences.reserve(points.size());
    long double local_scale = 0.0L;
    for(const Point3& point: points){
        differences.push_back(geometry3d_normalized_difference(point, anchor));
        const Point3& value = differences.back().value;
        if(value.x != 0.0L || value.y != 0.0L || value.z != 0.0L){
            local_scale = std::max(local_scale, differences.back().scale);
        }
    }
    if(local_scale == 0.0L) local_scale = 1.0L;
    std::vector<Point3> result;
    result.reserve(points.size());
    for(const Geometry3DNormalizedDifference& difference: differences){
        result.push_back(
            difference.value * (difference.scale / local_scale)
        );
    }
    return result;
}

inline Face outward_face(
    std::size_t first,
    std::size_t second,
    std::size_t third,
    const std::array<std::size_t, 4>& interior_witness,
    const std::vector<Point3>& points
){
    for(const std::size_t witness: interior_witness){
        const int side = adaptive_orient3d(
            points[first], points[second], points[third], points[witness]
        );
        if(side != 0){
            if(side > 0) std::swap(second, third);
            return {{{first, second, third}}, true};
        }
    }
    throw std::logic_error("convex hull produced a degenerate face");
}

inline ConvexPolyhedron3 collinear_hull(const std::vector<Point3>& points){
    if(points.size() <= 1) return {
        points.empty() ? -1 : 0, points, {}
    };
    // Lexicographic order is the line-parameter order, possibly reversed.
    return {1, {points.front(), points.back()}, {}};
}

inline ConvexPolyhedron3 coplanar_hull(
    const std::vector<Point3>& points,
    const std::vector<Point3>& scaled,
    std::size_t first,
    std::size_t second,
    std::size_t third
){
    const Point3 normal = cross(
        scaled[second] - scaled[first], scaled[third] - scaled[first]
    );
    const PlaneCoordinateSystem3 system = plane_coordinate_system({
        scaled[first], normal
    });
    std::vector<Point> projected;
    projected.reserve(points.size());
    std::map<std::pair<long double, long double>, std::size_t> source_indices;
    for(std::size_t index = 0; index < scaled.size(); ++index){
        const Point point = to_plane_coordinates(system, scaled[index]);
        projected.push_back(point);
        // unique_points sorted points lexicographically. Keeping the first
        // index preserves the former find_if representative rule even if two
        // projections happen to be exactly equal.
        source_indices.try_emplace(
            std::pair{point.x, point.y}, index
        );
    }
    const std::vector<Point> boundary = convex_hull(projected);
    std::vector<Point3> vertices;
    vertices.reserve(boundary.size());
    for(const Point& point: boundary){
        const auto iterator = source_indices.find({point.x, point.y});
        if(iterator == source_indices.end())[[unlikely]]{
            throw std::logic_error("coplanar hull projection mismatch");
        }
        vertices.push_back(points[iterator->second]);
    }
    std::vector<std::array<std::size_t, 3>> faces;
    for(std::size_t index = 1; index + 1 < vertices.size(); ++index){
        faces.push_back({0, index, index + 1});
    }
    return {2, std::move(vertices), std::move(faces)};
}

inline ConvexPolyhedron3 finish_spatial_hull(
    const std::vector<Point3>& points,
    const std::vector<Face>& faces
){
    std::set<std::size_t> used;
    for(const Face& face: faces){
        if(face.alive) used.insert(
            face.vertices.begin(), face.vertices.end()
        );
    }
    std::vector<std::size_t> remap(points.size(), points.size());
    std::vector<Point3> vertices;
    vertices.reserve(used.size());
    for(const std::size_t point: used){
        remap[point] = vertices.size();
        vertices.push_back(points[point]);
    }
    std::vector<std::array<std::size_t, 3>> result_faces;
    result_faces.reserve(faces.size());
    for(const Face& face: faces){
        if(face.alive) result_faces.push_back({
            remap[face.vertices[0]], remap[face.vertices[1]],
            remap[face.vertices[2]],
        });
    }
    return {3, std::move(vertices), std::move(result_faces)};
}

inline ConvexPolyhedron3 scan_incremental_spatial_hull(
    const std::vector<Point3>& points,
    const std::array<std::size_t, 4>& interior_witness
){
    const auto [first, second, third, fourth] = interior_witness;
    std::vector<Face> faces{
        outward_face(first, second, third, interior_witness, points),
        outward_face(first, fourth, second, interior_witness, points),
        outward_face(first, third, fourth, interior_witness, points),
        outward_face(second, fourth, third, interior_witness, points),
    };
    std::vector<bool> initial(points.size(), false);
    for(const std::size_t point: interior_witness) initial[point] = true;
    for(std::size_t point = 0; point < points.size(); ++point){
        if(initial[point]) continue;
        std::vector<std::size_t> visible;
        for(std::size_t face = 0; face < faces.size(); ++face){
            if(faces[face].alive && adaptive_orient3d(
                points[faces[face].vertices[0]],
                points[faces[face].vertices[1]],
                points[faces[face].vertices[2]], points[point]
            ) > 0) visible.push_back(face);
        }
        if(visible.empty()) continue;
        struct HorizonRecord{
            std::size_t first = no_index;
            std::size_t second = no_index;
            std::size_t count = 0;
        };
        std::map<EdgeKey, HorizonRecord> horizon;
        for(const std::size_t face_index: visible){
            Face& face = faces[face_index];
            face.alive = false;
            for(std::size_t edge = 0; edge < 3; ++edge){
                const std::size_t first_vertex = face.vertices[edge];
                const std::size_t second_vertex =
                    face.vertices[(edge + 1) % 3];
                HorizonRecord& record = horizon[edge_key(
                    first_vertex, second_vertex
                )];
                if(record.count == 0){
                    record.first = first_vertex;
                    record.second = second_vertex;
                }
                ++record.count;
            }
        }
        for(const auto& [key, edge]: horizon){
            static_cast<void>(key);
            if(edge.count == 1) faces.push_back(outward_face(
                edge.first, edge.second, point, interior_witness, points
            ));
        }
        faces.erase(std::remove_if(
            faces.begin(), faces.end(),
            [](const Face& face){ return !face.alive; }
        ), faces.end());
    }
    return finish_spatial_hull(points, faces);
}

}  // namespace convex_hull_3d_detail

inline ConvexPolyhedron3 convex_hull_3d_with_seed(
    std::vector<Point3> input,
    std::uint64_t random_seed
){
    using namespace convex_hull_3d_detail;
    std::vector<Point3> points = unique_points(std::move(input));
    if(points.size() <= 1){
        return {points.empty() ? -1 : 0, std::move(points), {}};
    }
    const std::size_t first = 0;
    const std::size_t second = 1;
    std::size_t third = points.size();
    for(std::size_t index = 0; index < points.size(); ++index){
        if(index != first && index != second && exactly_non_collinear(
            points[first], points[second], points[index]
        )){
            third = index;
            break;
        }
    }
    if(third == points.size()) return collinear_hull(points);
    std::size_t fourth = points.size();
    for(std::size_t index = 0; index < points.size(); ++index){
        if(index != first && index != second && index != third
            && adaptive_orient3d(
                points[first], points[second], points[third], points[index]
            ) != 0){
            fourth = index;
            break;
        }
    }
    if(fourth == points.size()){
        return coplanar_hull(
            points, locally_normalized_points(points, points[first]),
            first, second, third
        );
    }
    const std::array<std::size_t, 4> interior_witness{
        first, second, third, fourth,
    };
    std::vector<Face> faces{
        outward_face(first, second, third, interior_witness, points),
        outward_face(first, fourth, second, interior_witness, points),
        outward_face(first, third, fourth, interior_witness, points),
        outward_face(second, fourth, third, interior_witness, points),
    };
    std::vector<bool> processed(points.size(), false);
    for(const std::size_t point: interior_witness) processed[point] = true;

    ConflictGraph conflicts(points.size());
    for(std::size_t point = 0; point < points.size(); ++point){
        if(processed[point]) continue;
        for(std::size_t face = 0; face < faces.size(); ++face){
            const Face& current_face = faces[face];
            if(adaptive_orient3d(
                points[current_face.vertices[0]], points[current_face.vertices[1]],
                points[current_face.vertices[2]], points[point]
            ) > 0){
                conflicts.add(point, face, faces);
            }
        }
    }

    std::map<EdgeKey, EdgeIncidence> incidences;
    for(std::size_t face = 0; face < faces.size(); ++face){
        attach_face(incidences, faces[face], face);
    }
    std::vector<std::size_t> order;
    order.reserve(points.size() - 4);
    for(std::size_t point = 0; point < points.size(); ++point){
        if(!processed[point]) order.push_back(point);
    }
    shuffle_indices(order, random_seed);

    struct HorizonEdge{
        std::size_t first = no_index;
        std::size_t second = no_index;
        std::size_t visible_face = no_index;
        std::size_t hidden_face = no_index;
    };
    constexpr std::size_t budget_factor = 32;
    const std::size_t maximum = std::numeric_limits<std::size_t>::max();
    const std::size_t conflict_budget =
        points.size() > maximum / budget_factor / points.size()
        ? maximum
        : budget_factor * points.size() * points.size();
    std::size_t conflict_checks = 0;
    bool budget_exhausted = false;
    std::vector<bool> visible_mark(faces.size(), false);
    std::vector<std::size_t> candidate_mark(points.size(), 0);
    std::size_t candidate_generation = 0;

    for(const std::size_t point: order){
        processed[point] = true;
        std::vector<std::size_t> visible = conflicts.incident_faces(point);
        if(visible.empty()) continue;
        visible_mark.resize(faces.size(), false);
        for(const std::size_t face: visible){
            if(!faces[face].alive)[[unlikely]]{
                throw std::logic_error("convex hull conflict references dead face");
            }
            visible_mark[face] = true;
        }

        std::vector<HorizonEdge> horizon;
        for(const std::size_t face_index: visible){
            const Face& face = faces[face_index];
            for(std::size_t edge = 0; edge < 3; ++edge){
                const std::size_t edge_first = face.vertices[edge];
                const std::size_t edge_second = face.vertices[(edge + 1) % 3];
                const auto iterator = incidences.find(edge_key(
                    edge_first, edge_second
                ));
                if(iterator == incidences.end())[[unlikely]]{
                    throw std::logic_error(
                        "convex hull horizon incidence is missing"
                    );
                }
                const std::size_t other = other_incident_face(
                    iterator->second, face_index
                );
                if(other == no_index)[[unlikely]]{
                    throw std::logic_error("convex hull has an open boundary");
                }
                if(!visible_mark[other]){
                    horizon.push_back({
                        edge_first, edge_second, face_index, other,
                    });
                }
            }
        }
        std::sort(
            horizon.begin(), horizon.end(),
            [](const HorizonEdge& left, const HorizonEdge& right){
                return edge_key(left.first, left.second)
                    < edge_key(right.first, right.second);
            }
        );

        for(const std::size_t face: visible){
            detach_face(incidences, faces[face], face);
            faces[face].alive = false;
        }

        for(const HorizonEdge& edge: horizon){
            const std::size_t new_face = faces.size();
            faces.push_back(outward_face(
                edge.first, edge.second, point, interior_witness, points
            ));
            attach_face(incidences, faces[new_face], new_face);

            ++candidate_generation;
            if(candidate_generation == 0){
                std::fill(candidate_mark.begin(), candidate_mark.end(), 0);
                ++candidate_generation;
            }
            const auto consider = [&](std::size_t candidate){
                if(processed[candidate]
                    || candidate_mark[candidate] == candidate_generation){
                    return;
                }
                candidate_mark[candidate] = candidate_generation;
                if(conflict_checks == conflict_budget){
                    budget_exhausted = true;
                    return;
                }
                ++conflict_checks;
                const Face& face = faces[new_face];
                if(adaptive_orient3d(
                    points[face.vertices[0]], points[face.vertices[1]],
                    points[face.vertices[2]], points[candidate]
                ) > 0){
                    conflicts.add(candidate, new_face, faces);
                }
            };
            conflicts.for_each_face_point(
                edge.visible_face, faces, consider
            );
            if(!budget_exhausted){
                conflicts.for_each_face_point(
                    edge.hidden_face, faces, consider
                );
            }
            if(budget_exhausted) break;
        }
        if(budget_exhausted){
            return scan_incremental_spatial_hull(points, interior_witness);
        }
        for(const std::size_t face: visible){
            conflicts.remove_face(face, faces);
            visible_mark[face] = false;
        }
    }
    return finish_spatial_hull(points, faces);
}

inline ConvexPolyhedron3 convex_hull_3d(std::vector<Point3> input){
    return convex_hull_3d_with_seed(
        std::move(input), convex_hull_3d_detail::default_random_seed
    );
}
