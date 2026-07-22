#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_DIVIDE_AND_CONQUER_CONVEX_HULL_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_DIVIDE_AND_CONQUER_CONVEX_HULL_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../predicate/adaptive_orient3d.hpp"
#include "../core/convex_polyhedron3.hpp"
#include "../predicate/is_finite.hpp"

namespace divide_and_conquer_convex_hull_3d_detail{

inline bool same_point(const Point3& left, const Point3& right){
    return left.x == right.x && left.y == right.y && left.z == right.z;
}

inline int exact_turn(
    long double origin_x,
    long double origin_y,
    long double first_x,
    long double first_y,
    long double second_x,
    long double second_y
){
    using namespace geometry3d_adaptive_detail;
    const ExactDyadic first_dx = subtract(
        exact_dyadic(first_x), exact_dyadic(origin_x)
    );
    const ExactDyadic first_dy = subtract(
        exact_dyadic(first_y), exact_dyadic(origin_y)
    );
    const ExactDyadic second_dx = subtract(
        exact_dyadic(second_x), exact_dyadic(origin_x)
    );
    const ExactDyadic second_dy = subtract(
        exact_dyadic(second_y), exact_dyadic(origin_y)
    );
    return sign(subtract(
        multiply(first_dx, second_dy), multiply(first_dy, second_dx)
    ));
}

inline bool non_collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return exact_turn(
        first.x, first.y, second.x, second.y, third.x, third.y
    ) != 0 || exact_turn(
        first.x, first.z, second.x, second.z, third.x, third.z
    ) != 0 || exact_turn(
        first.y, first.z, second.y, second.z, third.y, third.z
    ) != 0;
}

struct ProjectedPoint{
    long double first = 0.0L;
    long double second = 0.0L;
    std::size_t index = 0;
};

inline int projected_turn(
    const ProjectedPoint& origin,
    const ProjectedPoint& first,
    const ProjectedPoint& second
){
    return exact_turn(
        origin.first, origin.second, first.first, first.second,
        second.first, second.second
    );
}

inline std::vector<std::size_t> planar_boundary(
    const std::vector<Point3>& points,
    const std::vector<std::size_t>& coplanar,
    const std::array<std::size_t, 3>& plane
){
    const Point3& origin = points[plane[0]];
    const Point3& first = points[plane[1]];
    const Point3& second = points[plane[2]];
    std::size_t dropped = 2;
    if(exact_turn(
        origin.y, origin.z, first.y, first.z, second.y, second.z
    ) != 0){
        dropped = 0;
    }else if(exact_turn(
        origin.x, origin.z, first.x, first.z, second.x, second.z
    ) != 0){
        dropped = 1;
    }

    std::vector<ProjectedPoint> projected;
    projected.reserve(coplanar.size());
    for(std::size_t index: coplanar){
        const Point3& point = points[index];
        if(dropped == 0) projected.push_back({point.y, point.z, index});
        if(dropped == 1) projected.push_back({point.x, point.z, index});
        if(dropped == 2) projected.push_back({point.x, point.y, index});
    }
    std::sort(projected.begin(), projected.end(), [](const auto& left,
                                                     const auto& right){
        if(left.first != right.first) return left.first < right.first;
        if(left.second != right.second) return left.second < right.second;
        return left.index < right.index;
    });

    std::vector<ProjectedPoint> boundary;
    boundary.reserve(projected.size() * 2);
    for(const ProjectedPoint& point: projected){
        while(boundary.size() >= 2 && projected_turn(
            boundary[boundary.size() - 2], boundary.back(), point
        ) <= 0) boundary.pop_back();
        boundary.push_back(point);
    }
    const std::size_t lower_size = boundary.size();
    for(std::size_t index = projected.size(); index-- > 0;){
        const ProjectedPoint& point = projected[index];
        while(boundary.size() > lower_size && projected_turn(
            boundary[boundary.size() - 2], boundary.back(), point
        ) <= 0) boundary.pop_back();
        boundary.push_back(point);
    }
    if(boundary.size() > 1) boundary.pop_back();

    std::vector<std::size_t> result;
    result.reserve(boundary.size());
    for(const ProjectedPoint& point: boundary) result.push_back(point.index);
    return result;
}

