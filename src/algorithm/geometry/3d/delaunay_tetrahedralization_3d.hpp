#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "adaptive_insphere.hpp"
#include "adaptive_orient3d.hpp"
#include "convex_hull_3d.hpp"
#include "cross.hpp"
#include "delaunay_tetrahedralization3.hpp"
#include "is_finite.hpp"

namespace delaunay_tetrahedralization_3d_detail{

struct TetrahedronIndex{
    std::array<std::size_t, 4> vertices{};
    std::array<std::size_t, 4> neighbors{
        std::numeric_limits<std::size_t>::max(),
        std::numeric_limits<std::size_t>::max(),
        std::numeric_limits<std::size_t>::max(),
        std::numeric_limits<std::size_t>::max(),
    };
    std::vector<std::size_t> conflicts;
    bool alive = true;
};

inline std::vector<Point3> exact_unique_points(std::vector<Point3> points){
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "delaunay_tetrahedralization_3d requires finite points"
            );
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

inline std::vector<Point3> normalized_points(const std::vector<Point3>& points){
    const Point3 anchor = points.front();
    std::vector<Geometry3DNormalizedDifference> differences;
    differences.reserve(points.size());
    long double local_scale = 0.0L;
    for(const Point3& point: points){
        differences.push_back(geometry3d_normalized_difference(point, anchor));
        const auto& difference = differences.back();
        if(difference.value.x != 0.0L || difference.value.y != 0.0L
            || difference.value.z != 0.0L){
            local_scale = std::max(local_scale, difference.scale);
        }
    }
    if(local_scale == 0.0L) local_scale = 1.0L;

    std::vector<Point3> scaled;
    scaled.reserve(points.size());
    for(const auto& difference: differences){
        scaled.push_back(
            difference.value * (difference.scale / local_scale)
        );
    }

    Point3 minimum = scaled.front();
    Point3 maximum = scaled.front();
    for(const Point3& point: scaled){
        minimum.x = std::min(minimum.x, point.x);
        minimum.y = std::min(minimum.y, point.y);
        minimum.z = std::min(minimum.z, point.z);
        maximum.x = std::max(maximum.x, point.x);
        maximum.y = std::max(maximum.y, point.y);
        maximum.z = std::max(maximum.z, point.z);
    }
    const Point3 center{
        minimum.x / 2 + maximum.x / 2,
        minimum.y / 2 + maximum.y / 2,
        minimum.z / 2 + maximum.z / 2,
    };
    long double range = std::max({
        maximum.x - minimum.x,
        maximum.y - minimum.y,
        maximum.z - minimum.z,
    });
    if(range == 0.0L) range = 1.0L;
    for(Point3& point: scaled) point = (point - center) / range;
    return scaled;
}

inline bool exactly_non_collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    using namespace geometry3d_adaptive_detail;
    std::array<ExactDyadic, 3> first_direction{};
    std::array<ExactDyadic, 3> second_direction{};
    const std::array<long double, 3> first_coordinates{
        first.x, first.y, first.z
    };
    const std::array<long double, 3> second_coordinates{
        second.x, second.y, second.z
    };
    const std::array<long double, 3> third_coordinates{
        third.x, third.y, third.z
    };
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        first_direction[coordinate] = subtract(
            exact_dyadic(second_coordinates[coordinate]),
            exact_dyadic(first_coordinates[coordinate])
        );
        second_direction[coordinate] = subtract(
            exact_dyadic(third_coordinates[coordinate]),
            exact_dyadic(first_coordinates[coordinate])
        );
    }
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        const std::size_t next = (coordinate + 1) % 3;
        const std::size_t last = (coordinate + 2) % 3;
        const ExactDyadic component = subtract(
            multiply(first_direction[next], second_direction[last]),
            multiply(first_direction[last], second_direction[next])
        );
        if(sign(component) != 0) return true;
    }
    return false;
}

inline int affine_dimension(const std::vector<Point3>& points){
    if(points.empty()) return -1;
    if(points.size() == 1) return 0;
    std::size_t third = points.size();
    for(std::size_t index = 2; index < points.size(); ++index){
        if(exactly_non_collinear(points[0], points[1], points[index])){
            third = index;
            break;
        }
    }
    if(third == points.size()) return 1;
    for(std::size_t index = 0; index < points.size(); ++index){
        if(adaptive_orient3d(
            points[0], points[1], points[third], points[index]
        ) != 0) return 3;
    }
    return 2;
}

