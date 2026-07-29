#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_PLANE_POINT_HALFSPACE_INTERSECTION_3D_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_PLANE_POINT_HALFSPACE_INTERSECTION_3D_DETAIL_HPP_INCLUDED

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

#include "../../polyhedron/point_set/convex_hull_3d_default.hpp"
#include "../../type/definition/convex_polyhedron3.hpp"
#include "../../point/point_point/cross.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../aggregate/all/is_finite.hpp"
#include "../../aggregate/all/expansion_arithmetic.hpp"
#include "../../point/line_plane/line_plane_intersection.hpp"
#include "../../predicate/point_point/parallel.hpp"
#include "../../predicate/plane_point/on_plane.hpp"
#include "../../point/plane/plane3_unit_normal.hpp"
#include "../../line/plane_plane/plane_plane_intersection.hpp"
#include "../../aggregate/all/projection.hpp"
#include "../../aggregate/all/three_plane_intersection.hpp"

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
    std::array<long double, 4> approximate{};
    long double approximation_error =
        std::numeric_limits<long double>::infinity();
    std::array<Geometry3DExpansion, 4> expansion{};
    Point3 cartesian{};
    long double cartesian_error =
        std::numeric_limits<long double>::infinity();
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

inline std::optional<int> approximate_determinant4_sign(
    const std::array<const DualPoint*, 4>& rows
){
    long double maximum_input_error = 0.0L;
    for(const DualPoint* row: rows){
        if(!std::isfinite(row->approximation_error)) return std::nullopt;
        maximum_input_error = std::max(
            maximum_input_error, row->approximation_error
        );
    }
    std::array<std::size_t, 4> permutation{0, 1, 2, 3};
    long double determinant = 0.0L;
    long double permanent = 0.0L;
    do{
        long double product = 1.0L;
        int inversions = 0;
        for(std::size_t row = 0; row < 4; ++row){
            product *= rows[row]->approximate[permutation[row]];
            for(std::size_t previous = 0; previous < row; ++previous){
                if(permutation[row] < permutation[previous]) ++inversions;
            }
        }
        permanent += std::abs(product);
        determinant += inversions % 2 == 0 ? product : -product;
    }while(std::next_permutation(permutation.begin(), permutation.end()));
    const long double error =
        512.0L * (
            std::numeric_limits<long double>::epsilon() * permanent
            + maximum_input_error
        );
    if(std::isfinite(determinant) && std::isfinite(error)
        && std::abs(determinant) > error){
        return determinant > 0.0L ? 1 : -1;
    }
    return std::nullopt;
}

inline std::optional<int> expansion_determinant4_sign(
    const std::array<const DualPoint*, 4>& rows
){
    try{
        std::array<std::size_t, 4> permutation{0, 1, 2, 3};
        Geometry3DExpansion determinant;
        do{
            Geometry3DExpansion product{1.0L};
            int inversions = 0;
            for(std::size_t row = 0; row < 4; ++row){
                product = geometry3d_expansion_product(
                    product, rows[row]->expansion[permutation[row]]
                );
                for(std::size_t previous = 0; previous < row; ++previous){
                    if(permutation[row] < permutation[previous]) ++inversions;
                }
            }
            if(inversions % 2 != 0){
                product = geometry3d_expansion_negate(std::move(product));
            }
            determinant = geometry3d_expansion_sum(determinant, product);
        }while(std::next_permutation(
            permutation.begin(), permutation.end()
        ));
        return geometry3d_expansion_sign(determinant);
    }catch(const std::overflow_error&){
        return std::nullopt;
    }
}

inline ExactDyadic expansion_to_exact_dyadic(
    const Geometry3DExpansion& expansion
){
    ExactDyadic result;
    for(const long double component: expansion){
        result = geometry3d_adaptive_detail::add(
            result, geometry3d_adaptive_detail::exact_dyadic(component)
        );
    }
    return result;
}