inline ConvexPolyhedron3 lower_dimensional_hull(
    const std::vector<Point3>& points
){
    if(points.size() <= 1){
        return {points.empty() ? -1 : 0, points, {}};
    }
    std::size_t third = points.size();
    for(std::size_t index = 2; index < points.size(); ++index){
        if(non_collinear(points[0], points[1], points[index])){
            third = index;
            break;
        }
    }
    if(third == points.size()) return {1, {points.front(), points.back()}, {}};

    std::vector<std::size_t> all(points.size());
    for(std::size_t index = 0; index < points.size(); ++index) all[index] = index;
    const std::array<std::size_t, 3> plane{0, 1, third};
    const std::vector<std::size_t> boundary = planar_boundary(
        points, all, plane
    );
    std::vector<Point3> vertices;
    vertices.reserve(boundary.size());
    for(std::size_t index: boundary) vertices.push_back(points[index]);
    std::vector<std::array<std::size_t, 3>> faces;
    for(std::size_t index = 1; index + 1 < vertices.size(); ++index){
        faces.push_back({0, index, index + 1});
    }
    return {2, std::move(vertices), std::move(faces)};
}

struct SymbolicMonomial{
    std::array<std::size_t, 4> variables{};
    std::size_t size = 0;
};

struct SymbolicMonomialLess{
    bool operator()(
        const SymbolicMonomial& left,
        const SymbolicMonomial& right
    ) const{
        std::size_t left_size = left.size;
        std::size_t right_size = right.size;
        while(left_size != 0 || right_size != 0){
            std::size_t variable;
            if(left_size == 0){
                variable = right.variables[right_size - 1];
            }else if(right_size == 0){
                variable = left.variables[left_size - 1];
            }else{
                variable = std::max(
                    left.variables[left_size - 1],
                    right.variables[right_size - 1]
                );
            }
            std::size_t left_count = 0;
            while(left_size != 0
                && left.variables[left_size - 1] == variable){
                --left_size;
                ++left_count;
            }
            std::size_t right_count = 0;
            while(right_size != 0
                && right.variables[right_size - 1] == variable){
                --right_size;
                ++right_count;
            }
            if(left_count != right_count) return left_count < right_count;
        }
        return false;
    }
};

using SymbolicPolynomial = std::map<
    SymbolicMonomial,
    geometry3d_adaptive_detail::ExactDyadic,
    SymbolicMonomialLess
>;

inline void add_symbolic_term(
    SymbolicPolynomial& polynomial,
    const SymbolicMonomial& monomial,
    const geometry3d_adaptive_detail::ExactDyadic& coefficient
){
    using namespace geometry3d_adaptive_detail;
    if(sign(coefficient) == 0) return;
    const auto iterator = polynomial.find(monomial);
    if(iterator == polynomial.end()){
        polynomial.emplace(monomial, coefficient);
        return;
    }
    const ExactDyadic combined = add(iterator->second, coefficient);
    if(sign(combined) == 0){
        polynomial.erase(iterator);
    }else{
        iterator->second = combined;
    }
}

inline SymbolicPolynomial symbolic_constant(long double value){
    SymbolicPolynomial result;
    add_symbolic_term(
        result, {}, geometry3d_adaptive_detail::exact_dyadic(value)
    );
    return result;
}

inline SymbolicPolynomial symbolic_coordinate(
    const std::vector<Point3>& points,
    std::size_t point,
    std::size_t coordinate,
    int z_sign
){
    using namespace geometry3d_adaptive_detail;
    long double value = coordinate == 0 ? points[point].x
        : coordinate == 1 ? points[point].y : points[point].z;
    int perturbation_sign = 1;
    if(coordinate == 2 && z_sign < 0){
        value = -value;
        perturbation_sign = -1;
    }
    SymbolicPolynomial result = symbolic_constant(value);
    SymbolicMonomial variable;
    variable.variables[0] = point * 3 + coordinate;
    variable.size = 1;
    add_symbolic_term(
        result, variable, {ExactInteger(perturbation_sign), 0}
    );
    return result;
}

inline SymbolicPolynomial add_symbolic(
    SymbolicPolynomial result,
    const SymbolicPolynomial& other,
    int other_sign = 1
){
    using namespace geometry3d_adaptive_detail;
    for(const auto& [monomial, coefficient]: other){
        add_symbolic_term(
            result, monomial,
            other_sign < 0 ? negate(coefficient) : coefficient
        );
    }
    return result;
}

inline SymbolicMonomial multiply_monomials(
    const SymbolicMonomial& left,
    const SymbolicMonomial& right
){
    if(left.size + right.size > 4)[[unlikely]]{
        throw std::logic_error("symbolic perturbation degree overflow");
    }
    SymbolicMonomial result;
    result.size = left.size + right.size;
    std::merge(
        left.variables.begin(), left.variables.begin() + left.size,
        right.variables.begin(), right.variables.begin() + right.size,
        result.variables.begin()
    );
    return result;
}

