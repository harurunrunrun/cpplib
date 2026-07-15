// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>

#include "../../src/algorithm/geometry/3d/affine_transform3.hpp"
#include "geometry_3d_test_common.hpp"

namespace{

bool check_inverse(const AffineTransform3& transform, const Point3& point){
    const std::optional<AffineTransform3> inverse = transform.inverse();
    return inverse && geometry3d_close_point(
        (*inverse)(transform(point)), point, 2e-8L
    ) && geometry3d_close_point(
        transform((*inverse)(point)), point, 2e-8L
    );
}

} // namespace

int main(){
    std::uint64_t seed = 0;
    std::size_t rounds = 0;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    const Point3 point{1, 2, 3};
    if(!geometry3d_close_point(AffineTransform3::identity()(point), point)) return 1;
    if(!geometry3d_close_point(
        AffineTransform3::translation({4, -2, 1})(point), {5, 0, 4}
    )) return 1;
    if(!geometry3d_close_point(
        AffineTransform3::uniform_scaling(3)(point), {3, 6, 9}
    )) return 1;
    if(!geometry3d_close_point(
        AffineTransform3::nonuniform_scaling({2, 3, 4})(point), {2, 6, 12}
    )) return 1;

    const AffineTransform3 origin_rotation =
        AffineTransform3::rotation_about_origin({0, 0, 2}, GEOMETRY3D_PI / 2);
    if(!geometry3d_close_point(origin_rotation({1, 0, 0}), {0, 1, 0})) return 1;

    const Line3 axis{{1, 2, 3}, {1, 2, 5}};
    const AffineTransform3 axis_rotation =
        AffineTransform3::rotation_about_axis(axis, GEOMETRY3D_PI / 2);
    if(!geometry3d_close_point(axis_rotation(axis.a), axis.a)) return 1;
    if(!geometry3d_close_point(axis_rotation({2, 2, 3}), {1, 3, 3})) return 1;

    const AffineTransform3 point_rotation =
        AffineTransform3::rotation_about_point({3, 4, 5}, {0, 0, 1}, GEOMETRY3D_PI);
    if(!geometry3d_close_point(point_rotation({3, 4, 5}), {3, 4, 5})) return 1;
    if(!geometry3d_close_point(point_rotation({4, 6, 8}), {2, 2, 8})) return 1;

    const Plane3 plane{{0, 0, 5}, {0, 0, 2}};
    const AffineTransform3 plane_reflection =
        AffineTransform3::reflection_across_plane(plane);
    if(!geometry3d_close_point(plane_reflection({1, 2, 7}), {1, 2, 3})) return 1;
    if(!geometry3d_close_point(
        plane_reflection(plane_reflection(point)), point
    )) return 1;

    const AffineTransform3 half_turn = AffineTransform3::half_turn_about_line(axis);
    if(!geometry3d_close_point(half_turn(axis.a), axis.a)) return 1;
    if(!geometry3d_close_point(half_turn({2, 4, 6}), {0, 0, 6})) return 1;
    if(!geometry3d_close_point(half_turn(half_turn(point)), point)) return 1;

    const AffineTransform3 scaling = AffineTransform3::uniform_scaling(2);
    const AffineTransform3 translation = AffineTransform3::translation({1, 0, 0});
    if(!geometry3d_close_point(
        translation.compose(scaling)(point), translation(scaling(point))
    )) return 1;
    if(!geometry3d_close_point(
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
        if(geometry3d_close_value(
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
        if(!geometry3d_close_point(rotation(center), center, 2e-8L)) return 1;
        if(!geometry3d_close_value(
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
        if(!geometry3d_close_point(
            reflection(reflection(query)), query, 2e-8L
        )) return 1;
        if(!check_inverse(reflection, query)) return 1;
    }

    std::cout << "OK\n";
    return 0;
}
