// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>

#include "../../src/algorithm/geometry/3d/core/affine_transform3.hpp"

namespace{

bool close_value(
    long double left,
    long double right,
    long double epsilon = 1e-9L
){
    return std::abs(left - right) <= epsilon *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

bool close_point(const Point3& left, const Point3& right, long double epsilon = 1e-9L){
    return close_value(left.x, right.x, epsilon) &&
        close_value(left.y, right.y, epsilon) &&
        close_value(left.z, right.z, epsilon);
}

bool check_inverse(const AffineTransform3& transform, const Point3& point){
    const std::optional<AffineTransform3> inverse = transform.inverse();
    return inverse && close_point(
        (*inverse)(transform(point)), point, 2e-8L
    ) && close_point(
        transform((*inverse)(point)), point, 2e-8L
    );
}

} // namespace

int main(){
    std::uint64_t seed = 0;
    std::size_t rounds = 0;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    const Point3 point{1, 2, 3};
    if(!close_point(AffineTransform3::identity()(point), point)) return 1;
    if(!close_point(
        AffineTransform3::translation({4, -2, 1})(point), {5, 0, 4}
    )) return 1;
    if(!close_point(
        AffineTransform3::uniform_scaling(3)(point), {3, 6, 9}
    )) return 1;
    if(!close_point(
        AffineTransform3::nonuniform_scaling({2, 3, 4})(point), {2, 6, 12}
    )) return 1;

    const AffineTransform3 origin_rotation =
        AffineTransform3::rotation_about_origin({0, 0, 2}, GEOMETRY3D_PI / 2);
    if(!close_point(origin_rotation({1, 0, 0}), {0, 1, 0})) return 1;

    const Line3 axis{{1, 2, 3}, {1, 2, 5}};
    const AffineTransform3 axis_rotation =
        AffineTransform3::rotation_about_axis(axis, GEOMETRY3D_PI / 2);
    if(!close_point(axis_rotation(axis.a), axis.a)) return 1;
    if(!close_point(axis_rotation({2, 2, 3}), {1, 3, 3})) return 1;

    const AffineTransform3 point_rotation =
        AffineTransform3::rotation_about_point({3, 4, 5}, {0, 0, 1}, GEOMETRY3D_PI);
    if(!close_point(point_rotation({3, 4, 5}), {3, 4, 5})) return 1;
    if(!close_point(point_rotation({4, 6, 8}), {2, 2, 8})) return 1;

    const Plane3 plane{{0, 0, 5}, {0, 0, 2}};
    const AffineTransform3 plane_reflection =
        AffineTransform3::reflection_across_plane(plane);
    if(!close_point(plane_reflection({1, 2, 7}), {1, 2, 3})) return 1;
    if(!close_point(
        plane_reflection(plane_reflection(point)), point
    )) return 1;

    const AffineTransform3 half_turn = AffineTransform3::half_turn_about_line(axis);
    if(!close_point(half_turn(axis.a), axis.a)) return 1;
    if(!close_point(half_turn({2, 4, 6}), {0, 0, 6})) return 1;
    if(!close_point(half_turn(half_turn(point)), point)) return 1;

    const AffineTransform3 scaling = AffineTransform3::uniform_scaling(2);
    const AffineTransform3 translation = AffineTransform3::translation({1, 0, 0});
    if(!close_point(
        translation.compose(scaling)(point), translation(scaling(point))
    )) return 1;
    if(!close_point(
        (translation * scaling)(point), translation(scaling(point))
    )) return 1;
    if(AffineTransform3::uniform_scaling(0).inverse()) return 1;

    std::mt19937_64 random(seed);
    std::uniform_real_distribution<long double> coordinate(-20, 20);
    std::uniform_real_distribution<long double> angle(-GEOMETRY3D_PI, GEOMETRY3D_PI);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        Point3 direction{
            coordinate(random), coordinate(random), coordinate(random),
        };
        if(close_value(
            direction.x * direction.x + direction.y * direction.y +
                direction.z * direction.z,
            0
        )) direction = {1, 0, 0};
        const Point3 center{
            coordinate(random), coordinate(random), coordinate(random),
        };
        const Point3 query{
            coordinate(random), coordinate(random), coordinate(random),
        };
        const Line3 random_axis{center, center + direction};
        const AffineTransform3 rotation = AffineTransform3::rotation_about_axis(
            random_axis, angle(random)
        );
        if(!close_point(rotation(center), center, 2e-8L)) return 1;
        if(!close_value(
            (rotation(query) - center).x * (rotation(query) - center).x +
                (rotation(query) - center).y * (rotation(query) - center).y +
                (rotation(query) - center).z * (rotation(query) - center).z,
            (query - center).x * (query - center).x +
                (query - center).y * (query - center).y +
                (query - center).z * (query - center).z,
            2e-8L
        )) return 1;
        if(!check_inverse(rotation, query)) return 1;

        const AffineTransform3 reflection =
            AffineTransform3::reflection_across_plane({center, direction});
        if(!close_point(
            reflection(reflection(query)), query, 2e-8L
        )) return 1;
        if(!check_inverse(reflection, query)) return 1;
    }

    std::cout << "OK\n";
    return 0;
}