inline SymbolicPolynomial multiply_symbolic(
    const SymbolicPolynomial& left,
    const SymbolicPolynomial& right
){
    using namespace geometry3d_adaptive_detail;
    SymbolicPolynomial result;
    for(const auto& [left_monomial, left_coefficient]: left){
        for(const auto& [right_monomial, right_coefficient]: right){
            add_symbolic_term(
                result,
                multiply_monomials(left_monomial, right_monomial),
                multiply(left_coefficient, right_coefficient)
            );
        }
    }
    return result;
}

inline int symbolic_sign(const SymbolicPolynomial& polynomial){
    if(polynomial.empty()) return 0;
    return geometry3d_adaptive_detail::sign(polynomial.begin()->second);
}

inline geometry3d_adaptive_detail::ExactDyadic exact_coordinate(
    const std::vector<Point3>& points,
    std::size_t point,
    std::size_t coordinate,
    int z_sign
){
    using namespace geometry3d_adaptive_detail;
    ExactDyadic result = exact_dyadic(
        coordinate == 0 ? points[point].x
        : coordinate == 1 ? points[point].y : points[point].z
    );
    if(coordinate == 2 && z_sign < 0) result = negate(result);
    return result;
}

inline geometry3d_adaptive_detail::ExactDyadic exact_projected_determinant(
    const std::vector<Point3>& points,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    std::size_t first_coordinate,
    std::size_t second_coordinate,
    int z_sign
){
    using namespace geometry3d_adaptive_detail;
    const ExactDyadic first_x = subtract(
        exact_coordinate(points, second, first_coordinate, z_sign),
        exact_coordinate(points, first, first_coordinate, z_sign)
    );
    const ExactDyadic first_y = subtract(
        exact_coordinate(points, second, second_coordinate, z_sign),
        exact_coordinate(points, first, second_coordinate, z_sign)
    );
    const ExactDyadic second_x = subtract(
        exact_coordinate(points, third, first_coordinate, z_sign),
        exact_coordinate(points, first, first_coordinate, z_sign)
    );
    const ExactDyadic second_y = subtract(
        exact_coordinate(points, third, second_coordinate, z_sign),
        exact_coordinate(points, first, second_coordinate, z_sign)
    );
    return subtract(
        multiply(first_x, second_y),
        multiply(first_y, second_x)
    );
}

inline SymbolicPolynomial symbolic_projected_determinant(
    const std::vector<Point3>& points,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    std::size_t first_coordinate,
    std::size_t second_coordinate,
    int z_sign
){
    SymbolicPolynomial first_x = add_symbolic(
        symbolic_coordinate(points, second, first_coordinate, z_sign),
        symbolic_coordinate(points, first, first_coordinate, z_sign), -1
    );
    SymbolicPolynomial first_y = add_symbolic(
        symbolic_coordinate(points, second, second_coordinate, z_sign),
        symbolic_coordinate(points, first, second_coordinate, z_sign), -1
    );
    SymbolicPolynomial second_x = add_symbolic(
        symbolic_coordinate(points, third, first_coordinate, z_sign),
        symbolic_coordinate(points, first, first_coordinate, z_sign), -1
    );
    SymbolicPolynomial second_y = add_symbolic(
        symbolic_coordinate(points, third, second_coordinate, z_sign),
        symbolic_coordinate(points, first, second_coordinate, z_sign), -1
    );
    return add_symbolic(
        multiply_symbolic(first_x, second_y),
        multiply_symbolic(first_y, second_x), -1
    );
}

inline int symbolic_projected_determinant_sign(
    const std::vector<Point3>& points,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    std::size_t first_coordinate,
    std::size_t second_coordinate,
    int z_sign
){
    using namespace geometry3d_adaptive_detail;
    const ExactDyadic exact = exact_projected_determinant(
        points, first, second, third,
        first_coordinate, second_coordinate, z_sign
    );
    const int exact_result = sign(exact);
    if(exact_result != 0) return exact_result;
    const int result = symbolic_sign(symbolic_projected_determinant(
        points, first, second, third,
        first_coordinate, second_coordinate, z_sign
    ));
    if(result == 0)[[unlikely]]{
        throw std::logic_error("symbolic projected determinant vanished");
    }
    return result;
}

