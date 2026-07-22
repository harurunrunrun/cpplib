#ifndef CPPLIB_TEST_STANDALONE_GEOMETRY_3D_PROPERTY_TESTS_HPP_INCLUDED
#define CPPLIB_TEST_STANDALONE_GEOMETRY_3D_PROPERTY_TESTS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <string_view>

#include "../../src/algorithm/geometry/3d/core/geometry_3d.hpp"

inline std::string_view geometry3d_test_stem(std::string_view path){
    const std::size_t slash = path.find_last_of("/\\");
    if(slash != std::string_view::npos) path.remove_prefix(slash + 1);
    constexpr std::string_view suffix = ".test.cpp";
    if(path.ends_with(suffix)) path.remove_suffix(suffix.size());
    return path;
}

inline long double geometry3d_random_scalar(std::mt19937_64& random){
    return static_cast<long double>(static_cast<long long>(random() % 19) - 9);
}

inline long double geometry3d_random_positive(std::mt19937_64& random){
    return static_cast<long double>(random() % 9 + 1);
}

inline Point3 geometry3d_random_point(std::mt19937_64& random){
    return {
        geometry3d_random_scalar(random),
        geometry3d_random_scalar(random),
        geometry3d_random_scalar(random),
    };
}

inline Point3 geometry3d_random_nonzero(std::mt19937_64& random){
    Point3 result;
    do{
        result = geometry3d_random_point(random);
    }while(geometry3d_sign(abs(result)) == 0);
    return result;
}