inline Geometry3DExpansion expansion_determinant3(
    const std::array<std::array<Geometry3DExpansion, 3>, 3>& rows
){
    std::array<std::size_t, 3> permutation{0, 1, 2};
    Geometry3DExpansion determinant;
    do{
        Geometry3DExpansion product{1.0L};
        int inversions = 0;
        for(std::size_t row = 0; row < 3; ++row){
            product = geometry3d_expansion_product(
                product, rows[row][permutation[row]]
            );
            for(std::size_t previous = 0; previous < row; ++previous){
                if(permutation[row] < permutation[previous]) ++inversions;
            }
        }
        if(inversions % 2 != 0){
            product = geometry3d_expansion_negate(std::move(product));
        }
        determinant = geometry3d_expansion_sum(determinant, product);
    }while(std::next_permutation(permutation.begin(), permutation.end()));
    return determinant;
}

inline Point3 exact_three_boundary_point(
    const Plane3& first,
    const Plane3& second,
    const Plane3& third
){
    try{
        const std::array<const Plane3*, 3> source{&first, &second, &third};
        std::array<std::array<Geometry3DExpansion, 3>, 3> coefficients;
        std::array<Geometry3DExpansion, 3> offsets;
        for(std::size_t row = 0; row < 3; ++row){
            coefficients[row] = {
                Geometry3DExpansion{source[row]->normal.x},
                Geometry3DExpansion{source[row]->normal.y},
                Geometry3DExpansion{source[row]->normal.z},
            };
            offsets[row] = geometry3d_expansion_sum(
                geometry3d_expansion_sum(
                    geometry3d_expansion_scale(
                        geometry3d_two_diff(source[row]->point.x, 0.0L),
                        source[row]->normal.x
                    ),
                    geometry3d_expansion_scale(
                        geometry3d_two_diff(source[row]->point.y, 0.0L),
                        source[row]->normal.y
                    )
                ),
                geometry3d_expansion_scale(
                    geometry3d_two_diff(source[row]->point.z, 0.0L),
                    source[row]->normal.z
                )
            );
        }
        const ExactDyadic denominator = expansion_to_exact_dyadic(
            expansion_determinant3(coefficients)
        );
        if(geometry3d_adaptive_detail::sign(denominator) == 0){
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
                expansion_to_exact_dyadic(expansion_determinant3(matrix)),
                denominator,
                "halfspace intersection vertex is not representable"
            );
        };
        return {coordinate(0), coordinate(1), coordinate(2)};
    }catch(const std::overflow_error&){
        // Expansion overflow falls through to arbitrary-precision products.
    }
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

inline std::optional<int> approximate_dual_orientation(
    const DualPoint& first,
    const DualPoint& second,
    const DualPoint& third,
    const DualPoint& fourth
){
    const long double input_error = std::max({
        first.cartesian_error, second.cartesian_error,
        third.cartesian_error, fourth.cartesian_error,
    });
    if(!std::isfinite(input_error)) return std::nullopt;
    const long double scale = std::max({
        std::abs(first.cartesian.x), std::abs(first.cartesian.y),
        std::abs(first.cartesian.z), std::abs(second.cartesian.x),
        std::abs(second.cartesian.y), std::abs(second.cartesian.z),
        std::abs(third.cartesian.x), std::abs(third.cartesian.y),
        std::abs(third.cartesian.z), std::abs(fourth.cartesian.x),
        std::abs(fourth.cartesian.y), std::abs(fourth.cartesian.z),
        1.0L,
    });
    if(!std::isfinite(scale)) return std::nullopt;
    const auto normalized = [scale](const Point3& point){
        return point / scale;
    };
    const Point3 origin = normalized(first.cartesian);
    const Point3 u = normalized(second.cartesian) - origin;
    const Point3 v = normalized(third.cartesian) - origin;
    const Point3 w = normalized(fourth.cartesian) - origin;
    const long double first_minor = std::fma(v.y, w.z, -v.z * w.y);
    const long double second_minor = std::fma(v.x, w.z, -v.z * w.x);
    const long double third_minor = std::fma(v.x, w.y, -v.y * w.x);
    const long double first_term = u.x * first_minor;
    const long double second_term = u.y * second_minor;
    const long double third_term = u.z * third_minor;
    const long double determinant =
        std::fma(u.x, first_minor,
            std::fma(-u.y, second_minor, third_term));
    const long double permanent =
        std::abs(first_term) + std::abs(second_term)
        + std::abs(third_term);
    const long double normalized_error = input_error / scale;
    const long double error =
        64.0L * std::numeric_limits<long double>::epsilon() * permanent
        + 512.0L * normalized_error;
    if(std::isfinite(determinant) && std::isfinite(error)
        && std::abs(determinant) > error){
        return determinant > 0.0L ? 1 : -1;
    }
    return std::nullopt;
}