struct ApproximateDeterminant{
    long double value = 0.0L;
    long double error = 0.0L;
    bool available = false;
};

inline ApproximateDeterminant approximate_projected_determinant(
    const std::vector<Point3>& points,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    std::size_t first_coordinate,
    std::size_t second_coordinate,
    int z_sign
){
    Point3 first_direction = points[second] - points[first];
    Point3 second_direction = points[third] - points[first];
    if(z_sign < 0){
        first_direction.z = -first_direction.z;
        second_direction.z = -second_direction.z;
    }
    const std::array<long double, 3> first_values{
        first_direction.x, first_direction.y, first_direction.z
    };
    const std::array<long double, 3> second_values{
        second_direction.x, second_direction.y, second_direction.z
    };
    const long double first_product =
        first_values[first_coordinate] * second_values[second_coordinate];
    const long double second_product =
        first_values[second_coordinate] * second_values[first_coordinate];
    const long double value = std::fma(
        first_values[first_coordinate],
        second_values[second_coordinate],
        -second_product
    );
    const long double magnitude =
        std::abs(first_product) + std::abs(second_product) + 1.0L;
    return {
        value,
        128.0L * std::numeric_limits<long double>::epsilon() * magnitude,
        true,
    };
}

inline constexpr std::size_t no_point =
    static_cast<std::size_t>(-1);

struct EventTime{
    int infinity = 1;
    std::array<std::size_t, 3> points{no_point, no_point, no_point};
    int z_sign = 1;
    ApproximateDeterminant numerator{};
    ApproximateDeterminant denominator{};
};

inline EventTime negative_infinity(){
    EventTime result;
    result.infinity = -1;
    return result;
}

inline EventTime positive_infinity(){
    return {};
}

inline EventTime event_time(
    const std::vector<Point3>& points,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    int z_sign
){
    if(first == no_point || second == no_point || third == no_point){
        return positive_infinity();
    }
    EventTime result;
    result.infinity = 0;
    result.points = {first, second, third};
    result.z_sign = z_sign;
    result.numerator = approximate_projected_determinant(
        points, first, second, third, 0, 2, z_sign
    );
    result.denominator = approximate_projected_determinant(
        points, first, second, third, 0, 1, z_sign
    );
    return result;
}

inline int event_denominator_sign(
    const EventTime& event,
    const std::vector<Point3>& points
){
    using namespace geometry3d_adaptive_detail;
    if(event.denominator.available
        && std::abs(event.denominator.value) > event.denominator.error){
        return event.denominator.value > 0.0L ? 1 : -1;
    }
    const ExactDyadic exact = exact_projected_determinant(
        points, event.points[0], event.points[1], event.points[2],
        0, 1, event.z_sign
    );
    const int exact_result = sign(exact);
    if(exact_result != 0) return exact_result;
    const int result = symbolic_sign(symbolic_projected_determinant(
        points, event.points[0], event.points[1], event.points[2],
        0, 1, event.z_sign
    ));
    if(result == 0)[[unlikely]]{
        throw std::logic_error("symbolic event denominator vanished");
    }
    return result;
}

inline SymbolicPolynomial event_numerator_polynomial(
    const EventTime& event,
    const std::vector<Point3>& points
){
    return symbolic_projected_determinant(
        points, event.points[0], event.points[1], event.points[2],
        0, 2, event.z_sign
    );
}

inline SymbolicPolynomial event_denominator_polynomial(
    const EventTime& event,
    const std::vector<Point3>& points
){
    return symbolic_projected_determinant(
        points, event.points[0], event.points[1], event.points[2],
        0, 1, event.z_sign
    );
}

