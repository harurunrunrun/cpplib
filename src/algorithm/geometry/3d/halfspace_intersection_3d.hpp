#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "line_plane_intersection.hpp"
#include "parallel.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"
#include "projection.hpp"
#include "three_plane_intersection.hpp"

namespace halfspace_intersection_3d_detail{

inline bool feasible_point(
    const std::vector<Plane3>& halfspaces,
    const Point3& point
){
    for(const Plane3& halfspace: halfspaces){
        const auto product = geometry3d_plane_numeric_detail::exact_dot_difference(
            halfspace.normal, point, halfspace.point
        );
        if(geometry3d_plane_numeric_detail::exact_dot_sign(product) > 0) return false;
    }
    return true;
}

inline bool feasible_direction(
    const std::vector<Plane3>& halfspaces,
    Point3 direction
){
    const long double scale = std::max({
        std::abs(direction.x), std::abs(direction.y), std::abs(direction.z)
    });
    if(scale == 0.0L || !std::isfinite(scale)) return false;
    direction /= scale;
    const auto exact_direction =
        geometry3d_plane_numeric_detail::exact_difference(direction, Point3{});
    for(const Plane3& halfspace: halfspaces){
        if(geometry3d_plane_numeric_detail::exact_dot_sign(
            geometry3d_plane_numeric_detail::exact_dot(
                halfspace.normal, exact_direction
            ),
            8.0L
        ) > 0){
            return false;
        }
    }
    return true;
}

inline Point3 scaled_normal(const Plane3& plane){
    const long double scale = std::max({
        std::abs(plane.normal.x), std::abs(plane.normal.y),
        std::abs(plane.normal.z)
    });
    return plane.normal / scale;
}

inline bool has_nonzero_recession_direction(
    const std::vector<Plane3>& halfspaces
){
    const std::array<Point3, 3> axes{
        Point3{1, 0, 0}, Point3{0, 1, 0}, Point3{0, 0, 1}
    };
    for(const Point3& axis: axes){
        if(feasible_direction(halfspaces, axis)
            || feasible_direction(halfspaces, -axis)) return true;
    }
    for(const Plane3& halfspace: halfspaces){
        if(feasible_direction(halfspaces, halfspace.normal)
            || feasible_direction(halfspaces, -halfspace.normal)) return true;
        for(const Point3& axis: axes){
            const Point3 tangent = cross(scaled_normal(halfspace), axis);
            if(feasible_direction(halfspaces, tangent)
                || feasible_direction(halfspaces, -tangent)) return true;
        }
    }
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size(); ++second){
            const Point3 direction = cross(
                scaled_normal(halfspaces[first]),
                scaled_normal(halfspaces[second])
            );
            if(feasible_direction(halfspaces, direction)
                || feasible_direction(halfspaces, -direction)) return true;
        }
    }
    return false;
}

inline std::optional<Point3> closest_boundary_point_to_origin(
    const Plane3& halfspace
){
    using namespace geometry3d_adaptive_detail;
    const Point3 normal = plane3_unit_normal(halfspace);
    const ExactDyadic distance =
        geometry3d_plane_numeric_detail::exact_dot_difference(
            normal, halfspace.point, Point3{}
        ).value;
    const ExactDyadic one = exact_dyadic(1.0L);
    const auto coordinate = [&](long double component){
        return geometry3d_plane_numeric_detail::exact_ratio(
            multiply(exact_dyadic(component), distance),
            one,
            "halfspace boundary projection is not representable"
        );
    };
    try{
        return Point3{
            coordinate(normal.x),
            coordinate(normal.y),
            coordinate(normal.z),
        };
    }catch(const std::overflow_error&){
        return std::nullopt;
    }
}