inline bool geometry3d_property_close(
    long double left,
    long double right,
    long double epsilon = 1e-8L
){
    return std::abs(left - right) <= epsilon *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

inline bool geometry3d_property_close(
    const Point3& left,
    const Point3& right,
    long double epsilon = 1e-8L
){
    return geometry3d_property_close(left.x, right.x, epsilon) &&
        geometry3d_property_close(left.y, right.y, epsilon) &&
        geometry3d_property_close(left.z, right.z, epsilon);
}

template<class Function>
inline bool geometry3d_throws(Function&& function){
    try{
        function();
    }catch(const std::exception&){
        return true;
    }
    return false;
}

inline bool geometry3d_property_suite(
    std::string_view path,
    std::mt19937_64& random,
    std::size_t rounds
){
    const std::string_view name = geometry3d_test_stem(path);

    if(name == "geometry_3d_abs"){
        if(!geometry3d_property_close(abs(Point3{}), 0)) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 value = geometry3d_random_point(random);
            const long double scale = geometry3d_random_scalar(random);
            if(!geometry3d_property_close(abs(value) * abs(value), norm(value)) ||
               !geometry3d_property_close(abs(-value), abs(value)) ||
               !geometry3d_property_close(abs(scale * value), std::abs(scale) * abs(value))){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_angle"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 first = geometry3d_random_nonzero(random);
            Point3 second = geometry3d_random_nonzero(random);
            if(parallel(first, second)) second += Point3{1, 2, 3};
            const long double value = angle(first, second);
            if(!geometry3d_property_close(angle(first, first), 0) ||
               !geometry3d_property_close(angle(first, -first), GEOMETRY3D_PI) ||
               !geometry3d_property_close(value, angle(second, first)) ||
               value < 0 || value > GEOMETRY3D_PI){
                return false;
            }
        }
        return geometry3d_throws([]{ (void)angle(Point3{}, Point3{1, 0, 0}); });
    }
    if(name == "geometry_3d_area"){
        if(!geometry3d_property_close(area({{0, 0, 0}, {1, 1, 1}, {2, 2, 2}}), 0)) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 a = geometry3d_random_point(random);
            const Point3 b = a + geometry3d_random_nonzero(random);
            const Point3 c = a + geometry3d_random_nonzero(random);
            const Point3 shift = geometry3d_random_point(random);
            const long double scale = geometry3d_random_positive(random);
            const long double value = area({a, b, c});
            if(!geometry3d_property_close(value, area({b, c, a})) ||
               !geometry3d_property_close(value, area({a + shift, b + shift, c + shift})) ||
               !geometry3d_property_close(scale * scale * value,
                   area({scale * a, scale * b, scale * c}))){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_base"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 first = geometry3d_random_point(random);
            const Point3 second = geometry3d_random_point(random);
            const long double scale = geometry3d_random_scalar(random);
            if(!geometry3d_property_close((first + second) - second, first) ||
               !geometry3d_property_close(scale * (first + second),
                   scale * first + scale * second) ||
               !geometry3d_property_close(-(-first), first)){
                return false;
            }
            Point3 assigned = first;
            assigned += second;
            assigned -= second;
            if(!geometry3d_property_close(assigned, first)) return false;
        }
        return Point3{} == Point3{0, 0, 0} && !(Point3{0, 0, 0} < Point3{0, 0, 0});
    }
    if(name == "geometry_3d_cross"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 first = geometry3d_random_point(random);
            const Point3 second = geometry3d_random_point(random);
            const Point3 value = cross(first, second);
            if(!geometry3d_property_close(value, -cross(second, first)) ||
               !geometry3d_property_close(dot(value, first), 0) ||
               !geometry3d_property_close(dot(value, second), 0) ||
               !geometry3d_property_close(norm(value),
                   norm(first) * norm(second) - dot(first, second) * dot(first, second))){
                return false;
            }
        }
        return geometry3d_property_close(cross({1, 2, 3}, {2, 4, 6}), Point3{});
    }
    if(name == "geometry_3d_dot"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 a = geometry3d_random_point(random);
            const Point3 b = geometry3d_random_point(random);
            const Point3 c = geometry3d_random_point(random);
            if(!geometry3d_property_close(dot(a, b), dot(b, a)) ||
               !geometry3d_property_close(dot(a, b + c), dot(a, b) + dot(a, c)) ||
               !geometry3d_property_close(dot(a, a), norm(a))){
                return false;
            }
        }
        return geometry3d_property_close(dot(Point3{}, geometry3d_random_point(random)), 0);
    }
    if(name == "geometry_3d_norm"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 value = geometry3d_random_point(random);
            const long double scale = geometry3d_random_scalar(random);
            if(norm(value) < 0 || !geometry3d_property_close(norm(value), dot(value, value)) ||
               !geometry3d_property_close(norm(-value), norm(value)) ||
               !geometry3d_property_close(norm(scale * value), scale * scale * norm(value))){
                return false;
            }
        }
        return geometry3d_property_close(norm(Point3{}), 0);
    }
    if(name == "geometry_3d_scalar_triple"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 a = geometry3d_random_point(random);
            const Point3 b = geometry3d_random_point(random);
            const Point3 c = geometry3d_random_point(random);
            const long double value = scalar_triple(a, b, c);
            if(!geometry3d_property_close(value, dot(a, cross(b, c))) ||
               !geometry3d_property_close(value, scalar_triple(b, c, a)) ||
               !geometry3d_property_close(value, -scalar_triple(b, a, c)) ||
               !geometry3d_property_close(scalar_triple(a, b, a + b), 0)){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_geometry3d_sign"){
        if(geometry3d_sign(0) != 0 || geometry3d_sign(GEOMETRY3D_EPS) != 0 ||
           geometry3d_sign(-GEOMETRY3D_EPS) != 0 ||
           geometry3d_sign(2 * GEOMETRY3D_EPS) != 1 ||
           geometry3d_sign(-2 * GEOMETRY3D_EPS) != -1){
            return false;
        }
        for(std::size_t i = 0; i < rounds; ++i){
            long double value = geometry3d_random_nonzero(random).x;
            if(value == 0) value = 1;
            if(geometry3d_sign(value) != -geometry3d_sign(-value)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_parallel"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 value = geometry3d_random_nonzero(random);
            const long double scale = geometry3d_random_positive(random);
            if(!parallel(value, scale * value) || !parallel(-value, value) ||
               parallel(value, cross(value, geometry3d_random_nonzero(random)))){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_orthogonal"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 first = geometry3d_random_nonzero(random);
            Point3 helper = geometry3d_random_nonzero(random);
            if(parallel(first, helper)) helper += Point3{1, 2, 4};
            const Point3 second = cross(first, helper);
            if(!orthogonal(first, second) || !orthogonal(second, first) ||
               !orthogonal(-first, second) ||
               orthogonal(first, first)){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_unit"){
        if(!geometry3d_throws([]{ (void)unit(Point3{}); })) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 value = geometry3d_random_nonzero(random);
            const long double scale = geometry3d_random_positive(random);
            if(!geometry3d_property_close(abs(unit(value)), 1) ||
               !geometry3d_property_close(unit(scale * value), unit(value)) ||
               !geometry3d_property_close(unit(-value), -unit(value))){
                return false;
            }
        }
        return true;
    }

    if(name == "geometry_3d_barycentric_coordinates"){
        const Triangle3 triangle{{0, 0, 0}, {2, 0, 0}, {0, 3, 0}};
        const Tetrahedron3 tetrahedron{{0, 0, 0}, {2, 0, 0}, {0, 3, 0}, {0, 0, 4}};
        if(!geometry3d_throws([&]{
            (void)barycentric_coordinates(Triangle3{{}, {1, 1, 1}, {2, 2, 2}}, Point3{});
        }) || !geometry3d_throws([&]{
            (void)barycentric_coordinates(Tetrahedron3{{}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}, Point3{});
        })) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            long double u = static_cast<long double>(random() % 1001) / 1000;
            long double v = static_cast<long double>(random() % 1001) / 1000;
            if(u + v > 1){ u = 1 - u; v = 1 - v; }
            const Point3 point = (1 - u - v) * triangle.a + u * triangle.b + v * triangle.c;
            const auto weights = barycentric_coordinates(triangle, point);
            const Point3 rebuilt = weights[0] * triangle.a + weights[1] * triangle.b + weights[2] * triangle.c;
            const Point3 shift = geometry3d_random_point(random);
            const auto shifted = barycentric_coordinates(
                Triangle3{triangle.a + shift, triangle.b + shift, triangle.c + shift}, point + shift
            );
            if(!geometry3d_property_close(weights[0] + weights[1] + weights[2], 1) ||
               !geometry3d_property_close(rebuilt, point) ||
               !geometry3d_property_close(weights[0], shifted[0]) ||
               !geometry3d_property_close(weights[1], shifted[1]) ||
               !geometry3d_property_close(weights[2], shifted[2])) return false;

            std::array<long double, 4> tw{
                static_cast<long double>(random() % 100 + 1),
                static_cast<long double>(random() % 100 + 1),
                static_cast<long double>(random() % 100 + 1),
                static_cast<long double>(random() % 100 + 1),
            };
            const long double sum = tw[0] + tw[1] + tw[2] + tw[3];
            for(long double& weight: tw) weight /= sum;
            const Point3 tetra_point = tw[0] * tetrahedron.a + tw[1] * tetrahedron.b +
                tw[2] * tetrahedron.c + tw[3] * tetrahedron.d;
            const auto actual = barycentric_coordinates(tetrahedron, tetra_point);
            for(std::size_t j = 0; j < 4; ++j){
                if(!geometry3d_property_close(actual[j], tw[j])) return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_centroid"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Triangle3 tri{
                geometry3d_random_point(random), geometry3d_random_point(random),
                geometry3d_random_point(random)
            };
            const Tetrahedron3 tet{
                geometry3d_random_point(random), geometry3d_random_point(random),
                geometry3d_random_point(random), geometry3d_random_point(random)
            };
            const Point3 shift = geometry3d_random_point(random);
            if(!geometry3d_property_close(centroid(tri), (tri.a + tri.b + tri.c) / 3) ||
               !geometry3d_property_close(centroid(Triangle3{tri.b, tri.c, tri.a}), centroid(tri)) ||
               !geometry3d_property_close(
                    centroid(Triangle3{tri.a + shift, tri.b + shift, tri.c + shift}), centroid(tri) + shift
               ) ||
               !geometry3d_property_close(centroid(tet), (tet.a + tet.b + tet.c + tet.d) / 4)){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_collinear"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 origin = geometry3d_random_point(random);
            const Point3 direction = geometry3d_random_nonzero(random);
            const Point3 first = origin + geometry3d_random_scalar(random) * direction;
            const Point3 second = origin + geometry3d_random_scalar(random) * direction;
            const Point3 third = origin + geometry3d_random_scalar(random) * direction;
            const Point3 shift = geometry3d_random_point(random);
            Point3 perpendicular = cross(direction, {1, 0, 0});
            if(geometry3d_sign(abs(perpendicular)) == 0) perpendicular = cross(direction, {0, 1, 0});
            if(!collinear(first, second, third) || !collinear(third, first, second) ||
               !collinear(first + shift, second + shift, third + shift) ||
               collinear(origin, origin + direction, origin + perpendicular)){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_coplanar"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 origin = geometry3d_random_point(random);
            const Point3 first = geometry3d_random_nonzero(random);
            Point3 second = geometry3d_random_nonzero(random);
            Point3 normal = cross(first, second);
            if(geometry3d_sign(abs(normal)) == 0){ second = cross(first, {1, 2, 3}); normal = cross(first, second); }
            if(geometry3d_sign(abs(normal)) == 0){ second = cross(first, {2, -1, 4}); normal = cross(first, second); }
            const Point3 a = origin;
            const Point3 b = origin + first;
            const Point3 c = origin + second;
            const Point3 d = origin + geometry3d_random_scalar(random) * first +
                geometry3d_random_scalar(random) * second;
            const Point3 shift = geometry3d_random_point(random);
            if(!coplanar(a, b, c, d) || !coplanar(d, c, b, a) ||
               !coplanar(a + shift, b + shift, c + shift, d + shift) ||
               coplanar(a, b, c, d + normal)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_contains"){
        const Triangle3 tri{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}};
        const Tetrahedron3 tet{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        if(!contains(tri, tri.a) || !contains(tri, (tri.a + tri.b) / 2) ||
           contains(tri, {0.25L, 0.25L, 0.1L}) || !contains(tet, tet.a) ||
           contains(tet, {1, 1, 1})) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 center = geometry3d_random_point(random);
            const long double radius = geometry3d_random_positive(random);
            const Point3 direction = unit(geometry3d_random_nonzero(random));
            if(contains(Sphere3{center, radius}, center) != 1 ||
               contains(Sphere3{center, radius}, center + radius * direction) != 0 ||
               contains(Sphere3{center, radius}, center + 2 * radius * direction) != -1){
                return false;
            }
        }
        return geometry3d_throws([]{ (void)contains(Sphere3{{}, -1}, Point3{}); });
    }
    if(name == "geometry_3d_line3_direction"){
        if(!geometry3d_throws([]{ (void)line3_direction(Line3{{1, 2, 3}, {1, 2, 3}}); })) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 a = geometry3d_random_point(random);
            const Point3 direction = geometry3d_random_nonzero(random);
            const Point3 shift = geometry3d_random_point(random);
            if(!geometry3d_property_close(line3_direction({a, a + direction}), direction) ||
               !geometry3d_property_close(line3_direction({a + direction, a}), -direction) ||
               !geometry3d_property_close(line3_direction({a + shift, a + direction + shift}), direction)){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_ray3_direction"){
        if(!geometry3d_throws([]{ (void)ray3_direction(Ray3{{1, 2, 3}, {1, 2, 3}}); })) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 origin = geometry3d_random_point(random);
            const Point3 direction = geometry3d_random_nonzero(random);
            const Point3 shift = geometry3d_random_point(random);
            if(!geometry3d_property_close(ray3_direction({origin, origin + direction}), direction) ||
               !geometry3d_property_close(ray3_direction({origin, origin - direction}), -direction) ||
               !geometry3d_property_close(ray3_direction({origin + shift, origin + direction + shift}), direction)){
                return false;
            }
        }
        return true;
    }
    if(name == "geometry_3d_on_line"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 origin = geometry3d_random_point(random);
            const Point3 direction = geometry3d_random_nonzero(random);
            const Point3 shift = geometry3d_random_point(random);
            Point3 perpendicular = cross(direction, {1, 0, 0});
            if(geometry3d_sign(abs(perpendicular)) == 0) perpendicular = cross(direction, {0, 1, 0});
            const Line3 line{origin, origin + direction};
            const Point3 point = origin + geometry3d_random_scalar(random) * direction;
            if(!on_line(line, point) || !on_line({line.b, line.a}, point) ||
               !on_line({line.a + shift, line.b + shift}, point + shift) ||
               on_line(line, point + perpendicular)) return false;
        }
        return geometry3d_throws([]{ (void)on_line(Line3{{}, {}}, Point3{}); });
    }
    if(name == "geometry_3d_on_ray"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 origin = geometry3d_random_point(random);
            const Point3 direction = geometry3d_random_nonzero(random);
            const long double scale = geometry3d_random_positive(random);
            const Ray3 ray{origin, origin + direction};
            if(!on_ray(ray, origin) || !on_ray(ray, origin + scale * direction) ||
               on_ray(ray, origin - scale * direction) ||
               !on_ray({origin, origin + 2 * direction}, origin + scale * direction)) return false;
        }
        return geometry3d_throws([]{ (void)on_ray(Ray3{{}, {}}, Point3{}); });
    }
    if(name == "geometry_3d_on_segment"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 a = geometry3d_random_point(random);
            const Point3 direction = geometry3d_random_nonzero(random);
            const Point3 b = a + direction;
            const long double ratio = static_cast<long double>(random() % 1001) / 1000;
            const Point3 point = a + ratio * direction;
            if(!on_segment({a, b}, a) || !on_segment({a, b}, b) ||
               !on_segment({a, b}, point) || !on_segment({b, a}, point) ||
               on_segment({a, b}, a + 2 * direction)) return false;
        }
        return on_segment({{1, 2, 3}, {1, 2, 3}}, {1, 2, 3}) &&
            !on_segment({{1, 2, 3}, {1, 2, 3}}, {1, 2, 4});
    }
    if(name == "geometry_3d_on_plane"){
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 origin = geometry3d_random_point(random);
            const Point3 normal = geometry3d_random_nonzero(random);
            Point3 tangent = cross(normal, {1, 0, 0});
            if(geometry3d_sign(abs(tangent)) == 0) tangent = cross(normal, {0, 1, 0});
            const Point3 point = origin + geometry3d_random_scalar(random) * tangent;
            const Point3 shift = geometry3d_random_point(random);
            if(!on_plane({origin, normal}, point) || !on_plane({origin, -normal}, point) ||
               !on_plane({origin + shift, normal}, point + shift) ||
               on_plane({origin, normal}, point + normal)) return false;
        }
        return geometry3d_throws([]{ (void)on_plane(Plane3{{}, {}}, Point3{}); });
    }
    if(name == "geometry_3d_plane3_unit_normal"){
        if(!geometry3d_throws([]{ (void)plane3_unit_normal(Plane3{{}, {}}); })) return false;
        for(std::size_t i = 0; i < rounds; ++i){
            const Point3 normal = geometry3d_random_nonzero(random);
            const Point3 result = plane3_unit_normal({geometry3d_random_point(random), normal});
            if(!geometry3d_property_close(abs(result), 1) || !parallel(result, normal) ||
               !geometry3d_property_close(
                    plane3_unit_normal({{}, geometry3d_random_positive(random) * normal}), result
               ) || !geometry3d_property_close(plane3_unit_normal({{}, -normal}), -result)) return false;
        }
        return true;
    }

    if(name == "geometry_3d_circumcircle"){
        if(!geometry3d_throws([]{ (void)circumcircle({{}, {1, 1, 1}, {2, 2, 2}}); })) return false;
        for(std::size_t i=0;i<rounds;++i){
            const Point3 shift=geometry3d_random_point(random);
            const Triangle3 tri{shift,shift+Point3{2,0,0},shift+Point3{0,3,0}};
            const Circle3 c=circumcircle(tri);
            if(!geometry3d_property_close(abs(tri.a-c.center),c.radius) ||
               !geometry3d_property_close(abs(tri.b-c.center),c.radius) ||
               !geometry3d_property_close(abs(tri.c-c.center),c.radius) ||
               !orthogonal(c.normal,tri.b-tri.a) ||
               !geometry3d_property_close(circumcircle({tri.b,tri.c,tri.a}).center,c.center)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_circumsphere"){
        if(!geometry3d_throws([]{ (void)circumsphere({{}, {1,0,0}, {0,1,0}, {1,1,0}}); })) return false;
        for(std::size_t i=0;i<rounds;++i){
            const Point3 shift=geometry3d_random_point(random);
            const Tetrahedron3 tet{shift,shift+Point3{2,0,0},shift+Point3{0,3,0},shift+Point3{0,0,4}};
            const Sphere3 s=circumsphere(tet);
            if(!geometry3d_property_close(abs(tet.a-s.center),s.radius) ||
               !geometry3d_property_close(abs(tet.b-s.center),s.radius) ||
               !geometry3d_property_close(abs(tet.c-s.center),s.radius) ||
               !geometry3d_property_close(abs(tet.d-s.center),s.radius) ||
               !geometry3d_property_close(circumsphere({tet.b,tet.a,tet.c,tet.d}).center,s.center)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_closest_point"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 a=geometry3d_random_point(random), d=geometry3d_random_nonzero(random);
            const Segment3 seg{a,a+d};
            if(!geometry3d_property_close(closest_point(seg,a-2*d),a) ||
               !geometry3d_property_close(closest_point(seg,a+3*d),a+d) ||
               !geometry3d_property_close(closest_point(seg,a+d/2),a+d/2)) return false;
            const Ray3 ray{a,a+d};
            if(!geometry3d_property_close(closest_point(ray,a-d),a) ||
               !geometry3d_property_close(closest_point(ray,a+2*d),a+2*d)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_closest_points"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 shift=geometry3d_random_point(random);
            const Line3 first{shift,shift+Point3{1,0,0}};
            const Line3 second{shift+Point3{0,2,3},shift+Point3{0,3,3}};
            const auto value=closest_points(first,second);
            const auto reverse=closest_points(second,first);
            if(!on_line(first,value.first) || !on_line(second,value.second) ||
               !geometry3d_property_close(value.first,reverse.second) ||
               !geometry3d_property_close(value.second,reverse.first) ||
               !orthogonal(value.second-value.first,line3_direction(first)) ||
               !orthogonal(value.second-value.first,line3_direction(second))) return false;
            const Segment3 a{shift,shift+Point3{1,0,0}}, b{shift+Point3{3,0,0},shift+Point3{4,0,0}};
            const auto segments=closest_points(a,b);
            if(!geometry3d_property_close(segments.first,a.b) ||
               !geometry3d_property_close(segments.second,b.a)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_distance"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 a=geometry3d_random_point(random), b=geometry3d_random_point(random);
            const Point3 shift=geometry3d_random_point(random);
            if(distance(a,b)<0 || !geometry3d_property_close(distance(a,b),distance(b,a)) ||
               !geometry3d_property_close(distance(a+shift,b+shift),distance(a,b))) return false;
            const Point3 d=geometry3d_random_nonzero(random), p=geometry3d_random_point(random);
            const Line3 line{a,a+d};
            if(!geometry3d_property_close(distance(line,p),abs(p-projection(line,p))) ||
               !geometry3d_property_close(distance(line,projection(line,p)),0)) return false;
            const Plane3 plane{a,d};
            if(!geometry3d_property_close(distance(plane,p),std::abs(signed_distance(plane,p)))) return false;
        }
        return true;
    }
    if(name == "geometry_3d_projection"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 a=geometry3d_random_point(random), d=geometry3d_random_nonzero(random);
            const Point3 p=geometry3d_random_point(random), shift=geometry3d_random_point(random);
            const Line3 line{a,a+d};
            const Point3 q=projection(line,p);
            if(!on_line(line,q) || !orthogonal(p-q,d) ||
               !geometry3d_property_close(projection(line,q),q) ||
               !geometry3d_property_close(projection(Line3{a+shift,a+d+shift},p+shift),q+shift)) return false;
            const Plane3 plane{a,d};
            const Point3 r=projection(plane,p);
            if(!on_plane(plane,r) || !parallel(p-r,d) ||
               !geometry3d_property_close(projection(plane,r),r)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_reflection"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 a=geometry3d_random_point(random), d=geometry3d_random_nonzero(random);
            const Point3 p=geometry3d_random_point(random);
            const Line3 line{a,a+d};
            const Point3 reflected=reflection(line,p), midpoint=(p+reflected)/2;
            if(!geometry3d_property_close(reflection(line,reflected),p) ||
               !geometry3d_property_close(midpoint,projection(line,p)) ||
               !geometry3d_property_close(distance(line,p),distance(line,reflected))) return false;
            const Plane3 plane{a,d};
            const Point3 plane_reflected=reflection(plane,p);
            if(!geometry3d_property_close(reflection(plane,plane_reflected),p) ||
               !geometry3d_property_close((p+plane_reflected)/2,projection(plane,p))) return false;
        }
        return true;
    }
    if(name == "geometry_3d_signed_distance"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 a=geometry3d_random_point(random), n=geometry3d_random_nonzero(random);
            const Point3 p=geometry3d_random_point(random), shift=geometry3d_random_point(random);
            const Plane3 plane{a,n};
            const long double value=signed_distance(plane,p);
            if(!geometry3d_property_close(std::abs(value),distance(plane,p)) ||
               !geometry3d_property_close(signed_distance({a,-n},p),-value) ||
               !geometry3d_property_close(signed_distance({a+shift,n},p+shift),value) ||
               !geometry3d_property_close(signed_distance(plane,projection(plane,p)),0)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_plane_orthonormal_basis"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 normal=geometry3d_random_nonzero(random);
            const auto [first,second]=plane_orthonormal_basis({geometry3d_random_point(random),normal});
            if(!geometry3d_property_close(abs(first),1) || !geometry3d_property_close(abs(second),1) ||
               !orthogonal(first,second) || !orthogonal(first,normal) || !orthogonal(second,normal) ||
               !parallel(cross(first,second),normal)) return false;
        }
        return geometry3d_throws([]{ (void)plane_orthonormal_basis(Plane3{{}, {}}); });
    }
    if(name == "geometry_3d_line_plane_intersection"){
        if(!geometry3d_throws([]{ (void)line_plane_intersection({{0,0,1},{1,0,1}},{{}, {0,0,1}}); })) return false;
        for(std::size_t i=0;i<rounds;++i){
            const Point3 hit=geometry3d_random_point(random), n=geometry3d_random_nonzero(random);
            const Line3 line{hit-n,hit+2*n}; const Plane3 plane{hit,n};
            const Point3 value=line_plane_intersection(line,plane);
            if(!on_line(line,value) || !on_plane(plane,value) ||
               !geometry3d_property_close(line_plane_intersection({line.b,line.a},plane),value)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_ray_plane_intersection"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 hit=geometry3d_random_point(random), n=geometry3d_random_nonzero(random);
            const Plane3 plane{hit,n};
            const Ray3 toward{hit-n,hit+n}, away{hit+n,hit+2*n};
            const auto value=ray_plane_intersection(toward,plane);
            if(!value || !on_plane(plane,*value) || !on_ray(toward,*value) ||
               ray_plane_intersection(away,plane)) return false;
            const auto origin_hit=ray_plane_intersection({hit,hit+n},plane);
            if(!origin_hit || !geometry3d_property_close(*origin_hit,hit)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_segment_plane_intersection"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 hit=geometry3d_random_point(random), n=geometry3d_random_nonzero(random);
            const Plane3 plane{hit,n}; const Segment3 segment{hit-n,hit+2*n};
            const auto value=segment_plane_intersection(segment,plane);
            const auto reverse=segment_plane_intersection({segment.b,segment.a},plane);
            if(!value || !reverse || !on_plane(plane,*value) || !on_segment(segment,*value) ||
               !geometry3d_property_close(*value,*reverse) ||
               segment_plane_intersection({hit+n,hit+2*n},plane)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_plane_plane_intersection"){
        if(!geometry3d_throws([]{ (void)plane_plane_intersection({{}, {1,0,0}},{{0,1,0},{2,0,0}}); })) return false;
        for(std::size_t i=0;i<rounds;++i){
            const Point3 hit=geometry3d_random_point(random);
            const Plane3 first{hit,{1,0,0}}, second{hit,{0,1,0}};
            const Line3 line=plane_plane_intersection(first,second);
            const Line3 reverse=plane_plane_intersection(second,first);
            if(!on_plane(first,line.a) || !on_plane(first,line.b) ||
               !on_plane(second,line.a) || !on_plane(second,line.b) ||
               !on_line(reverse,line.a) || !parallel(line3_direction(line),cross(first.normal,second.normal))) return false;
        }
        return true;
    }
    if(name == "geometry_3d_line_sphere_intersections"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 center=geometry3d_random_point(random); const long double r=geometry3d_random_positive(random);
            const Sphere3 sphere{center,r};
            const auto two=line_sphere_intersections({center-2*r*Point3{1,0,0},center+2*r*Point3{1,0,0}},sphere);
            const auto tangent=line_sphere_intersections({center+Point3{-2*r,r,0},center+Point3{2*r,r,0}},sphere);
            const auto none=line_sphere_intersections({center+Point3{-2*r,2*r,0},center+Point3{2*r,2*r,0}},sphere);
            if(two.size()!=2 || tangent.size()!=1 || !none.empty()) return false;
            for(const Point3& p:two) if(!geometry3d_property_close(abs(p-center),r)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_ray_sphere_intersections"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 center=geometry3d_random_point(random); const long double r=geometry3d_random_positive(random);
            const Sphere3 sphere{center,r};
            const Ray3 ray{center-2*r*Point3{1,0,0},center-1*r*Point3{1,0,0}};
            const auto two=ray_sphere_intersections(ray,sphere);
            const auto inside=ray_sphere_intersections({center,center+Point3{1,0,0}},sphere);
            const auto away=ray_sphere_intersections({center+2*r*Point3{1,0,0},center+3*r*Point3{1,0,0}},sphere);
            if(two.size()!=2 || inside.size()!=1 || !away.empty()) return false;
            for(const Point3& p:two) if(!on_ray(ray,p) || !geometry3d_property_close(abs(p-center),r)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_segment_sphere_intersections"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 center=geometry3d_random_point(random); const long double r=geometry3d_random_positive(random);
            const Sphere3 sphere{center,r}; const Segment3 segment{center-2*r*Point3{1,0,0},center+2*r*Point3{1,0,0}};
            const auto two=segment_sphere_intersections(segment,sphere);
            const auto reverse=segment_sphere_intersections({segment.b,segment.a},sphere);
            const auto none=segment_sphere_intersections({center+2*r*Point3{1,0,0},center+3*r*Point3{1,0,0}},sphere);
            if(two.size()!=2 || reverse.size()!=2 || !none.empty()) return false;
            for(const Point3& p:two) if(!on_segment(segment,p) || !geometry3d_property_close(abs(p-center),r)) return false;
        }
        return true;
    }

    if(name == "geometry_3d_on_circle"){
        if(geometry3d_throws([]{ (void)on_circle(Circle3{{}, {0,0,1}, -1}, Point3{}); }) == false) return false;
        for(std::size_t i=0;i<rounds;++i){
            const Point3 center=geometry3d_random_point(random), normal=geometry3d_random_nonzero(random);
            const long double radius=geometry3d_random_positive(random);
            Point3 tangent=cross(normal,{1,0,0});
            if(geometry3d_sign(abs(tangent))==0) tangent=cross(normal,{0,1,0});
            tangent=unit(tangent);
            const Circle3 circle{center,normal,radius};
            if(!on_circle(circle,center+radius*tangent) ||
               !on_circle({center,2*normal,radius},center+radius*tangent) ||
               on_circle(circle,center) || on_circle(circle,center+radius*tangent+unit(normal))) return false;
        }
        return true;
    }
    if(name == "geometry_3d_plane_sphere_intersection"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 center=geometry3d_random_point(random), normal=geometry3d_random_nonzero(random);
            const long double radius=geometry3d_random_positive(random);
            const Sphere3 sphere{center,radius};
            const auto great=plane_sphere_intersection({center,normal},sphere);
            const auto tangent=plane_sphere_intersection({center+radius*unit(normal),normal},sphere);
            const auto none=plane_sphere_intersection({center+2*radius*unit(normal),normal},sphere);
            if(!great || !tangent || none || !geometry3d_property_close(great->center,center) ||
               !geometry3d_property_close(great->radius,radius) ||
               !geometry3d_property_close(tangent->radius,0) ||
               !on_plane({center,normal},great->center)) return false;
        }
        return geometry3d_throws([]{ (void)plane_sphere_intersection(Plane3{{},{0,0,1}},Sphere3{{},-1}); });
    }
    if(name == "geometry_3d_sphere_sphere_intersection"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 center=geometry3d_random_point(random); const long double radius=geometry3d_random_positive(random);
            const Sphere3 first{center,radius}, second{center+radius*Point3{1,0,0},radius};
            const auto value=sphere_sphere_intersection(first,second);
            const auto reverse=sphere_sphere_intersection(second,first);
            if(!value || !reverse || !geometry3d_property_close(value->center,reverse->center) ||
               !geometry3d_property_close(value->radius,reverse->radius) ||
               !geometry3d_property_close(abs(value->center-first.center)*abs(value->center-first.center)+
                    value->radius*value->radius,radius*radius) ||
               sphere_sphere_intersection(first,{center+3*radius*Point3{1,0,0},radius})) return false;
        }
        return geometry3d_throws([]{ (void)sphere_sphere_intersection(Sphere3{{},1},Sphere3{{},1}); });
    }
    if(name == "geometry_3d_triangle_normal"){
        if(!geometry3d_throws([]{ (void)triangle_normal({{}, {1,1,1}, {2,2,2}}); })) return false;
        for(std::size_t i=0;i<rounds;++i){
            const Point3 a=geometry3d_random_point(random), u=geometry3d_random_nonzero(random);
            Point3 v=geometry3d_random_nonzero(random);
            if(parallel(u,v)) v=cross(u,{1,2,3});
            if(geometry3d_sign(abs(v))==0) v=cross(u,{2,-1,4});
            const Triangle3 tri{a,a+u,a+v}; const Point3 n=triangle_normal(tri);
            if(!geometry3d_property_close(abs(n),1) || !orthogonal(n,u) || !orthogonal(n,v) ||
               !geometry3d_property_close(triangle_normal({tri.a,tri.c,tri.b}),-n)) return false;
        }
        return true;
    }
    if(name == "geometry_3d_signed_volume"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 shift=geometry3d_random_point(random);
            const Tetrahedron3 tet{shift,shift+Point3{1,0,0},shift+Point3{0,2,0},shift+Point3{0,0,3}};
            const long double value=signed_volume(tet);
            if(!geometry3d_property_close(value,1) ||
               !geometry3d_property_close(signed_volume({tet.b,tet.a,tet.c,tet.d}),-value) ||
               !geometry3d_property_close(signed_volume({tet.a+shift,tet.b+shift,tet.c+shift,tet.d+shift}),value) ||
               !geometry3d_property_close(std::abs(value),volume(tet))) return false;
        }
        return true;
    }
    if(name == "geometry_3d_volume"){
        for(std::size_t i=0;i<rounds;++i){
            const Tetrahedron3 tet{{},geometry3d_random_nonzero(random),geometry3d_random_nonzero(random),geometry3d_random_nonzero(random)};
            const long double scale=geometry3d_random_positive(random), value=volume(tet);
            if(value<0 || !geometry3d_property_close(value,std::abs(signed_volume(tet))) ||
               !geometry3d_property_close(volume({tet.b,tet.a,tet.c,tet.d}),value) ||
               !geometry3d_property_close(volume({scale*tet.a,scale*tet.b,scale*tet.c,scale*tet.d}),
                    scale*scale*scale*value)) return false;
        }
        return geometry3d_property_close(volume({{}, {1,0,0}, {0,1,0}, {1,1,0}}),0);
    }
    if(name == "geometry_3d_sphere_surface_area"){
        if(!geometry3d_property_close(sphere_surface_area({{},0}),0)) return false;
        for(std::size_t i=0;i<rounds;++i){
            const long double radius=geometry3d_random_positive(random), scale=geometry3d_random_positive(random);
            if(!geometry3d_property_close(sphere_surface_area({geometry3d_random_point(random),radius}),
                    4*GEOMETRY3D_PI*radius*radius) ||
               !geometry3d_property_close(sphere_surface_area({{},scale*radius}),
                    scale*scale*sphere_surface_area({{},radius}))) return false;
        }
        return geometry3d_throws([]{ (void)sphere_surface_area({{},-1}); });
    }
    if(name == "geometry_3d_sphere_volume"){
        if(!geometry3d_property_close(sphere_volume({{},0}),0)) return false;
        for(std::size_t i=0;i<rounds;++i){
            const long double radius=geometry3d_random_positive(random), scale=geometry3d_random_positive(random);
            if(!geometry3d_property_close(sphere_volume({geometry3d_random_point(random),radius}),
                    4*GEOMETRY3D_PI*radius*radius*radius/3) ||
               !geometry3d_property_close(sphere_volume({{},scale*radius}),
                    scale*scale*scale*sphere_volume({{},radius}))) return false;
        }
        return geometry3d_throws([]{ (void)sphere_volume({{},-1}); });
    }
    if(name == "geometry_3d_sphere_cap_surface_area"){
        for(std::size_t i=0;i<rounds;++i){
            const long double radius=geometry3d_random_positive(random);
            if(!geometry3d_property_close(sphere_cap_surface_area({{},radius},0),0) ||
               !geometry3d_property_close(sphere_cap_surface_area({{},radius},2*radius),
                    sphere_surface_area({{},radius})) ||
               !geometry3d_property_close(sphere_cap_surface_area({{},radius},radius),
                    2*GEOMETRY3D_PI*radius*radius)) return false;
        }
        return geometry3d_throws([]{ (void)sphere_cap_surface_area({{},1},3); });
    }
    if(name == "geometry_3d_sphere_cap_volume"){
        for(std::size_t i=0;i<rounds;++i){
            const long double radius=geometry3d_random_positive(random);
            const long double height=static_cast<long double>(random()%1001)*2*radius/1000;
            if(!geometry3d_property_close(sphere_cap_volume({{},radius},0),0) ||
               !geometry3d_property_close(sphere_cap_volume({{},radius},2*radius),sphere_volume({{},radius})) ||
               !geometry3d_property_close(sphere_cap_volume({{},radius},height)+
                    sphere_cap_volume({{},radius},2*radius-height),sphere_volume({{},radius}))) return false;
        }
        return geometry3d_throws([]{ (void)sphere_cap_volume({{},1},-1); });
    }
    if(name == "geometry_3d_sphere_intersection_volume"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 center=geometry3d_random_point(random); const long double radius=geometry3d_random_positive(random);
            const Sphere3 first{center,radius}, same{center,radius}, small{center,radius/2};
            if(!geometry3d_property_close(sphere_intersection_volume(first,same),sphere_volume(first)) ||
               !geometry3d_property_close(sphere_intersection_volume(first,small),sphere_volume(small)) ||
               !geometry3d_property_close(sphere_intersection_volume(first,{center+3*radius*Point3{1,0,0},radius}),0) ||
               !geometry3d_property_close(sphere_intersection_volume(first,small),
                    sphere_intersection_volume(small,first))) return false;
        }
        return geometry3d_throws([]{ (void)sphere_intersection_volume(Sphere3{{},-1},Sphere3{{},1}); });
    }
    if(name == "geometry_3d_rotate_around_axis"){
        for(std::size_t i=0;i<rounds;++i){
            const Point3 point=geometry3d_random_point(random), axis=geometry3d_random_nonzero(random);
            const long double theta=geometry3d_random_scalar(random);
            const Point3 rotated=rotate_around_axis(point,axis,theta);
            if(!geometry3d_property_close(abs(rotated),abs(point)) ||
               !geometry3d_property_close(rotate_around_axis(rotated,axis,-theta),point) ||
               !geometry3d_property_close(rotate_around_axis(axis,axis,theta),axis) ||
               !geometry3d_property_close(rotate_around_axis(point,axis,0),point)) return false;
        }
        return geometry3d_throws([]{ (void)rotate_around_axis(Point3{1,0,0},Point3{},1); });
    }

    if(name == "geometry_3d_headers") return true;
    return false;
}

#endif  // CPPLIB_TEST_STANDALONE_GEOMETRY_3D_PROPERTY_TESTS_HPP_INCLUDED