inline int compare_event_times(
    const EventTime& left,
    const EventTime& right,
    const std::vector<Point3>& points
){
    using namespace geometry3d_adaptive_detail;
    if(left.infinity != 0 || right.infinity != 0){
        if(left.infinity == right.infinity) return 0;
        return left.infinity < right.infinity ? -1 : 1;
    }
    if(left.z_sign == right.z_sign){
        std::array<std::size_t, 3> left_points = left.points;
        std::array<std::size_t, 3> right_points = right.points;
        std::sort(left_points.begin(), left_points.end());
        std::sort(right_points.begin(), right_points.end());
        if(left_points == right_points) return 0;
    }
    const int denominator_product_sign =
        event_denominator_sign(left, points)
        * event_denominator_sign(right, points);
    int difference_sign = 0;
    if(left.numerator.available && left.denominator.available
        && right.numerator.available && right.denominator.available){
        const long double first_product =
            left.numerator.value * right.denominator.value;
        const long double second_product =
            right.numerator.value * left.denominator.value;
        const long double difference = std::fma(
            left.numerator.value,
            right.denominator.value,
            -second_product
        );
        const long double propagated_error =
            std::abs(left.numerator.value) * right.denominator.error
            + std::abs(right.denominator.value) * left.numerator.error
            + left.numerator.error * right.denominator.error
            + std::abs(right.numerator.value) * left.denominator.error
            + std::abs(left.denominator.value) * right.numerator.error
            + right.numerator.error * left.denominator.error;
        const long double rounding_error =
            16.0L * std::numeric_limits<long double>::epsilon()
            * (std::abs(first_product) + std::abs(second_product) + 1.0L);
        if(std::abs(difference) > propagated_error + rounding_error){
            difference_sign = difference > 0.0L ? 1 : -1;
        }
    }
    if(difference_sign == 0){
        const ExactDyadic left_numerator = exact_projected_determinant(
            points, left.points[0], left.points[1], left.points[2],
            0, 2, left.z_sign
        );
        const ExactDyadic left_denominator = exact_projected_determinant(
            points, left.points[0], left.points[1], left.points[2],
            0, 1, left.z_sign
        );
        const ExactDyadic right_numerator = exact_projected_determinant(
            points, right.points[0], right.points[1], right.points[2],
            0, 2, right.z_sign
        );
        const ExactDyadic right_denominator = exact_projected_determinant(
            points, right.points[0], right.points[1], right.points[2],
            0, 1, right.z_sign
        );
        const ExactDyadic exact_difference = subtract(
            multiply(left_numerator, right_denominator),
            multiply(right_numerator, left_denominator)
        );
        difference_sign = sign(exact_difference);
    }
    if(difference_sign == 0){
        const SymbolicPolynomial difference = add_symbolic(
            multiply_symbolic(
                event_numerator_polynomial(left, points),
                event_denominator_polynomial(right, points)
            ),
            multiply_symbolic(
                event_numerator_polynomial(right, points),
                event_denominator_polynomial(left, points)
            ),
            -1
        );
        difference_sign = symbolic_sign(difference);
    }
    return difference_sign * denominator_product_sign;
}

struct KineticNode{
    std::size_t previous = no_point;
    std::size_t next = no_point;
};

class DeterministicLowerHull{
public:
    DeterministicLowerHull(
        const std::vector<Point3>& input_points,
        int input_z_sign
    ):
        points(input_points),
        z_sign(input_z_sign),
        order(points.size()),
        nodes(points.size()){
        approximate_points.reserve(points.size());
        long double scale = 0.0L;
        for(const Point3& point: points){
            scale = std::max({
                scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
            });
        }
        if(scale == 0.0L) scale = 1.0L;
        for(const Point3& point: points){
            approximate_points.push_back({
                point.x / scale, point.y / scale, point.z / scale
            });
        }
        for(std::size_t index = 0; index < order.size(); ++index){
            order[index] = index;
        }
        std::sort(order.begin(), order.end(), [&](std::size_t left,
                                                  std::size_t right){
            return x_less(left, right);
        });
    }

    std::vector<std::array<std::size_t, 3>> faces(){
        std::vector<std::size_t> events = solve(0, order.size());
        std::vector<std::array<std::size_t, 3>> result;
        result.reserve(events.size());
        for(std::size_t point: events){
            if(nodes[point].previous == no_point
                || nodes[point].next == no_point)[[unlikely]]{
                throw std::logic_error(
                    "divide-and-conquer hull emitted an open event"
                );
            }
            result.push_back({
                nodes[point].previous, point, nodes[point].next
            });
            toggle(point);
        }
        return result;
    }

private:
    bool x_less(std::size_t left, std::size_t right) const{
        if(points[left].x != points[right].x){
            return points[left].x < points[right].x;
        }
        // x_i + epsilon^(base^(3i)) is larger for the smaller index.
        return left > right;
    }

    bool x_less_equal(std::size_t left, std::size_t right) const{
        return left == right || x_less(left, right);
    }

    int turn(
        std::size_t first,
        std::size_t second,
        std::size_t third
    ) const{
        if(first == no_point || second == no_point || third == no_point){
            return 1;
        }
        const ApproximateDeterminant approximate =
            approximate_projected_determinant(
                approximate_points, first, second, third, 0, 1, z_sign
            );
        if(std::abs(approximate.value) > approximate.error){
            return approximate.value > 0.0L ? 1 : -1;
        }
        return symbolic_projected_determinant_sign(
            points, first, second, third, 0, 1, z_sign
        );
    }

