// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/geometry_3d.hpp"

bool close_value(long double left, long double right, long double eps = 1e-9L){
    return std::abs(left - right) <= eps * std::max({1.0L, std::abs(left), std::abs(right)});
}

bool close_point(const Point3& left, const Point3& right, long double eps = 1e-9L){
    return close_value(left.x, right.x, eps) &&
        close_value(left.y, right.y, eps) &&
        close_value(left.z, right.z, eps);
}

long double independent_segment_distance(
    const Segment3& first,
    const Segment3& second
){
    long double answer = std::min({
        abs(first.a - closest_point(second, first.a)),
        abs(first.b - closest_point(second, first.b)),
        abs(second.a - closest_point(first, second.a)),
        abs(second.b - closest_point(first, second.b)),
    });
    const Point3 first_direction = first.b - first.a;
    const Point3 second_direction = second.b - second.a;
    if(geometry3d_sign(abs(first_direction)) == 0 ||
        geometry3d_sign(abs(second_direction)) == 0){
        return answer;
    }
    const auto [left, right] = closest_points(
        Line3{first.a, first.b}, Line3{second.a, second.b}
    );
    if(on_segment(first, left) && on_segment(second, right)){
        answer = std::min(answer, abs(left - right));
    }
    return answer;
}

void self_test(){
    assert(close_value(dot({1, 2, 3}, {4, -5, 6}), 12));
    assert(close_point(cross({1, 0, 0}, {0, 1, 0}), {0, 0, 1}));
    assert(close_value(scalar_triple({1, 0, 0}, {0, 1, 0}, {0, 0, 1}), 1));
    assert(collinear({1, 1, 1}, {2, 3, 4}, {3, 5, 7}));
    assert(coplanar({0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {2, 3, 0}));
    assert(orthogonal(Point3{1, 2, 0}, Point3{2, -1, 3}));

    const Line3 x_axis{{0, 0, 0}, {2, 0, 0}};
    assert(close_point(projection(x_axis, {1, 3, 4}), {1, 0, 0}));
    assert(close_point(reflection(x_axis, {1, 3, 4}), {1, -3, -4}));
    assert(close_value(distance(x_axis, Point3{1, 3, 4}), 5));
    assert(close_point(closest_point(Ray3{{0, 0, 0}, {1, 0, 0}}, {-2, 3, 0}), {0, 0, 0}));
    assert(close_point(closest_point(Segment3{{0, 0, 0}, {2, 0, 0}}, {3, 4, 0}), {2, 0, 0}));

    const Plane3 xy{{0, 0, 0}, {0, 0, 2}};
    assert(on_plane(xy, {2, 7, 0}));
    assert(close_point(projection(xy, {1, 2, 3}), {1, 2, 0}));
    assert(close_point(reflection(xy, {1, 2, 3}), {1, 2, -3}));
    assert(close_value(signed_distance(xy, {1, 2, -3}), -3));
    assert(close_point(line_plane_intersection(
        {{1, 2, -4}, {1, 2, 6}}, xy
    ), {1, 2, 0}));
    assert(!ray_plane_intersection({{0, 0, 1}, {0, 0, 2}}, xy));
    assert(segment_plane_intersection({{0, 0, -1}, {0, 0, 1}}, xy));

    const Line3 planes = plane_plane_intersection(
        {{0, 0, 0}, {1, 0, 0}}, {{0, 0, 0}, {0, 1, 0}}
    );
    assert(on_line(planes, {0, 0, 7}));
    const Line3 shifted_planes = plane_plane_intersection(
        {{1, 0, 0}, {1, 0, 0}}, {{0, 2, 0}, {0, 1, 0}}
    );
    assert(on_line(shifted_planes, {1, 2, -7}));

    const auto sphere_points = line_sphere_intersections(
        {{-3, 0, 0}, {3, 0, 0}}, {{0, 0, 0}, 2}
    );
    assert(sphere_points.size() == 2);
    assert(close_value(abs(sphere_points[0]), 2));
    assert(close_value(abs(sphere_points[1]), 2));
    assert(ray_sphere_intersections(
        {{0, 0, 0}, {1, 0, 0}}, {{0, 0, 0}, 2}
    ).size() == 1);
    assert(segment_sphere_intersections(
        {{-3, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, 2}
    ).size() == 1);
    const auto plane_circle = plane_sphere_intersection(
        {{0, 0, 1}, {0, 0, 1}}, {{0, 0, 0}, 2}
    );
    assert(plane_circle && close_value(plane_circle->radius, std::sqrt(3.0L)));
    const auto sphere_circle = sphere_sphere_intersection(
        {{0, 0, 0}, 2}, {{2, 0, 0}, 2}
    );
    assert(sphere_circle && close_point(sphere_circle->center, {1, 0, 0}));
    assert(close_value(sphere_circle->radius, std::sqrt(3.0L)));

    const Triangle3 triangle{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}};
    assert(close_value(area(triangle), 2));
    assert(close_point(centroid(triangle), {2.0L / 3, 2.0L / 3, 0}));
    assert(contains(triangle, {0.5L, 0.5L, 0}));
    assert(!contains(triangle, {0.5L, 0.5L, 1}));
    assert(close_point(closest_point(triangle, {0.5L, 0.5L, 3}), {0.5L, 0.5L, 0}));
    assert(close_point(closest_point(triangle, {2, 2, 0}), {1, 1, 0}));
    const Circle3 triangle_circle = circumcircle(triangle);
    assert(close_point(triangle_circle.center, {1, 1, 0}));
    assert(close_value(triangle_circle.radius, std::sqrt(2.0L)));

    const Tetrahedron3 tetrahedron{
        {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
    };
    assert(close_value(signed_volume(tetrahedron), 1.0L / 6));
    assert(close_point(centroid(tetrahedron), {0.25L, 0.25L, 0.25L}));
    assert(contains(tetrahedron, {0.1L, 0.2L, 0.3L}));
    assert(!contains(tetrahedron, {1, 1, 1}));
    const Sphere3 tetrahedron_sphere = circumsphere(tetrahedron);
    assert(close_point(tetrahedron_sphere.center, {0.5L, 0.5L, 0.5L}));
    assert(close_value(tetrahedron_sphere.radius, std::sqrt(3.0L) / 2));

    const Sphere3 sphere{{0, 0, 0}, 3};
    assert(contains(sphere, {1, 1, 1}) == 1);
    assert(contains(sphere, {3, 0, 0}) == 0);
    assert(contains(sphere, {4, 0, 0}) == -1);
    assert(close_value(sphere_surface_area(sphere), 36 * GEOMETRY3D_PI));
    assert(close_value(sphere_volume(sphere), 36 * GEOMETRY3D_PI));
    assert(close_value(sphere_cap_surface_area(sphere, 1), 6 * GEOMETRY3D_PI));
    assert(close_value(sphere_cap_volume(sphere, 1), 8 * GEOMETRY3D_PI / 3));
    assert(close_value(sphere_intersection_volume(sphere, sphere), sphere_volume(sphere)));
    assert(close_value(sphere_intersection_volume(
        {{0, 0, 0}, 1}, {{2, 0, 0}, 1}
    ), 0));

    assert(close_value(angle({1, 0, 0}, {0, 1, 0}), GEOMETRY3D_PI / 2));
    assert(close_point(rotate_around_axis(
        {1, 0, 0}, {0, 0, 2}, GEOMETRY3D_PI / 2
    ), {0, 1, 0}));
    const auto [basis_x, basis_y] = plane_orthonormal_basis(xy);
    assert(close_value(abs(basis_x), 1));
    assert(close_value(abs(basis_y), 1));
    assert(orthogonal(basis_x, basis_y));
    assert(orthogonal(basis_x, xy.normal));

    std::mt19937_64 random(2026071316);
    auto coordinate = [&](){
        return static_cast<long double>(static_cast<long long>(random() % 2001) - 1000);
    };
    for(int iteration = 0; iteration < 10000; ++iteration){
        Segment3 first{{coordinate(), coordinate(), coordinate()},
            {coordinate(), coordinate(), coordinate()}};
        Segment3 second{{coordinate(), coordinate(), coordinate()},
            {coordinate(), coordinate(), coordinate()}};
        const long double actual = distance(first, second);
        const long double expected = independent_segment_distance(first, second);
        assert(close_value(actual, expected, 1e-8L));
        const auto [left, right] = closest_points(first, second);
        assert(on_segment(first, left));
        assert(on_segment(second, right));
        assert(close_value(actual, abs(left - right), 1e-8L));
    }

    bool thrown = false;
    try{
        (void)unit({0, 0, 0});
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

long long rounded(long double value){ return std::llround(value); }

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }
    for(int query = 0; query < query_count; ++query){
        int type;
        std::cin >> type;
        if(type == 0){
            Point3 left;
            Point3 right;
            std::cin >> left.x >> left.y >> left.z >> right.x >> right.y >> right.z;
            const Point3 result = cross(left, right);
            std::cout << rounded(result.x) << ' ' << rounded(result.y) << ' '
                << rounded(result.z) << '\n';
        }else if(type == 1){
            Tetrahedron3 tetrahedron;
            std::cin >> tetrahedron.a.x >> tetrahedron.a.y >> tetrahedron.a.z
                >> tetrahedron.b.x >> tetrahedron.b.y >> tetrahedron.b.z
                >> tetrahedron.c.x >> tetrahedron.c.y >> tetrahedron.c.z
                >> tetrahedron.d.x >> tetrahedron.d.y >> tetrahedron.d.z;
            std::cout << rounded(6 * signed_volume(tetrahedron)) << '\n';
        }else if(type == 2){
            Segment3 segment;
            Point3 point;
            std::cin >> segment.a.x >> segment.a.y >> segment.a.z
                >> segment.b.x >> segment.b.y >> segment.b.z
                >> point.x >> point.y >> point.z;
            const Point3 result = closest_point(segment, point);
            std::cout << rounded(norm(point - result)) << '\n';
        }else if(type == 3){
            Sphere3 sphere;
            Point3 point;
            std::cin >> sphere.center.x >> sphere.center.y >> sphere.center.z
                >> sphere.radius >> point.x >> point.y >> point.z;
            std::cout << contains(sphere, point) << '\n';
        }else{
            Line3 line;
            Plane3 plane;
            std::cin >> line.a.x >> line.a.y >> line.a.z
                >> line.b.x >> line.b.y >> line.b.z
                >> plane.point.x >> plane.point.y >> plane.point.z
                >> plane.normal.x >> plane.normal.y >> plane.normal.z;
            const Point3 result = line_plane_intersection(line, plane);
            std::cout << rounded(result.x) << ' ' << rounded(result.y) << ' '
                << rounded(result.z) << '\n';
        }
    }
}