inline int dual_orientation(
    const std::vector<DualPoint>& points,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    std::size_t fourth
){
    if(const auto approximate = approximate_dual_orientation(
        points[first], points[second], points[third], points[fourth]
    )){
        return *approximate;
    }
    if(const auto expansion = expansion_determinant4_sign({
        &points[first], &points[second], &points[third], &points[fourth]
    })){
        return *expansion;
    }
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
    const DualPoint approximate_origin{
        origin, 0, {1.0L, 0.0L, 0.0L, 0.0L}, 0.0L,
        {Geometry3DExpansion{1.0L}, {}, {}, {}}
    };
    if(const auto approximate = approximate_determinant4_sign({
        &points[face[0]], &points[face[1]], &points[face[2]],
        &approximate_origin
    })){
        return *approximate;
    }
    if(const auto expansion = expansion_determinant4_sign({
        &points[face[0]], &points[face[1]], &points[face[2]],
        &approximate_origin
    })){
        return *expansion;
    }
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
        if(witness == first || witness == second || witness == third){
            continue;
        }
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
        DualPoint dual_point{{
            negate(signed_distance.value),
            exact_dyadic(halfspace.normal.x),
            exact_dyadic(halfspace.normal.y),
            exact_dyadic(halfspace.normal.z),
        }, index};
        const Point3 difference = interior - halfspace.point;
        const long double first_product =
            halfspace.normal.x * difference.x;
        const long double second_product =
            halfspace.normal.y * difference.y;
        const long double third_product =
            halfspace.normal.z * difference.z;
        const long double approximate_distance = std::fma(
            halfspace.normal.x, difference.x,
            std::fma(
                halfspace.normal.y, difference.y, third_product
            )
        );
        const long double magnitude =
            std::abs(first_product) + std::abs(second_product)
            + std::abs(third_product);
        const long double homogeneous_scale = std::max({
            std::abs(approximate_distance),
            std::abs(halfspace.normal.x),
            std::abs(halfspace.normal.y),
            std::abs(halfspace.normal.z),
        });
        if(geometry3d_is_finite(difference)
            && std::isfinite(approximate_distance)
            && std::isfinite(magnitude)
            && std::isfinite(homogeneous_scale)
            && homogeneous_scale > 0.0L){
            dual_point.approximate = {
                -approximate_distance / homogeneous_scale,
                halfspace.normal.x / homogeneous_scale,
                halfspace.normal.y / homogeneous_scale,
                halfspace.normal.z / homogeneous_scale,
            };
            dual_point.approximation_error =
                16.0L * std::numeric_limits<long double>::epsilon()
                * (1.0L + magnitude / homogeneous_scale);
            const long double denominator = dual_point.approximate[0];
            const long double denominator_lower =
                denominator - dual_point.approximation_error;
            if(denominator_lower > 0.0L){
                dual_point.cartesian = {
                    dual_point.approximate[1] / denominator,
                    dual_point.approximate[2] / denominator,
                    dual_point.approximate[3] / denominator,
                };
                long double ratio_error = 0.0L;
                for(std::size_t coordinate = 1; coordinate < 4; ++coordinate){
                    ratio_error = std::max(
                        ratio_error,
                        dual_point.approximation_error * (
                            std::abs(dual_point.approximate[coordinate])
                            + std::abs(denominator)
                        ) / (std::abs(denominator) * denominator_lower)
                    );
                }
                dual_point.cartesian_error =
                    ratio_error
                    + 4.0L * std::numeric_limits<long double>::epsilon()
                    * std::max({
                        std::abs(dual_point.cartesian.x),
                        std::abs(dual_point.cartesian.y),
                        std::abs(dual_point.cartesian.z),
                        1.0L,
                    });
            }
        }
        const auto expansion_term = [&](long double coefficient,
                                        long double first,
                                        long double second){
            return geometry3d_expansion_scale(
                geometry3d_two_diff(first, second), coefficient
            );
        };
        dual_point.expansion = {
            geometry3d_expansion_negate(geometry3d_expansion_sum(
                geometry3d_expansion_sum(
                    expansion_term(
                        halfspace.normal.x, interior.x, halfspace.point.x
                    ),
                    expansion_term(
                        halfspace.normal.y, interior.y, halfspace.point.y
                    )
                ),
                expansion_term(
                    halfspace.normal.z, interior.z, halfspace.point.z
                )
            )),
            Geometry3DExpansion{halfspace.normal.x},
            Geometry3DExpansion{halfspace.normal.y},
            Geometry3DExpansion{halfspace.normal.z},
        };
        dual_points.push_back(std::move(dual_point));
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
    std::map<DualEdge, DualEdgeIncidence> edge_incidences;
    bool incidence_valid = true;
    for(std::size_t face_index = 0; face_index < faces->size(); ++face_index){
        const auto& face = (*faces)[face_index];
        for(std::size_t edge = 0; edge < 3; ++edge){
            const DualEdge key = dual_edge(
                face[edge], face[(edge + 1) % 3]
            );
            DualEdgeIncidence& incidence = edge_incidences[key];
            if(incidence.first == no_dual_index){
                incidence.first = face_index;
                continue;
            }
            if(incidence.second != no_dual_index){
                incidence_valid = false;
                continue;
            }
            incidence.second = face_index;
            const auto& other = (*faces)[incidence.first];
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
                unite(face_index, incidence.first);
            }
        }
    }

    using CellCoordinate = std::pair<bool, long double>;
    using CellKey = std::array<CellCoordinate, 3>;
    std::map<CellKey, std::vector<std::size_t>> vertex_cells;
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
                    for(const std::size_t vertex: iterator->second){
                        if(point == vertices[vertex]) return vertex;
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
        const std::size_t index = vertices.size();
        vertex_cells[{
            central(point.x), central(point.y), central(point.z)
        }].push_back(index);
        vertices.push_back(point);
        return index;
    };

    std::vector<std::size_t> face_vertex(
        faces->size(), no_dual_index
    );
    for(std::size_t face_index = 0; face_index < faces->size(); ++face_index){
        if(find_root(face_index) != face_index) continue;
        const auto& face = (*faces)[face_index];
        face_vertex[face_index] = add_unique_vertex(
            exact_three_boundary_point(
                halfspaces[dual_points[face[0]].constraint],
                halfspaces[dual_points[face[1]].constraint],
                halfspaces[dual_points[face[2]].constraint]
            )
        );
    }
    for(std::size_t face_index = 0; face_index < faces->size(); ++face_index){
        face_vertex[face_index] = face_vertex[find_root(face_index)];
    }

    std::vector<std::vector<DualEdge>> facet_edges(dual_points.size());
    for(const auto& [edge, incidence]: edge_incidences){
        if(incidence.first == no_dual_index
            || incidence.second == no_dual_index){
            incidence_valid = false;
            continue;
        }
        const std::size_t first = face_vertex[incidence.first];
        const std::size_t second = face_vertex[incidence.second];
        if(first == no_dual_index || second == no_dual_index){
            incidence_valid = false;
            continue;
        }
        if(first == second) continue;
        const DualEdge primal_edge = dual_edge(first, second);
        facet_edges[edge.first].push_back(primal_edge);
        facet_edges[edge.second].push_back(primal_edge);
    }

    const auto exact_facet_turn = [](
        const Point3& normal,
        const Point3& first,
        const Point3& second,
        const Point3& third
    ){
        using namespace geometry3d_adaptive_detail;
        const auto left =
            geometry3d_plane_numeric_detail::exact_difference(second, first);
        const auto right =
            geometry3d_plane_numeric_detail::exact_difference(third, first);
        const std::array<ExactDyadic, 3> product{
            subtract(multiply(left[1], right[2]),
                     multiply(left[2], right[1])),
            subtract(multiply(left[2], right[0]),
                     multiply(left[0], right[2])),
            subtract(multiply(left[0], right[1]),
                     multiply(left[1], right[0])),
        };
        return sign(
            geometry3d_plane_numeric_detail::exact_dot(normal, product).value
        );
    };

    std::vector<std::array<std::size_t, 3>> primal_faces;
    primal_faces.reserve(edge_incidences.size() * 2);
    for(std::size_t dual_vertex = 0;
        dual_vertex < facet_edges.size() && incidence_valid;
        ++dual_vertex){
        auto& edges = facet_edges[dual_vertex];
        std::sort(edges.begin(), edges.end());
        edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
        if(edges.empty()) continue;
        if(edges.size() < 3){
            incidence_valid = false;
            break;
        }
        std::map<std::size_t, std::vector<std::size_t>> adjacency;
        for(const DualEdge& edge: edges){
            adjacency[edge.first].push_back(edge.second);
            adjacency[edge.second].push_back(edge.first);
        }
        for(auto& [vertex, neighbors]: adjacency){
            static_cast<void>(vertex);
            std::sort(neighbors.begin(), neighbors.end());
            neighbors.erase(
                std::unique(neighbors.begin(), neighbors.end()),
                neighbors.end()
            );
            if(neighbors.size() != 2){
                incidence_valid = false;
                break;
            }
        }
        if(!incidence_valid) break;

        std::vector<std::size_t> cycle;
        cycle.reserve(adjacency.size());
        const std::size_t start = adjacency.begin()->first;
        std::size_t previous = no_dual_index;
        std::size_t current = start;
        do{
            if(cycle.size() == adjacency.size()){
                incidence_valid = false;
                break;
            }
            cycle.push_back(current);
            const auto iterator = adjacency.find(current);
            if(iterator == adjacency.end()){
                incidence_valid = false;
                break;
            }
            const auto& neighbors = iterator->second;
            const std::size_t next =
                neighbors[0] == previous ? neighbors[1] : neighbors[0];
            previous = current;
            current = next;
        }while(current != start);
        if(!incidence_valid || cycle.size() != adjacency.size()
            || cycle.size() < 3){
            incidence_valid = false;
            break;
        }

        const Point3& normal =
            halfspaces[dual_points[dual_vertex].constraint].normal;
        int turn = exact_facet_turn(
            normal,
            vertices[cycle[0]], vertices[cycle[1]], vertices[cycle[2]]
        );
        if(turn == 0){
            incidence_valid = false;
            break;
        }
        if(turn < 0) std::reverse(cycle.begin() + 1, cycle.end());
        for(std::size_t index = 1; index + 1 < cycle.size(); ++index){
            if(exact_facet_turn(
                normal,
                vertices[cycle[0]],
                vertices[cycle[index]],
                vertices[cycle[index + 1]]
            ) <= 0){
                incidence_valid = false;
                break;
            }
            primal_faces.push_back({
                cycle[0], cycle[index], cycle[index + 1]
            });
        }
    }

    if(!incidence_valid || vertices.size() < 4 || primal_faces.size() < 4){
        ConvexPolyhedron3 result = convex_hull_3d(std::move(vertices));
        if(result.affine_dimension != 3)[[unlikely]]{
            throw std::logic_error(
                "strictly feasible bounded halfspaces produced a "
                "degenerate hull"
            );
        }
        return result;
    }

    long double scale = 0.0L;
    for(const Point3& vertex: vertices){
        scale = std::max({
            scale, std::abs(vertex.x), std::abs(vertex.y), std::abs(vertex.z)
        });
    }
    long double signed_six_volume = 0.0L;
    if(scale > 0.0L && std::isfinite(scale)){
        const Point3 reference = vertices.front() / scale;
        for(const auto& face: primal_faces){
            const Point3 first = vertices[face[0]] / scale - reference;
            const Point3 second = vertices[face[1]] / scale - reference;
            const Point3 third = vertices[face[2]] / scale - reference;
            signed_six_volume += dot(first, cross(second, third));
        }
    }
    if(signed_six_volume < 0.0L){
        for(auto& face: primal_faces) std::swap(face[1], face[2]);
    }
    return {3, std::move(vertices), std::move(primal_faces)};

}

}  // namespace halfspace_intersection_3d_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_PLANE_POINT_HALFSPACE_INTERSECTION_3D_DETAIL_HPP_INCLUDED