    EventTime time(
        std::size_t first,
        std::size_t second,
        std::size_t third
    ) const{
        return event_time(
            approximate_points, first, second, third, z_sign
        );
    }

    void toggle(std::size_t point){
        const std::size_t previous = nodes[point].previous;
        const std::size_t next = nodes[point].next;
        if(previous == no_point || next == no_point)[[unlikely]]{
            throw std::logic_error(
                "divide-and-conquer hull toggled an endpoint"
            );
        }
        if(nodes[previous].next != point){
            nodes[previous].next = point;
            nodes[next].previous = point;
        }else{
            nodes[previous].next = next;
            nodes[next].previous = previous;
        }
    }

    void append_event(
        std::vector<std::size_t>& events,
        std::size_t point,
        std::size_t point_count
    ) const{
        if(events.size() >= point_count * 2)[[unlikely]]{
            throw std::logic_error(
                "divide-and-conquer hull event bound exceeded"
            );
        }
        events.push_back(point);
    }

    std::vector<std::size_t> solve(
        std::size_t begin,
        std::size_t end
    ){
        const std::size_t point_count = end - begin;
        if(point_count == 1){
            nodes[order[begin]].previous = no_point;
            nodes[order[begin]].next = no_point;
            return {};
        }

        const std::size_t middle = begin + point_count / 2;
        std::size_t left_bridge = order[middle - 1];
        std::size_t right_bridge = order[middle];
        const std::size_t middle_point = right_bridge;
        std::vector<std::size_t> left_events = solve(begin, middle);
        std::vector<std::size_t> right_events = solve(middle, end);

        std::size_t bridge_steps = 0;
        while(true){
            if(turn(
                left_bridge, right_bridge, nodes[right_bridge].next
            ) < 0){
                right_bridge = nodes[right_bridge].next;
            }else if(turn(
                nodes[left_bridge].previous, left_bridge, right_bridge
            ) < 0){
                left_bridge = nodes[left_bridge].previous;
            }else{
                break;
            }
            if(++bridge_steps > point_count * 2 + 2)[[unlikely]]{
                throw std::logic_error(
                    "divide-and-conquer hull bridge search did not converge"
                );
            }
        }

        std::vector<std::size_t> events;
        events.reserve(point_count * 2);
        std::size_t left_event = 0;
        std::size_t right_event = 0;
        EventTime old_time = negative_infinity();
        while(true){
            const std::array<EventTime, 6> candidates{
                left_event == left_events.size()
                    ? positive_infinity()
                    : time(
                        nodes[left_events[left_event]].previous,
                        left_events[left_event],
                        nodes[left_events[left_event]].next
                    ),
                right_event == right_events.size()
                    ? positive_infinity()
                    : time(
                        nodes[right_events[right_event]].previous,
                        right_events[right_event],
                        nodes[right_events[right_event]].next
                    ),
                time(left_bridge, nodes[left_bridge].next, right_bridge),
                time(nodes[left_bridge].previous, left_bridge, right_bridge),
                time(left_bridge, nodes[right_bridge].previous, right_bridge),
                time(left_bridge, right_bridge, nodes[right_bridge].next),
            };
            EventTime next_time = positive_infinity();
            std::size_t selected = candidates.size();
            for(std::size_t index = 0; index < candidates.size(); ++index){
                if(compare_event_times(old_time, candidates[index], points) < 0
                    && compare_event_times(
                        candidates[index], next_time, points
                    ) < 0){
                    selected = index;
                    next_time = candidates[index];
                }
            }
            if(selected == candidates.size()) break;

            if(selected == 0){
                const std::size_t point = left_events[left_event++];
                if(x_less(point, left_bridge)){
                    append_event(events, point, point_count);
                }
                toggle(point);
            }else if(selected == 1){
                const std::size_t point = right_events[right_event++];
                if(x_less(right_bridge, point)){
                    append_event(events, point, point_count);
                }
                toggle(point);
            }else if(selected == 2){
                left_bridge = nodes[left_bridge].next;
                append_event(events, left_bridge, point_count);
            }else if(selected == 3){
                append_event(events, left_bridge, point_count);
                left_bridge = nodes[left_bridge].previous;
            }else if(selected == 4){
                right_bridge = nodes[right_bridge].previous;
                append_event(events, right_bridge, point_count);
            }else{
                append_event(events, right_bridge, point_count);
                right_bridge = nodes[right_bridge].next;
            }
            old_time = next_time;
        }

        nodes[left_bridge].next = right_bridge;
        nodes[right_bridge].previous = left_bridge;
        for(std::size_t index = events.size(); index-- > 0;){
            const std::size_t point = events[index];
            if(x_less_equal(point, left_bridge)
                || x_less_equal(right_bridge, point)){
                toggle(point);
                if(point == left_bridge){
                    left_bridge = nodes[point].previous;
                }else if(point == right_bridge){
                    right_bridge = nodes[point].next;
                }
            }else{
                nodes[left_bridge].next = point;
                nodes[point].previous = left_bridge;
                nodes[right_bridge].previous = point;
                nodes[point].next = right_bridge;
                if(x_less(point, middle_point)){
                    left_bridge = point;
                }else{
                    right_bridge = point;
                }
            }
        }
        return events;
    }