inline std::array<std::size_t, 4> positive_tetrahedron(
    std::array<std::size_t, 4> tetrahedron,
    const std::vector<Point3>& points
){
    const int orientation = adaptive_orient3d(
        points[tetrahedron[0]], points[tetrahedron[1]],
        points[tetrahedron[2]], points[tetrahedron[3]]
    );
    if(orientation < 0) std::swap(tetrahedron[0], tetrahedron[1]);
    return tetrahedron;
}

constexpr std::size_t no_tetrahedron =
    std::numeric_limits<std::size_t>::max();

struct FaceHash{
    std::size_t operator()(const std::array<std::size_t, 3>& face) const{
        std::size_t hash = 0x9e3779b97f4a7c15ULL;
        for(std::size_t vertex: face){
            vertex += 0x9e3779b97f4a7c15ULL;
            vertex = (vertex ^ (vertex >> 30)) * 0xbf58476d1ce4e5b9ULL;
            vertex = (vertex ^ (vertex >> 27)) * 0x94d049bb133111ebULL;
            vertex ^= vertex >> 31;
            hash ^= vertex + 0x9e3779b97f4a7c15ULL
                + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

struct TetrahedronHash{
    std::size_t operator()(const std::array<std::size_t, 4>& tetrahedron) const{
        std::size_t hash = 0x517cc1b727220a95ULL;
        for(std::size_t vertex: tetrahedron){
            vertex += 0x9e3779b97f4a7c15ULL;
            vertex = (vertex ^ (vertex >> 30)) * 0xbf58476d1ce4e5b9ULL;
            vertex = (vertex ^ (vertex >> 27)) * 0x94d049bb133111ebULL;
            vertex ^= vertex >> 31;
            hash ^= vertex + 0x9e3779b97f4a7c15ULL
                + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

inline std::array<std::size_t, 3> opposite_face(
    const std::array<std::size_t, 4>& tetrahedron,
    std::size_t opposite
){
    std::array<std::size_t, 3> face{};
    for(std::size_t source = 0, target = 0; source < 4; ++source){
        if(source != opposite) face[target++] = tetrahedron[source];
    }
    std::sort(face.begin(), face.end());
    return face;
}

inline int insphere_side(
    const TetrahedronIndex& tetrahedron,
    const std::vector<Point3>& points,
    std::size_t point
){
    const auto& v = tetrahedron.vertices;
    return adaptive_insphere(
        points[v[0]], points[v[1]], points[v[2]], points[v[3]], points[point]
    );
}

inline bool contains_point(
    const TetrahedronIndex& tetrahedron,
    const std::vector<Point3>& points,
    std::size_t point
){
    for(std::size_t replaced = 0; replaced < 4; ++replaced){
        auto vertices = tetrahedron.vertices;
        vertices[replaced] = point;
        if(adaptive_orient3d(
            points[vertices[0]], points[vertices[1]],
            points[vertices[2]], points[vertices[3]]
        ) < 0) return false;
    }
    return true;
}

inline std::size_t locate_point(
    const std::vector<TetrahedronIndex>& tetrahedra,
    const std::vector<Point3>& points,
    std::size_t point,
    std::size_t start
){
    std::size_t current = start;
    for(std::size_t step = 0; step <= tetrahedra.size(); ++step){
        if(current >= tetrahedra.size() || !tetrahedra[current].alive){
            throw std::logic_error("Delaunay point-location root is not alive");
        }
        const TetrahedronIndex& tetrahedron = tetrahedra[current];
        bool moved = false;
        for(std::size_t replaced = 0; replaced < 4; ++replaced){
            auto vertices = tetrahedron.vertices;
            vertices[replaced] = point;
            if(adaptive_orient3d(
                points[vertices[0]], points[vertices[1]],
                points[vertices[2]], points[vertices[3]]
            ) >= 0) continue;
            const std::size_t next = tetrahedron.neighbors[replaced];
            if(next == no_tetrahedron){
                throw std::logic_error("point escaped the Delaunay super tetrahedron");
            }
            current = next;
            moved = true;
            break;
        }
        if(!moved) return current;
    }
    throw std::logic_error("Delaunay point-location walk cycled");
}

inline std::vector<std::size_t> insertion_order(
    std::size_t size,
    std::uint64_t seed
){
    std::vector<std::size_t> order(size);
    std::iota(order.begin(), order.end(), 0);
    std::mt19937_64 random(seed);
    std::shuffle(order.begin(), order.end(), random);
    return order;
}

struct BoundaryFace{
    std::array<std::size_t, 3> vertices{};
    std::size_t outside = no_tetrahedron;
};

inline void connect_across_face(
    std::vector<TetrahedronIndex>& tetrahedra,
    std::size_t tetrahedron,
    const std::array<std::size_t, 3>& face,
    std::size_t neighbor
){
    for(std::size_t opposite = 0; opposite < 4; ++opposite){
        if(opposite_face(tetrahedra[tetrahedron].vertices, opposite) == face){
            tetrahedra[tetrahedron].neighbors[opposite] = neighbor;
            return;
        }
    }
    throw std::logic_error("Delaunay neighboring face is inconsistent");
}

inline std::vector<std::array<std::size_t, 4>> bowyer_watson(
    const std::vector<Point3>& normalized,
    long double super_extent,
    std::uint64_t seed
){
    std::vector<Point3> predicates = normalized;
    const std::size_t original_size = predicates.size();
    predicates.push_back({-super_extent, -super_extent, -super_extent});
    predicates.push_back({ super_extent, -super_extent,  super_extent});
    predicates.push_back({-super_extent,  super_extent,  super_extent});
    predicates.push_back({ super_extent,  super_extent, -super_extent});

    TetrahedronIndex initial;
    initial.vertices = positive_tetrahedron({
        original_size, original_size + 1, original_size + 2, original_size + 3
    }, predicates);
    std::vector<TetrahedronIndex> tetrahedra;
    tetrahedra.push_back(std::move(initial));
    std::vector<std::vector<std::size_t>> point_conflicts(original_size);
    for(std::size_t point = 0; point < original_size; ++point){
        if(!contains_point(tetrahedra.front(), predicates, point)){
            throw std::logic_error("Delaunay super tetrahedron is too small");
        }
        if(insphere_side(tetrahedra.front(), predicates, point) <= 0){
            throw std::logic_error("Delaunay super sphere does not contain input");
        }
        tetrahedra.front().conflicts.push_back(point);
        point_conflicts[point].push_back(0);
    }

    std::vector<unsigned char> inserted(original_size, false);
    std::vector<std::size_t> candidate_stamp(original_size, 0);
    std::size_t root = 0;
    std::vector<std::size_t> bad_stamp(1, 0);
    std::size_t epoch = 0;
    for(std::size_t point: insertion_order(original_size, seed)){
        ++epoch;
        std::vector<std::size_t> bad;
        for(std::size_t tetrahedron: point_conflicts[point]){
            if(tetrahedron >= tetrahedra.size()
                || !tetrahedra[tetrahedron].alive
                || bad_stamp[tetrahedron] == epoch
                || insphere_side(tetrahedra[tetrahedron], predicates, point) <= 0){
                continue;
            }
            bad_stamp[tetrahedron] = epoch;
            bad.push_back(tetrahedron);
        }
        if(bad.empty()){
            const std::size_t located = locate_point(
                tetrahedra, predicates, point, root
            );
            if(insphere_side(tetrahedra[located], predicates, point) <= 0){
                throw std::logic_error("located tetrahedron does not conflict");
            }
            bad_stamp[located] = epoch;
            bad.push_back(located);
        }
        for(std::size_t cursor = 0; cursor < bad.size(); ++cursor){
            const TetrahedronIndex& tetrahedron = tetrahedra[bad[cursor]];
            for(std::size_t neighbor: tetrahedron.neighbors){
                if(neighbor == no_tetrahedron || bad_stamp[neighbor] == epoch
                    || !tetrahedra[neighbor].alive
                    || insphere_side(tetrahedra[neighbor], predicates, point) <= 0){
                    continue;
                }
                bad_stamp[neighbor] = epoch;
                bad.push_back(neighbor);
            }
        }

        std::vector<BoundaryFace> boundary;
        std::vector<std::size_t> candidates;
        for(std::size_t tetrahedron: bad){
            const TetrahedronIndex& removed = tetrahedra[tetrahedron];
            for(std::size_t opposite = 0; opposite < 4; ++opposite){
                const std::size_t neighbor = removed.neighbors[opposite];
                if(neighbor == no_tetrahedron || bad_stamp[neighbor] != epoch){
                    boundary.push_back({
                        opposite_face(removed.vertices, opposite), neighbor
                    });
                }
            }
            for(std::size_t candidate: removed.conflicts){
                if(candidate == point || inserted[candidate]
                    || candidate_stamp[candidate] == epoch) continue;
                candidate_stamp[candidate] = epoch;
                candidates.push_back(candidate);
            }
        }
        for(std::size_t tetrahedron: bad) tetrahedra[tetrahedron].alive = false;

        std::vector<std::size_t> created;
        created.reserve(boundary.size());
        std::vector<std::size_t> boundary_slots;
        boundary_slots.reserve(boundary.size());
        for(const BoundaryFace& face: boundary){
            std::array<std::size_t, 4> vertices{
                face.vertices[0], face.vertices[1], face.vertices[2], point
            };
            if(adaptive_orient3d(
                predicates[vertices[0]], predicates[vertices[1]],
                predicates[vertices[2]], predicates[vertices[3]]
            ) == 0){
                throw std::logic_error("degenerate Delaunay cavity boundary");
            }
            TetrahedronIndex tetrahedron;
            tetrahedron.vertices = positive_tetrahedron(vertices, predicates);
            const std::size_t identifier = tetrahedra.size();
            tetrahedra.push_back(std::move(tetrahedron));
            bad_stamp.push_back(0);
            created.push_back(identifier);
            std::size_t boundary_slot = 4;
            for(std::size_t opposite = 0; opposite < 4; ++opposite){
                if(opposite_face(tetrahedra[identifier].vertices, opposite)
                    == face.vertices){
                    boundary_slot = opposite;
                    break;
                }
            }
            if(boundary_slot == 4){
                throw std::logic_error("new Delaunay tetrahedron lost its boundary");
            }
            boundary_slots.push_back(boundary_slot);
            tetrahedra[identifier].neighbors[boundary_slot] = face.outside;
            if(face.outside != no_tetrahedron){
                connect_across_face(
                    tetrahedra, face.outside, face.vertices, identifier
                );
            }
        }
        if(created.empty()) throw std::logic_error("Delaunay cavity has no boundary");

        std::unordered_map<
            std::array<std::size_t, 3>,
            std::pair<std::size_t, std::size_t>,
            FaceHash
        > unmatched;
        unmatched.reserve(created.size() * 3);
        for(std::size_t index = 0; index < created.size(); ++index){
            const std::size_t tetrahedron = created[index];
            for(std::size_t opposite = 0; opposite < 4; ++opposite){
                if(opposite == boundary_slots[index]) continue;
                const auto face = opposite_face(
                    tetrahedra[tetrahedron].vertices, opposite
                );
                const auto [iterator, fresh] = unmatched.emplace(
                    face, std::pair{tetrahedron, opposite}
                );
                if(fresh) continue;
                const auto [other, other_opposite] = iterator->second;
                tetrahedra[tetrahedron].neighbors[opposite] = other;
                tetrahedra[other].neighbors[other_opposite] = tetrahedron;
                unmatched.erase(iterator);
            }
        }
        if(!unmatched.empty()){
            throw std::logic_error("Delaunay cavity boundary is not a closed ball");
        }

        for(std::size_t tetrahedron: created){
            for(std::size_t candidate: candidates){
                if(insphere_side(
                    tetrahedra[tetrahedron], predicates, candidate
                ) <= 0) continue;
                tetrahedra[tetrahedron].conflicts.push_back(candidate);
                point_conflicts[candidate].push_back(tetrahedron);
            }
        }
        point_conflicts[point].clear();
        inserted[point] = true;
        if(bad_stamp[root] == epoch) root = created.front();
    }

    std::unordered_set<std::array<std::size_t, 4>, TetrahedronHash> seen;
    std::vector<std::array<std::size_t, 4>> result;
    for(const TetrahedronIndex& tetrahedron: tetrahedra){
        if(!tetrahedron.alive) continue;
        if(std::any_of(
            tetrahedron.vertices.begin(), tetrahedron.vertices.end(),
            [&](std::size_t vertex){ return vertex >= original_size; }
        )) continue;
        std::array<std::size_t, 4> key = tetrahedron.vertices;
        std::sort(key.begin(), key.end());
        if(seen.insert(key).second) result.push_back(tetrahedron.vertices);
    }
    return result;
}
inline geometry3d_adaptive_detail::ExactDyadic exact_tetrahedron_measure(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    using namespace geometry3d_adaptive_detail;
    const ExactDyadic ux = subtract(exact_dyadic(second.x), exact_dyadic(first.x));
    const ExactDyadic uy = subtract(exact_dyadic(second.y), exact_dyadic(first.y));
    const ExactDyadic uz = subtract(exact_dyadic(second.z), exact_dyadic(first.z));
    const ExactDyadic vx = subtract(exact_dyadic(third.x), exact_dyadic(first.x));
    const ExactDyadic vy = subtract(exact_dyadic(third.y), exact_dyadic(first.y));
    const ExactDyadic vz = subtract(exact_dyadic(third.z), exact_dyadic(first.z));
    const ExactDyadic wx = subtract(exact_dyadic(fourth.x), exact_dyadic(first.x));
    const ExactDyadic wy = subtract(exact_dyadic(fourth.y), exact_dyadic(first.y));
    const ExactDyadic wz = subtract(exact_dyadic(fourth.z), exact_dyadic(first.z));
    return add(
        subtract(
            multiply(ux, subtract(multiply(vy, wz), multiply(vz, wy))),
            multiply(uy, subtract(multiply(vx, wz), multiply(vz, wx)))
        ),
        multiply(uz, subtract(multiply(vx, wy), multiply(vy, wx)))
    );
}

inline geometry3d_adaptive_detail::ExactDyadic absolute_measure(
    geometry3d_adaptive_detail::ExactDyadic value
){
    if(geometry3d_adaptive_detail::sign(value) < 0){
        value = geometry3d_adaptive_detail::negate(std::move(value));
    }
    return value;
}

inline bool is_complete_tetrahedralization(
    const std::vector<std::array<std::size_t, 4>>& tetrahedra,
    const std::vector<Point3>& points
){
    if(tetrahedra.empty()) return false;
    std::vector<bool> used(points.size(), false);
    using namespace geometry3d_adaptive_detail;
    std::unordered_map<std::array<std::size_t, 3>, int, FaceHash> face_count;
    face_count.reserve(tetrahedra.size() * 2);
    for(const auto& tetrahedron: tetrahedra){
        for(std::size_t vertex: tetrahedron){
            if(vertex >= points.size()) return false;
            used[vertex] = true;
        }
        if(adaptive_orient3d(
            points[tetrahedron[0]], points[tetrahedron[1]],
            points[tetrahedron[2]], points[tetrahedron[3]]
        ) <= 0) return false;
        for(std::size_t opposite = 0; opposite < 4; ++opposite){
            if(++face_count[opposite_face(tetrahedron, opposite)] > 2){
                return false;
            }
        }
    }
    if(std::find(used.begin(), used.end(), false) != used.end()) return false;

    ExactDyadic tetrahedra_measure{};
    for(const auto& tetrahedron: tetrahedra){
        tetrahedra_measure = add(tetrahedra_measure, exact_tetrahedron_measure(
            points[tetrahedron[0]], points[tetrahedron[1]],
            points[tetrahedron[2]], points[tetrahedron[3]]
        ));
    }
    const ConvexPolyhedron3 hull = convex_hull_3d(points);
    ExactDyadic hull_measure{};
    const Point3& anchor = points.front();
    for(const auto& face: hull.faces){
        hull_measure = add(hull_measure, absolute_measure(
            exact_tetrahedron_measure(
                anchor,
                hull.vertices[face[0]], hull.vertices[face[1]],
                hull.vertices[face[2]]
            )
        ));
    }
    return sign(subtract(tetrahedra_measure, hull_measure)) == 0;
}

}  // namespace delaunay_tetrahedralization_3d_detail

inline DelaunayTetrahedralization3
delaunay_tetrahedralization_3d_randomized(
    std::vector<Point3> input, std::uint64_t seed
){
    using namespace delaunay_tetrahedralization_3d_detail;
    std::vector<Point3> vertices = exact_unique_points(std::move(input));
    const int dimension = affine_dimension(vertices);
    if(dimension < 3) return {dimension, std::move(vertices), {}};

    const std::vector<Point3> normalized = normalized_points(vertices);
    long double super_extent = 16.0L;
    for(int attempt = 0; attempt < 12; ++attempt){
        auto tetrahedra = bowyer_watson(normalized, super_extent, seed + attempt);
        if(is_complete_tetrahedralization(tetrahedra, normalized)){
            return {3, std::move(vertices), std::move(tetrahedra)};
        }
        super_extent *= 256.0L;
        if(!std::isfinite(super_extent)) break;
    }
    throw std::overflow_error(
        "could not construct a sufficiently large Delaunay super tetrahedron"
    );
}

inline DelaunayTetrahedralization3 delaunay_tetrahedralization_3d(
    std::vector<Point3> input
){
    return delaunay_tetrahedralization_3d_randomized(
        std::move(input), 0x243f6a8885a308d3ULL
    );
}