inline bool has_feasible_point(const std::vector<Plane3>& halfspaces){
    if(feasible_point(halfspaces, {0, 0, 0})) return true;
    for(const Plane3& halfspace: halfspaces){
        if(feasible_point(halfspaces, halfspace.point)){
            return true;
        }
        const auto closest = closest_boundary_point_to_origin(halfspace);
        if(closest && feasible_point(halfspaces, *closest)) return true;
    }
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size(); ++second){
            if(parallel(
                halfspaces[first].normal, halfspaces[second].normal
            )) continue;
            const Line3 line = plane_plane_intersection(
                halfspaces[first], halfspaces[second]
            );
            if(feasible_point(halfspaces, line.a)) return true;
        }
    }
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size(); ++second){
            for(std::size_t third = second + 1; third < halfspaces.size(); ++third){
                const ThreePlaneIntersection3 intersection = three_plane_intersection(
                    halfspaces[first], halfspaces[second], halfspaces[third]
                );
                if(const Point3* point = std::get_if<Point3>(&intersection)){
                    if(feasible_point(halfspaces, *point)) return true;
                }else if(const Line3* line = std::get_if<Line3>(&intersection)){
                    if(feasible_point(halfspaces, line->a)) return true;
                }else if(const Plane3* plane = std::get_if<Plane3>(&intersection)){
                    const auto plane_point =
                        closest_boundary_point_to_origin(*plane);
                    if(plane_point && feasible_point(halfspaces, *plane_point)){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

inline bool satisfies(const Plane3& halfspace, const Point3& point){
    return geometry3d_plane_numeric_detail::exact_dot_sign(
        geometry3d_plane_numeric_detail::exact_dot_difference(
            halfspace.normal, point, halfspace.point
        )
    ) <= 0;
}

inline std::uint64_t splitmix64(std::uint64_t& state){
    state += 0x9e3779b97f4a7c15ULL;
    std::uint64_t value = state;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

inline void deterministic_shuffle(std::vector<std::size_t>& order){
    std::uint64_t state = 0x243f6a8885a308d3ULL;
    for(std::size_t size = order.size(); size > 1; --size){
        const std::size_t index = static_cast<std::size_t>(
            splitmix64(state) % static_cast<std::uint64_t>(size)
        );
        std::swap(order[size - 1], order[index]);
    }
}

inline std::optional<Point3> minimum_norm_feasible_on_line(
    const std::vector<Plane3>& halfspaces,
    const std::vector<std::size_t>& order,
    std::size_t prefix,
    const Line3& line
){
    Point3 candidate = projection(line, Point3{});
    for(std::size_t position = 0; position < prefix; ++position){
        const Plane3& constraint = halfspaces[order[position]];
        if(satisfies(constraint, candidate)) continue;
        try{
            candidate = line_plane_intersection(line, constraint);
        }catch(const std::domain_error&){
            return std::nullopt;
        }
        for(std::size_t previous = 0; previous < position; ++previous){
            if(!satisfies(halfspaces[order[previous]], candidate)){
                return std::nullopt;
            }
        }
    }
    return candidate;
}

inline std::optional<Point3> minimum_norm_feasible_on_plane(
    const std::vector<Plane3>& halfspaces,
    const std::vector<std::size_t>& order,
    std::size_t prefix,
    const Plane3& plane
){
    Point3 candidate = projection(plane, Point3{});
    for(std::size_t position = 0; position < prefix; ++position){
        const Plane3& constraint = halfspaces[order[position]];
        if(satisfies(constraint, candidate)) continue;
        Line3 line;
        try{
            line = plane_plane_intersection(plane, constraint);
        }catch(const std::domain_error&){
            return std::nullopt;
        }
        const auto replacement = minimum_norm_feasible_on_line(
            halfspaces, order, position, line
        );
        if(!replacement) return std::nullopt;
        candidate = *replacement;
    }
    return candidate;
}

inline std::optional<Point3> minimum_norm_feasible_point(
    const std::vector<Plane3>& halfspaces
){
    std::vector<std::size_t> order(halfspaces.size());
    for(std::size_t index = 0; index < order.size(); ++index){
        order[index] = index;
    }
    deterministic_shuffle(order);
    Point3 candidate{};
    for(std::size_t position = 0; position < order.size(); ++position){
        const Plane3& constraint = halfspaces[order[position]];
        if(satisfies(constraint, candidate)) continue;
        const auto replacement = minimum_norm_feasible_on_plane(
            halfspaces, order, position, constraint
        );
        if(!replacement) return std::nullopt;
        candidate = *replacement;
    }
    return candidate;
}

inline std::optional<Point3> strict_interior_point(
    const std::vector<Plane3>& halfspaces,
    const Point3& feasible
){
    std::vector<Plane3> active_direction_constraints;
    for(const Plane3& halfspace: halfspaces){
        const int side = geometry3d_plane_numeric_detail::exact_dot_sign(
            geometry3d_plane_numeric_detail::exact_dot_difference(
                halfspace.normal, feasible, halfspace.point
            )
        );
        if(side < 0) continue;
        if(side > 0) return std::nullopt;
        const Point3 normal = plane3_unit_normal(halfspace);
        active_direction_constraints.push_back({-normal, normal});
    }
    if(active_direction_constraints.empty()) return feasible;
    const auto direction = minimum_norm_feasible_point(
        active_direction_constraints
    );
    if(!direction) return std::nullopt;

    using namespace geometry3d_adaptive_detail;
    const auto exact_direction =
        geometry3d_plane_numeric_detail::exact_difference(
            *direction, Point3{}
        );
    long double step = 1.0L;
    for(const Plane3& halfspace: halfspaces){
        const auto value =
            geometry3d_plane_numeric_detail::exact_dot_difference(
                halfspace.normal, feasible, halfspace.point
            );
        const auto slope = geometry3d_plane_numeric_detail::exact_dot(
            halfspace.normal, exact_direction
        );
        if(sign(slope.value) <= 0) continue;
        const ExactDyadic numerator = negate(value.value);
        if(sign(numerator) <= 0) continue;
        const long double bound =
            geometry3d_plane_numeric_detail::exact_ratio(
                numerator,
                slope.value,
                "strict interior step is not representable"
            );
        step = std::min(step, bound * 0.5L);
    }
    if(step <= 0.0L || !std::isfinite(step)) return std::nullopt;
    for(int attempt = 0; attempt < 64; ++attempt){
        const Point3 candidate{
            std::fma(direction->x, step, feasible.x),
            std::fma(direction->y, step, feasible.y),
            std::fma(direction->z, step, feasible.z),
        };
        if(geometry3d_is_finite(candidate)){
            bool strict = true;
            for(const Plane3& halfspace: halfspaces){
                if(geometry3d_plane_numeric_detail::exact_dot_sign(
                    geometry3d_plane_numeric_detail::exact_dot_difference(
                        halfspace.normal, candidate, halfspace.point
                    )
                ) >= 0){
                    strict = false;
                    break;
                }
            }
            if(strict) return candidate;
        }
        step *= 0.5L;
        if(step == 0.0L) break;
    }
    return std::nullopt;
}

using ExactDyadic = geometry3d_adaptive_detail::ExactDyadic;

struct DualPoint{
    std::array<ExactDyadic, 4> homogeneous{};
    std::size_t constraint = 0;
};

inline ExactDyadic exact_determinant2(
    const ExactDyadic& a,
    const ExactDyadic& b,
    const ExactDyadic& c,
    const ExactDyadic& d
){
    using namespace geometry3d_adaptive_detail;
    return subtract(multiply(a, d), multiply(b, c));
}

inline ExactDyadic exact_determinant3(
    const std::array<ExactDyadic, 3>& first,
    const std::array<ExactDyadic, 3>& second,
    const std::array<ExactDyadic, 3>& third
){
    using namespace geometry3d_adaptive_detail;
    return add(
        subtract(
            multiply(first[0], exact_determinant2(
                second[1], second[2], third[1], third[2]
            )),
            multiply(first[1], exact_determinant2(
                second[0], second[2], third[0], third[2]
            ))
        ),
        multiply(first[2], exact_determinant2(
            second[0], second[1], third[0], third[1]
        ))
    );
}

inline ExactDyadic exact_determinant4(
    const std::array<ExactDyadic, 4>& first,
    const std::array<ExactDyadic, 4>& second,
    const std::array<ExactDyadic, 4>& third,
    const std::array<ExactDyadic, 4>& fourth
){
    using namespace geometry3d_adaptive_detail;
    const auto minor = [&](std::size_t removed){
        std::array<ExactDyadic, 3> rows[3];
        const std::array<const std::array<ExactDyadic, 4>*, 3> source{
            &second, &third, &fourth
        };
        for(std::size_t row = 0; row < 3; ++row){
            std::size_t column = 0;
            for(std::size_t source_column = 0; source_column < 4;
                ++source_column){
                if(source_column != removed){
                    rows[row][column++] = (*source[row])[source_column];
                }
            }
        }
        return exact_determinant3(rows[0], rows[1], rows[2]);
    };
    ExactDyadic result{};
    for(std::size_t column = 0; column < 4; ++column){
        ExactDyadic term = multiply(first[column], minor(column));
        if(column % 2 != 0) term = negate(std::move(term));
        result = add(result, term);
    }
    return result;
}

inline Point3 exact_three_boundary_point(
    const Plane3& first,
    const Plane3& second,
    const Plane3& third
){
    using namespace geometry3d_adaptive_detail;
    const std::array<const Plane3*, 3> planes{&first, &second, &third};
    std::array<std::array<ExactDyadic, 3>, 3> coefficients;
    std::array<ExactDyadic, 3> offsets;
    for(std::size_t row = 0; row < 3; ++row){
        coefficients[row] = {
            exact_dyadic(planes[row]->normal.x),
            exact_dyadic(planes[row]->normal.y),
            exact_dyadic(planes[row]->normal.z),
        };
        offsets[row] =
            geometry3d_plane_numeric_detail::exact_dot_difference(
                planes[row]->normal, planes[row]->point, Point3{}
            ).value;
    }
    const ExactDyadic denominator = exact_determinant3(
        coefficients[0], coefficients[1], coefficients[2]
    );
    if(sign(denominator) == 0)[[unlikely]]{
        throw std::logic_error(
            "dual hull facet has dependent primal normals"
        );
    }
    const auto coordinate = [&](std::size_t replaced){
        auto matrix = coefficients;
        for(std::size_t row = 0; row < 3; ++row){
            matrix[row][replaced] = offsets[row];
        }
        return geometry3d_plane_numeric_detail::exact_ratio(
            exact_determinant3(matrix[0], matrix[1], matrix[2]),
            denominator,
            "halfspace intersection vertex is not representable"
        );
    };
    return {
        coordinate(0),
        coordinate(1),
        coordinate(2),
    };
}

inline int dual_orientation(
    const std::vector<DualPoint>& points,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    std::size_t fourth
){
    return geometry3d_adaptive_detail::sign(exact_determinant4(
        points[first].homogeneous,
        points[second].homogeneous,
        points[third].homogeneous,
        points[fourth].homogeneous
    ));
}

inline bool dual_same(
    const DualPoint& first,
    const DualPoint& second
){
    using namespace geometry3d_adaptive_detail;
    for(std::size_t coordinate = 1; coordinate < 4; ++coordinate){
        if(sign(subtract(
            multiply(first.homogeneous[coordinate], second.homogeneous[0]),
            multiply(second.homogeneous[coordinate], first.homogeneous[0])
        )) != 0) return false;
    }
    return true;
}

inline bool dual_non_collinear(
    const DualPoint& first,
    const DualPoint& second,
    const DualPoint& third
){
    const std::array<const std::array<ExactDyadic, 4>*, 3> source{
        &first.homogeneous, &second.homogeneous, &third.homogeneous
    };
    for(std::size_t omitted = 0; omitted < 4; ++omitted){
        std::array<ExactDyadic, 3> rows[3];
        for(std::size_t row = 0; row < 3; ++row){
            std::size_t column = 0;
            for(std::size_t source_column = 0; source_column < 4;
                ++source_column){
                if(source_column != omitted){
                    rows[row][column++] = (*source[row])[source_column];
                }
            }
        }
        if(geometry3d_adaptive_detail::sign(
            exact_determinant3(rows[0], rows[1], rows[2])
        ) != 0) return true;
    }
    return false;
}

inline int dual_origin_side(
    const std::vector<DualPoint>& points,
    const std::array<std::size_t, 3>& face
){
    std::array<ExactDyadic, 4> origin{};
    origin[0] = geometry3d_adaptive_detail::exact_dyadic(1.0L);
    return geometry3d_adaptive_detail::sign(exact_determinant4(
        points[face[0]].homogeneous,
        points[face[1]].homogeneous,
        points[face[2]].homogeneous,
        origin
    ));
}

struct DualFace{
    std::array<std::size_t, 3> vertices{};
    bool alive = true;
    std::size_t conflict_head = std::numeric_limits<std::size_t>::max();
};

inline constexpr std::size_t no_dual_index =
    std::numeric_limits<std::size_t>::max();

struct DualConflictGraph{
    struct Entry{
        std::size_t point = no_dual_index;
        std::size_t face = no_dual_index;
        std::size_t previous_point = no_dual_index;
        std::size_t next_point = no_dual_index;
        std::size_t previous_face = no_dual_index;
        std::size_t next_face = no_dual_index;
    };

    explicit DualConflictGraph(std::size_t size):
        point_heads(size, no_dual_index){}

    void add(
        std::size_t point,
        std::size_t face,
        std::vector<DualFace>& faces
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
            point, face, no_dual_index, point_heads[point],
            no_dual_index, faces[face].conflict_head
        };
        if(point_heads[point] != no_dual_index){
            entries[point_heads[point]].previous_point = index;
        }
        if(faces[face].conflict_head != no_dual_index){
            entries[faces[face].conflict_head].previous_face = index;
        }
        point_heads[point] = index;
        faces[face].conflict_head = index;
    }

    std::vector<std::size_t> incident_faces(std::size_t point) const{
        std::vector<std::size_t> result;
        for(std::size_t entry = point_heads[point]; entry != no_dual_index;
            entry = entries[entry].next_point){
            result.push_back(entries[entry].face);
        }
        return result;
    }

    template<class Function>
    void for_each_face_point(
        std::size_t face,
        const std::vector<DualFace>& faces,
        Function function
    ) const{
        for(std::size_t entry = faces[face].conflict_head;
            entry != no_dual_index; entry = entries[entry].next_face){
            function(entries[entry].point);
        }
    }

    void remove_face(std::size_t face, std::vector<DualFace>& faces){
        while(faces[face].conflict_head != no_dual_index){
            const std::size_t index = faces[face].conflict_head;
            const Entry entry = entries[index];
            if(entry.previous_point == no_dual_index){
                point_heads[entry.point] = entry.next_point;
            }else{
                entries[entry.previous_point].next_point = entry.next_point;
            }
            if(entry.next_point != no_dual_index){
                entries[entry.next_point].previous_point =
                    entry.previous_point;
            }
            faces[face].conflict_head = entry.next_face;
            if(entry.next_face != no_dual_index){
                entries[entry.next_face].previous_face = no_dual_index;
            }
            entries[index] = {};
            free_entries.push_back(index);
        }
    }

    std::vector<Entry> entries;
    std::vector<std::size_t> free_entries;
    std::vector<std::size_t> point_heads;
};

using DualEdge = std::pair<std::size_t, std::size_t>;

inline DualEdge dual_edge(std::size_t first, std::size_t second){
    if(second < first) std::swap(first, second);
    return {first, second};
}

struct DualEdgeIncidence{
    std::size_t first = no_dual_index;
    std::size_t second = no_dual_index;
};

inline void attach_dual_face(
    std::map<DualEdge, DualEdgeIncidence>& incidences,
    const DualFace& face,
    std::size_t index
){
    for(std::size_t edge = 0; edge < 3; ++edge){
        auto& incidence = incidences[dual_edge(
            face.vertices[edge], face.vertices[(edge + 1) % 3]
        )];
        if(incidence.first == no_dual_index) incidence.first = index;
        else incidence.second = index;
    }
}

inline void detach_dual_face(
    std::map<DualEdge, DualEdgeIncidence>& incidences,
    const DualFace& face,
    std::size_t index
){
    for(std::size_t edge = 0; edge < 3; ++edge){
        const DualEdge key = dual_edge(
            face.vertices[edge], face.vertices[(edge + 1) % 3]
        );
        auto iterator = incidences.find(key);
        if(iterator == incidences.end()) continue;
        auto& incidence = iterator->second;
        if(incidence.first == index){
            incidence.first = incidence.second;
            incidence.second = no_dual_index;
        }else if(incidence.second == index){
            incidence.second = no_dual_index;
        }
        if(incidence.first == no_dual_index) incidences.erase(iterator);
    }
}

inline std::size_t other_dual_face(
    const DualEdgeIncidence& incidence,
    std::size_t face
){
    if(incidence.first == face) return incidence.second;
    if(incidence.second == face) return incidence.first;
    return no_dual_index;
}

inline DualFace outward_dual_face(
    std::size_t first,
    std::size_t second,
    std::size_t third,
    const std::array<std::size_t, 4>& witnesses,
    const std::vector<DualPoint>& points
){
    for(const std::size_t witness: witnesses){
        const int side = dual_orientation(
            points, first, second, third, witness
        );
        if(side != 0){
            if(side > 0) std::swap(second, third);
            return {{first, second, third}, true, no_dual_index};
        }
    }
    throw std::logic_error("degenerate dual hull face");
}

inline std::vector<std::array<std::size_t, 3>> scan_dual_hull_faces(
    const std::vector<DualPoint>& points,
    const std::array<std::size_t, 4>& witnesses
){
    const auto [first, second, third, fourth] = witnesses;
    std::vector<DualFace> faces{
        outward_dual_face(first, second, third, witnesses, points),
        outward_dual_face(first, fourth, second, witnesses, points),
        outward_dual_face(first, third, fourth, witnesses, points),
        outward_dual_face(second, fourth, third, witnesses, points),
    };
    std::vector<bool> initial(points.size(), false);
    for(const std::size_t point: witnesses) initial[point] = true;
    std::vector<std::size_t> order;
    for(std::size_t point = 0; point < points.size(); ++point){
        if(!initial[point]) order.push_back(point);
    }
    deterministic_shuffle(order);
    struct HorizonRecord{
        std::size_t first = no_dual_index;
        std::size_t second = no_dual_index;
        std::size_t count = 0;
    };
    for(const std::size_t point: order){
        std::vector<std::size_t> visible;
        for(std::size_t face = 0; face < faces.size(); ++face){
            const auto& vertices = faces[face].vertices;
            if(dual_orientation(
                points, vertices[0], vertices[1], vertices[2], point
            ) > 0){
                visible.push_back(face);
            }
        }
        if(visible.empty()) continue;
        std::map<DualEdge, HorizonRecord> horizon;
        for(const std::size_t face_index: visible){
            DualFace& face = faces[face_index];
            face.alive = false;
            for(std::size_t edge = 0; edge < 3; ++edge){
                const std::size_t edge_first = face.vertices[edge];
                const std::size_t edge_second =
                    face.vertices[(edge + 1) % 3];
                HorizonRecord& record = horizon[dual_edge(
                    edge_first, edge_second
                )];
                if(record.count == 0){
                    record.first = edge_first;
                    record.second = edge_second;
                }
                ++record.count;
            }
        }
        for(const auto& [key, edge]: horizon){
            static_cast<void>(key);
            if(edge.count == 1) faces.push_back(outward_dual_face(
                edge.first, edge.second, point, witnesses, points
            ));
        }
        faces.erase(std::remove_if(
            faces.begin(), faces.end(),
            [](const DualFace& face){ return !face.alive; }
        ), faces.end());
    }
    std::vector<std::array<std::size_t, 3>> result;
    result.reserve(faces.size());
    for(const DualFace& face: faces) result.push_back(face.vertices);
    return result;
}

inline std::optional<std::vector<std::array<std::size_t, 3>>>
dual_hull_faces(const std::vector<DualPoint>& points){
    if(points.size() < 4) return std::nullopt;
    const std::size_t first = 0;
    std::size_t second = points.size();
    for(std::size_t index = 1; index < points.size(); ++index){
        if(!dual_same(points[first], points[index])){
            second = index;
            break;
        }
    }
    if(second == points.size()) return std::nullopt;
    std::size_t third = points.size();
    for(std::size_t index = 0; index < points.size(); ++index){
        if(index != first && index != second && dual_non_collinear(
            points[first], points[second], points[index]
        )){
            third = index;
            break;
        }
    }
    if(third == points.size()) return std::nullopt;
    std::size_t fourth = points.size();
    for(std::size_t index = 0; index < points.size(); ++index){
        if(index != first && index != second && index != third
            && dual_orientation(points, first, second, third, index) != 0){
            fourth = index;
            break;
        }
    }
    if(fourth == points.size()) return std::nullopt;

    const std::array<std::size_t, 4> witnesses{
        first, second, third, fourth
    };
    std::vector<DualFace> faces{
        outward_dual_face(first, second, third, witnesses, points),
        outward_dual_face(first, fourth, second, witnesses, points),
        outward_dual_face(first, third, fourth, witnesses, points),
        outward_dual_face(second, fourth, third, witnesses, points),
    };
    std::vector<bool> processed(points.size(), false);
    for(const std::size_t point: witnesses) processed[point] = true;
    DualConflictGraph conflicts(points.size());
    for(std::size_t point = 0; point < points.size(); ++point){
        if(processed[point]) continue;
        for(std::size_t face = 0; face < faces.size(); ++face){
            const auto& vertices = faces[face].vertices;
            if(dual_orientation(
                points, vertices[0], vertices[1], vertices[2], point
            ) > 0){
                conflicts.add(point, face, faces);
            }
        }
    }
    std::map<DualEdge, DualEdgeIncidence> incidences;
    for(std::size_t face = 0; face < faces.size(); ++face){
        attach_dual_face(incidences, faces[face], face);
    }
    std::vector<std::size_t> order;
    for(std::size_t point = 0; point < points.size(); ++point){
        if(!processed[point]) order.push_back(point);
    }
    deterministic_shuffle(order);
    std::vector<bool> visible_mark(faces.size(), false);
    std::vector<std::size_t> candidate_mark(points.size(), 0);
    std::size_t generation = 0;
    constexpr std::size_t budget_factor = 32;
    const std::size_t maximum = std::numeric_limits<std::size_t>::max();
    const std::size_t conflict_budget =
        points.size() > maximum / budget_factor / points.size()
        ? maximum
        : budget_factor * points.size() * points.size();
    std::size_t conflict_checks = 0;
    bool budget_exhausted = false;
    for(const std::size_t point: order){
        processed[point] = true;
        const std::vector<std::size_t> visible =
            conflicts.incident_faces(point);
        if(visible.empty()) continue;
        visible_mark.resize(faces.size(), false);
        for(const std::size_t face: visible) visible_mark[face] = true;
        struct Horizon{
            std::size_t first;
            std::size_t second;
            std::size_t visible_face;
            std::size_t hidden_face;
        };
        std::vector<Horizon> horizon;
        for(const std::size_t face_index: visible){
            const DualFace& face = faces[face_index];
            for(std::size_t edge = 0; edge < 3; ++edge){
                const std::size_t edge_first = face.vertices[edge];
                const std::size_t edge_second =
                    face.vertices[(edge + 1) % 3];
                const auto iterator = incidences.find(dual_edge(
                    edge_first, edge_second
                ));
                if(iterator == incidences.end()) continue;
                const std::size_t other = other_dual_face(
                    iterator->second, face_index
                );
                if(other != no_dual_index && !visible_mark[other]){
                    horizon.push_back({
                        edge_first, edge_second, face_index, other
                    });
                }
            }
        }
        for(const std::size_t face: visible){
            detach_dual_face(incidences, faces[face], face);
            faces[face].alive = false;
        }
        for(const Horizon& edge: horizon){
            const std::size_t face_index = faces.size();
            faces.push_back(outward_dual_face(
                edge.first, edge.second, point, witnesses, points
            ));
            attach_dual_face(incidences, faces.back(), face_index);
            ++generation;
            if(generation == 0){
                std::fill(candidate_mark.begin(), candidate_mark.end(), 0);
                ++generation;
            }
            const auto consider = [&](std::size_t candidate){
                if(processed[candidate]
                    || candidate_mark[candidate] == generation) return;
                candidate_mark[candidate] = generation;
                if(conflict_checks == conflict_budget){
                    budget_exhausted = true;
                    return;
                }
                ++conflict_checks;
                const auto& vertices = faces[face_index].vertices;
                if(dual_orientation(
                    points, vertices[0], vertices[1], vertices[2], candidate
                ) > 0){
                    conflicts.add(candidate, face_index, faces);
                }
            };
            conflicts.for_each_face_point(
                edge.visible_face, faces, consider
            );
            conflicts.for_each_face_point(
                edge.hidden_face, faces, consider
            );
            if(budget_exhausted) break;
        }
        if(budget_exhausted){
            return scan_dual_hull_faces(points, witnesses);
        }
        for(const std::size_t face: visible){
            conflicts.remove_face(face, faces);
            visible_mark[face] = false;
        }
    }
    std::vector<std::array<std::size_t, 3>> result;
    for(const DualFace& face: faces){
        if(face.alive) result.push_back(face.vertices);
    }
    return result;
}

inline void validate_halfspaces(const std::vector<Plane3>& halfspaces){
    for(const Plane3& halfspace: halfspaces){
        if(!geometry3d_is_finite(halfspace.point)
            || !geometry3d_is_finite(halfspace.normal))[[unlikely]]{
            throw std::invalid_argument("non-finite 3D halfspace");
        }
        static_cast<void>(plane3_unit_normal(halfspace));
    }
}

inline ConvexPolyhedron3 intersection_from_strict_interior(
    const std::vector<Plane3>& halfspaces,
    const Point3& interior
){
    if(!geometry3d_is_finite(interior))[[unlikely]]{
        throw std::invalid_argument("non-finite strict interior point");
    }
    std::vector<DualPoint> dual_points;
    dual_points.reserve(halfspaces.size());
    for(std::size_t index = 0; index < halfspaces.size(); ++index){
        const Plane3& halfspace = halfspaces[index];
        const auto signed_distance =
            geometry3d_plane_numeric_detail::exact_dot_difference(
                halfspace.normal, interior, halfspace.point
            );
        if(geometry3d_plane_numeric_detail::exact_dot_sign(
            signed_distance
        ) >= 0)[[unlikely]]{
            throw std::invalid_argument(
                "point is not strictly inside every 3D halfspace"
            );
        }
        using namespace geometry3d_adaptive_detail;
        dual_points.push_back({{
            negate(signed_distance.value),
            exact_dyadic(halfspace.normal.x),
            exact_dyadic(halfspace.normal.y),
            exact_dyadic(halfspace.normal.z),
        }, index});
    }
    const auto faces = dual_hull_faces(dual_points);
    if(!faces)[[unlikely]]{
        throw std::domain_error("3D halfspace intersection is unbounded");
    }
    for(const auto& face: *faces){
        if(dual_origin_side(dual_points, face) >= 0)[[unlikely]]{
            throw std::domain_error(
                "3D halfspace intersection is unbounded"
            );
        }
    }

    std::vector<std::size_t> parent(faces->size());
    std::vector<std::size_t> rank(faces->size(), 0);
    for(std::size_t face = 0; face < faces->size(); ++face){
        parent[face] = face;
    }
    const auto find_root = [&](std::size_t value){
        std::size_t root = value;
        while(parent[root] != root) root = parent[root];
        while(parent[value] != value){
            const std::size_t next = parent[value];
            parent[value] = root;
            value = next;
        }
        return root;
    };
    const auto unite = [&](std::size_t first, std::size_t second){
        first = find_root(first);
        second = find_root(second);
        if(first == second) return;
        if(rank[first] < rank[second]) std::swap(first, second);
        parent[second] = first;
        if(rank[first] == rank[second]) ++rank[first];
    };
    std::map<DualEdge, std::size_t> edge_owner;
    for(std::size_t face_index = 0; face_index < faces->size(); ++face_index){
        const auto& face = (*faces)[face_index];
        for(std::size_t edge = 0; edge < 3; ++edge){
            const DualEdge key = dual_edge(
                face[edge], face[(edge + 1) % 3]
            );
            const auto [iterator, inserted] =
                edge_owner.emplace(key, face_index);
            if(inserted) continue;
            const auto& other = (*faces)[iterator->second];
            const auto third_vertex = [&](const auto& triangle){
                for(const std::size_t vertex: triangle){
                    if(vertex != key.first && vertex != key.second){
                        return vertex;
                    }
                }
                return no_dual_index;
            };
            const std::size_t first_third = third_vertex(face);
            const std::size_t second_third = third_vertex(other);
            if(first_third != no_dual_index
                && second_third != no_dual_index
                && dual_orientation(
                    dual_points, key.first, key.second,
                    first_third, second_third
                ) == 0){
                unite(face_index, iterator->second);
            }
        }
    }

    using CellCoordinate = std::pair<bool, long double>;
    using CellKey = std::array<CellCoordinate, 3>;
    std::map<CellKey, std::vector<Point3>> vertex_cells;
    std::vector<Point3> vertices;
    vertices.reserve(faces->size());
    const auto cell_coordinates = [](long double coordinate){
        std::vector<CellCoordinate> result;
        const long double exact_only_threshold =
            4.0L * GEOMETRY3D_EPS
            / std::numeric_limits<long double>::epsilon();
        if(std::abs(coordinate) > exact_only_threshold){
            result.push_back({true, coordinate});
        }else{
            const long double cell = std::floor(
                coordinate / GEOMETRY3D_EPS
            );
            result.push_back({false, cell - 1.0L});
            result.push_back({false, cell});
            result.push_back({false, cell + 1.0L});
        }
        return result;
    };
    const auto add_unique_vertex = [&](const Point3& point){
        const std::array<std::vector<CellCoordinate>, 3> candidates{
            cell_coordinates(point.x),
            cell_coordinates(point.y),
            cell_coordinates(point.z),
        };
        for(const CellCoordinate& x: candidates[0]){
            for(const CellCoordinate& y: candidates[1]){
                for(const CellCoordinate& z: candidates[2]){
                    const auto iterator = vertex_cells.find({x, y, z});
                    if(iterator == vertex_cells.end()) continue;
                    if(std::any_of(
                        iterator->second.begin(), iterator->second.end(),
                        [&](const Point3& other){ return point == other; }
                    )){
                        return;
                    }
                }
            }
        }
        const auto central = [&](long double coordinate){
            const long double exact_only_threshold =
                4.0L * GEOMETRY3D_EPS
                / std::numeric_limits<long double>::epsilon();
            if(std::abs(coordinate) > exact_only_threshold){
                return CellCoordinate{true, coordinate};
            }
            return CellCoordinate{
                false, std::floor(coordinate / GEOMETRY3D_EPS)
            };
        };
        vertex_cells[{
            central(point.x), central(point.y), central(point.z)
        }].push_back(point);
        vertices.push_back(point);
    };
    for(std::size_t face_index = 0; face_index < faces->size(); ++face_index){
        if(find_root(face_index) != face_index) continue;
        const auto& face = (*faces)[face_index];
        add_unique_vertex(exact_three_boundary_point(
            halfspaces[dual_points[face[0]].constraint],
            halfspaces[dual_points[face[1]].constraint],
            halfspaces[dual_points[face[2]].constraint]
        ));
    }
    ConvexPolyhedron3 result = convex_hull_3d(std::move(vertices));
    if(result.affine_dimension != 3)[[unlikely]]{
        throw std::logic_error(
            "strictly feasible bounded halfspaces produced a degenerate hull"
        );
    }
    return result;
}

}  // namespace halfspace_intersection_3d_detail

inline ConvexPolyhedron3 halfspace_intersection_3d_with_interior_point(
    const std::vector<Plane3>& halfspaces,
    const Point3& strict_interior_point
){
    using namespace halfspace_intersection_3d_detail;
    validate_halfspaces(halfspaces);
    return intersection_from_strict_interior(
        halfspaces, strict_interior_point
    );
}

inline ConvexPolyhedron3 halfspace_intersection_3d(
    const std::vector<Plane3>& halfspaces
){
    using namespace halfspace_intersection_3d_detail;
    std::vector<Plane3> normalized;
    normalized.reserve(halfspaces.size());
    for(const Plane3& halfspace: halfspaces){
        normalized.push_back(halfspace);
    }
    validate_halfspaces(normalized);
    try{
        const auto feasible = minimum_norm_feasible_point(normalized);
        if(feasible && feasible_point(normalized, *feasible)){
            const auto interior = strict_interior_point(
                normalized, *feasible
            );
            if(interior){
                return intersection_from_strict_interior(
                    normalized, *interior
                );
            }
        }
    }catch(const std::overflow_error&){
        // Unrepresentable intermediate projections use the exhaustive path.
    }
    if(!has_feasible_point(normalized)) return {-1, {}, {}};
    if(has_nonzero_recession_direction(normalized))[[unlikely]]{
        throw std::domain_error("3D halfspace intersection is unbounded");
    }

    std::vector<Point3> vertices;
    const auto add_unique = [&](const Point3& point){
        if(!feasible_point(normalized, point)) return;
        const bool duplicate = std::any_of(
            vertices.begin(), vertices.end(), [&](const Point3& other){
                return point == other;
            }
        );
        if(!duplicate) vertices.push_back(point);
    };
    for(std::size_t first = 0; first < normalized.size(); ++first){
        for(std::size_t second = first + 1; second < normalized.size(); ++second){
            for(std::size_t third = second + 1; third < normalized.size(); ++third){
                const ThreePlaneIntersection3 intersection = three_plane_intersection(
                    normalized[first], normalized[second], normalized[third]
                );
                if(const Point3* point = std::get_if<Point3>(&intersection)){
                    add_unique(*point);
                }
            }
        }
    }
    if(vertices.empty())[[unlikely]]{
        throw std::domain_error(
            "bounded 3D halfspace intersection has no enumerable vertex"
        );
    }
    return convex_hull_3d(std::move(vertices));
}