    const std::vector<Point3>& points;
    int z_sign;
    std::vector<std::size_t> order;
    std::vector<KineticNode> nodes;
    std::vector<Point3> approximate_points;
};

struct ExactPlane{
    std::array<geometry3d_adaptive_detail::ExactDyadic, 4> coefficients{};
    std::size_t pivot = 4;
};

inline ExactPlane exact_plane(
    const std::array<std::size_t, 3>& triangle,
    const std::vector<Point3>& points
){
    using namespace geometry3d_adaptive_detail;
    std::array<ExactDyadic, 3> first_direction;
    std::array<ExactDyadic, 3> second_direction;
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        first_direction[coordinate] = subtract(
            exact_coordinate(points, triangle[1], coordinate, 1),
            exact_coordinate(points, triangle[0], coordinate, 1)
        );
        second_direction[coordinate] = subtract(
            exact_coordinate(points, triangle[2], coordinate, 1),
            exact_coordinate(points, triangle[0], coordinate, 1)
        );
    }
    ExactPlane result;
    result.coefficients[0] = subtract(
        multiply(first_direction[1], second_direction[2]),
        multiply(first_direction[2], second_direction[1])
    );
    result.coefficients[1] = subtract(
        multiply(first_direction[2], second_direction[0]),
        multiply(first_direction[0], second_direction[2])
    );
    result.coefficients[2] = subtract(
        multiply(first_direction[0], second_direction[1]),
        multiply(first_direction[1], second_direction[0])
    );
    result.coefficients[3] = negate(add(
        add(
            multiply(
                result.coefficients[0],
                exact_coordinate(points, triangle[0], 0, 1)
            ),
            multiply(
                result.coefficients[1],
                exact_coordinate(points, triangle[0], 1, 1)
            )
        ),
        multiply(
            result.coefficients[2],
            exact_coordinate(points, triangle[0], 2, 1)
        )
    ));
    for(std::size_t index = 0; index < result.coefficients.size(); ++index){
        if(sign(result.coefficients[index]) != 0){
            result.pivot = index;
            break;
        }
    }
    if(result.pivot == result.coefficients.size())[[unlikely]]{
        throw std::logic_error(
            "divide-and-conquer hull constructed a zero plane"
        );
    }
    if(sign(result.coefficients[result.pivot]) < 0){
        for(ExactDyadic& coefficient: result.coefficients){
            coefficient = negate(coefficient);
        }
    }
    return result;
}

struct ExactPlaneLess{
    bool operator()(const ExactPlane& left, const ExactPlane& right) const{
        using namespace geometry3d_adaptive_detail;
        if(left.pivot != right.pivot) return left.pivot < right.pivot;
        for(std::size_t index = 0; index < left.coefficients.size(); ++index){
            const ExactDyadic difference = subtract(
                multiply(
                    left.coefficients[index],
                    right.coefficients[right.pivot]
                ),
                multiply(
                    right.coefficients[index],
                    left.coefficients[left.pivot]
                )
            );
            const int comparison = sign(difference);
            if(comparison != 0) return comparison < 0;
        }
        return false;
    }
};

struct FacetPoints{
    std::array<std::size_t, 3> plane{};
    std::vector<std::size_t> points;
};

inline bool outward_boundary(
    const std::vector<std::size_t>& boundary,
    const std::array<std::size_t, 4>& witnesses,
    const std::vector<Point3>& points
){
    for(std::size_t witness: witnesses){
        const int side = adaptive_orient3d(
            points[boundary[0]], points[boundary[1]], points[boundary[2]],
            points[witness]
        );
        if(side != 0) return side < 0;
    }
    throw std::logic_error(
        "divide-and-conquer hull facet has no orientation witness"
    );
}

inline ConvexPolyhedron3 spatial_hull(
    const std::vector<Point3>& points,
    const std::array<std::size_t, 4>& witnesses
){
    std::vector<std::array<std::size_t, 3>> candidates =
        DeterministicLowerHull(points, 1).faces();
    std::vector<std::array<std::size_t, 3>> upper =
        DeterministicLowerHull(points, -1).faces();
    candidates.insert(candidates.end(), upper.begin(), upper.end());

    std::map<ExactPlane, FacetPoints, ExactPlaneLess> facets;
    for(const std::array<std::size_t, 3>& triangle: candidates){
        if(!non_collinear(
            points[triangle[0]], points[triangle[1]], points[triangle[2]]
        )) continue;
        const auto [iterator, inserted] = facets.try_emplace(
            exact_plane(triangle, points), FacetPoints{triangle, {}}
        );
        static_cast<void>(inserted);
        iterator->second.points.insert(
            iterator->second.points.end(), triangle.begin(), triangle.end()
        );
    }
    if(facets.empty())[[unlikely]]{
        throw std::logic_error(
            "divide-and-conquer hull produced no spatial facets"
        );
    }

    std::vector<std::array<std::size_t, 3>> point_faces;
    for(auto& [plane, facet]: facets){
        static_cast<void>(plane);
        std::sort(facet.points.begin(), facet.points.end());
        facet.points.erase(std::unique(
            facet.points.begin(), facet.points.end()
        ), facet.points.end());
        std::vector<std::size_t> boundary = planar_boundary(
            points, facet.points, facet.plane
        );
        if(boundary.size() < 3)[[unlikely]]{
            throw std::logic_error(
                "divide-and-conquer hull facet has no planar boundary"
            );
        }
        if(!outward_boundary(boundary, witnesses, points)){
            std::reverse(boundary.begin() + 1, boundary.end());
        }
        for(std::size_t index = 1; index + 1 < boundary.size(); ++index){
            point_faces.push_back({
                boundary[0], boundary[index], boundary[index + 1]
            });
        }
    }

    std::set<std::size_t> used;
    for(const auto& face: point_faces) used.insert(face.begin(), face.end());
    std::vector<std::size_t> remap(points.size(), points.size());
    std::vector<Point3> vertices;
    vertices.reserve(used.size());
    for(std::size_t point: used){
        remap[point] = vertices.size();
        vertices.push_back(points[point]);
    }
    std::vector<std::array<std::size_t, 3>> result_faces;
    result_faces.reserve(point_faces.size());
    for(const auto& face: point_faces){
        result_faces.push_back({
            remap[face[0]], remap[face[1]], remap[face[2]]
        });
    }
    return {3, std::move(vertices), std::move(result_faces)};
}

}  // namespace divide_and_conquer_convex_hull_3d_detail

inline ConvexPolyhedron3 divide_and_conquer_convex_hull_3d(
    std::vector<Point3> points
){
    using namespace divide_and_conquer_convex_hull_3d_detail;
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "divide_and_conquer_convex_hull_3d requires finite points"
            );
        }
    }
    std::sort(points.begin(), points.end());
    points.erase(std::unique(
        points.begin(), points.end(), same_point
    ), points.end());
    if(points.size() <= 1){
        return {points.empty() ? -1 : 0, std::move(points), {}};
    }
    if(points.size() > (std::numeric_limits<std::size_t>::max() - 2) / 3){
        throw std::length_error(
            "divide_and_conquer_convex_hull_3d has too many points"
        );
    }

    std::size_t third = points.size();
    for(std::size_t index = 2; index < points.size(); ++index){
        if(non_collinear(points[0], points[1], points[index])){
            third = index;
            break;
        }
    }
    if(third == points.size()) return lower_dimensional_hull(points);

    std::size_t fourth = points.size();
    for(std::size_t index = 0; index < points.size(); ++index){
        if(adaptive_orient3d(
            points[0], points[1], points[third], points[index]
        ) != 0){
            fourth = index;
            break;
        }
    }
    if(fourth == points.size()) return lower_dimensional_hull(points);

    return spatial_hull(points, {0, 1, third, fourth});
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_DIVIDE_AND_CONQUER_CONVEX_HULL_3D_HPP_INCLUDED
