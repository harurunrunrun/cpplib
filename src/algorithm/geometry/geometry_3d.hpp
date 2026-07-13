#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

inline constexpr long double GEOMETRY3D_EPS = 1e-10L;
inline constexpr long double GEOMETRY3D_PI =
    3.141592653589793238462643383279502884L;

inline int geometry3d_sign(long double value){
    if(value > GEOMETRY3D_EPS) return 1;
    if(value < -GEOMETRY3D_EPS) return -1;
    return 0;
}

struct Point3{
    long double x = 0;
    long double y = 0;
    long double z = 0;

    Point3() = default;
    Point3(long double x_, long double y_, long double z_)
        : x(x_), y(y_), z(z_){}

    Point3 operator+() const{ return *this; }
    Point3 operator-() const{ return {-x, -y, -z}; }
    Point3 operator+(const Point3& other) const{
        return {x + other.x, y + other.y, z + other.z};
    }
    Point3 operator-(const Point3& other) const{
        return {x - other.x, y - other.y, z - other.z};
    }
    Point3 operator*(long double scalar) const{
        return {x * scalar, y * scalar, z * scalar};
    }
    Point3 operator/(long double scalar) const{
        return {x / scalar, y / scalar, z / scalar};
    }
    Point3& operator+=(const Point3& other){
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    Point3& operator-=(const Point3& other){
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    Point3& operator*=(long double scalar){
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Point3& operator/=(long double scalar){
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    bool operator==(const Point3& other) const{
        return geometry3d_sign(x - other.x) == 0 &&
            geometry3d_sign(y - other.y) == 0 &&
            geometry3d_sign(z - other.z) == 0;
    }
    bool operator<(const Point3& other) const{
        if(x != other.x) return x < other.x;
        if(y != other.y) return y < other.y;
        return z < other.z;
    }
};

inline Point3 operator*(long double scalar, const Point3& point){
    return point * scalar;
}

struct Line3{
    Point3 a;
    Point3 b;
};

struct Ray3{
    Point3 origin;
    Point3 through;
};

struct Segment3{
    Point3 a;
    Point3 b;
};

struct Plane3{
    Point3 point;
    Point3 normal;
};

struct Circle3{
    Point3 center;
    Point3 normal;
    long double radius;
};

struct Sphere3{
    Point3 center;
    long double radius;
};

struct Triangle3{
    Point3 a;
    Point3 b;
    Point3 c;
};

struct Tetrahedron3{
    Point3 a;
    Point3 b;
    Point3 c;
    Point3 d;
};

inline long double dot(const Point3& left, const Point3& right){
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline Point3 cross(const Point3& left, const Point3& right){
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x,
    };
}

inline long double norm(const Point3& point){ return dot(point, point); }
inline long double abs(const Point3& point){ return std::sqrt(norm(point)); }

inline Point3 unit(const Point3& point){
    const long double length = abs(point);
    if(geometry3d_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("unit vector of zero 3D vector");
    }
    return point / length;
}

inline long double scalar_triple(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return dot(first, cross(second, third));
}

inline bool parallel(const Point3& left, const Point3& right){
    return geometry3d_sign(abs(cross(left, right))) == 0;
}

inline bool orthogonal(const Point3& left, const Point3& right){
    return geometry3d_sign(dot(left, right)) == 0;
}

inline bool collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return parallel(second - first, third - first);
}

inline bool coplanar(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    return geometry3d_sign(scalar_triple(
        second - first, third - first, fourth - first
    )) == 0;
}

inline Point3 line3_direction(const Line3& line){
    const Point3 direction = line.b - line.a;
    if(geometry3d_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D line");
    }
    return direction;
}

inline Point3 ray3_direction(const Ray3& ray){
    const Point3 direction = ray.through - ray.origin;
    if(geometry3d_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D ray");
    }
    return direction;
}

inline Point3 plane3_unit_normal(const Plane3& plane){
    return unit(plane.normal);
}

inline bool on_line(const Line3& line, const Point3& point){
    return parallel(line3_direction(line), point - line.a);
}

inline bool on_ray(const Ray3& ray, const Point3& point){
    const Point3 direction = ray3_direction(ray);
    return parallel(direction, point - ray.origin) &&
        geometry3d_sign(dot(direction, point - ray.origin)) >= 0;
}

inline bool on_segment(const Segment3& segment, const Point3& point){
    const Point3 direction = segment.b - segment.a;
    if(geometry3d_sign(abs(direction)) == 0) return point == segment.a;
    return parallel(direction, point - segment.a) &&
        geometry3d_sign(dot(point - segment.a, point - segment.b)) <= 0;
}

inline bool on_plane(const Plane3& plane, const Point3& point){
    const Point3 normal = plane3_unit_normal(plane);
    return geometry3d_sign(dot(normal, point - plane.point)) == 0;
}

inline Point3 projection(const Line3& line, const Point3& point){
    const Point3 direction = line3_direction(line);
    return line.a + direction * (dot(point - line.a, direction) / norm(direction));
}

inline Point3 closest_point(const Ray3& ray, const Point3& point){
    const Point3 direction = ray3_direction(ray);
    const long double parameter = dot(point - ray.origin, direction) / norm(direction);
    return parameter <= 0 ? ray.origin : ray.origin + direction * parameter;
}

inline Point3 closest_point(const Segment3& segment, const Point3& point){
    const Point3 direction = segment.b - segment.a;
    if(geometry3d_sign(abs(direction)) == 0) return segment.a;
    const long double parameter = std::clamp(
        dot(point - segment.a, direction) / norm(direction), 0.0L, 1.0L
    );
    return segment.a + direction * parameter;
}

inline Point3 projection(const Plane3& plane, const Point3& point){
    const Point3 normal = plane3_unit_normal(plane);
    return point - normal * dot(point - plane.point, normal);
}

inline Point3 reflection(const Line3& line, const Point3& point){
    return point + (projection(line, point) - point) * 2;
}

inline Point3 reflection(const Plane3& plane, const Point3& point){
    return point + (projection(plane, point) - point) * 2;
}

inline long double distance(const Point3& left, const Point3& right){
    return abs(left - right);
}

inline long double distance(const Line3& line, const Point3& point){
    return abs(point - projection(line, point));
}

inline long double distance(const Ray3& ray, const Point3& point){
    return abs(point - closest_point(ray, point));
}

inline long double distance(const Segment3& segment, const Point3& point){
    return abs(point - closest_point(segment, point));
}

inline long double signed_distance(const Plane3& plane, const Point3& point){
    return dot(point - plane.point, plane3_unit_normal(plane));
}

inline long double distance(const Plane3& plane, const Point3& point){
    return std::abs(signed_distance(plane, point));
}

inline std::pair<Point3, Point3> closest_points(
    const Line3& first,
    const Line3& second
){
    const Point3 first_direction = line3_direction(first);
    const Point3 second_direction = line3_direction(second);
    const Point3 offset = first.a - second.a;
    const long double aa = norm(first_direction);
    const long double bb = dot(first_direction, second_direction);
    const long double cc = norm(second_direction);
    const long double dd = dot(first_direction, offset);
    const long double ee = dot(second_direction, offset);
    const long double denominator = aa * cc - bb * bb;
    if(geometry3d_sign(denominator) == 0){
        const Point3 on_second = projection(second, first.a);
        return {first.a, on_second};
    }
    const long double first_parameter = (bb * ee - cc * dd) / denominator;
    const long double second_parameter = (aa * ee - bb * dd) / denominator;
    return {
        first.a + first_direction * first_parameter,
        second.a + second_direction * second_parameter,
    };
}

inline long double distance(const Line3& first, const Line3& second){
    const auto [left, right] = closest_points(first, second);
    return abs(left - right);
}

inline std::pair<Point3, Point3> closest_points(
    const Segment3& first,
    const Segment3& second
){
    const Point3 first_direction = first.b - first.a;
    const Point3 second_direction = second.b - second.a;
    const Point3 offset = first.a - second.a;
    const long double aa = norm(first_direction);
    const long double ee = norm(second_direction);
    const long double ff = dot(second_direction, offset);

    long double first_parameter = 0;
    long double second_parameter = 0;
    if(geometry3d_sign(aa) == 0 && geometry3d_sign(ee) == 0){
        return {first.a, second.a};
    }
    if(geometry3d_sign(aa) == 0){
        second_parameter = std::clamp(ff / ee, 0.0L, 1.0L);
    }else{
        const long double cc = dot(first_direction, offset);
        if(geometry3d_sign(ee) == 0){
            first_parameter = std::clamp(-cc / aa, 0.0L, 1.0L);
        }else{
            const long double bb = dot(first_direction, second_direction);
            const long double denominator = aa * ee - bb * bb;
            if(geometry3d_sign(denominator) != 0){
                first_parameter = std::clamp(
                    (bb * ff - cc * ee) / denominator, 0.0L, 1.0L
                );
            }
            second_parameter = (bb * first_parameter + ff) / ee;
            if(second_parameter < 0){
                second_parameter = 0;
                first_parameter = std::clamp(-cc / aa, 0.0L, 1.0L);
            }else if(second_parameter > 1){
                second_parameter = 1;
                first_parameter = std::clamp((bb - cc) / aa, 0.0L, 1.0L);
            }
        }
    }
    return {
        first.a + first_direction * first_parameter,
        second.a + second_direction * second_parameter,
    };
}

inline long double distance(const Segment3& first, const Segment3& second){
    const auto [left, right] = closest_points(first, second);
    return abs(left - right);
}
inline Point3 line_plane_intersection(const Line3& line, const Plane3& plane){
    const Point3 direction = line3_direction(line);
    const Point3 normal = plane3_unit_normal(plane);
    const long double denominator = dot(direction, normal);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::domain_error("3D line and plane do not have a unique intersection");
    }
    const long double parameter = dot(plane.point - line.a, normal) / denominator;
    return line.a + direction * parameter;
}

inline std::optional<Point3> ray_plane_intersection(
    const Ray3& ray,
    const Plane3& plane
){
    const Point3 direction = ray3_direction(ray);
    const Point3 normal = plane3_unit_normal(plane);
    const long double denominator = dot(direction, normal);
    if(geometry3d_sign(denominator) == 0) return std::nullopt;
    const long double parameter =
        dot(plane.point - ray.origin, normal) / denominator;
    if(geometry3d_sign(parameter) < 0) return std::nullopt;
    return ray.origin + direction * parameter;
}

inline std::optional<Point3> segment_plane_intersection(
    const Segment3& segment,
    const Plane3& plane
){
    const Point3 direction = segment.b - segment.a;
    if(geometry3d_sign(abs(direction)) == 0){
        return on_plane(plane, segment.a)
            ? std::optional<Point3>(segment.a) : std::nullopt;
    }
    const Point3 normal = plane3_unit_normal(plane);
    const long double denominator = dot(direction, normal);
    if(geometry3d_sign(denominator) == 0) return std::nullopt;
    const long double parameter =
        dot(plane.point - segment.a, normal) / denominator;
    if(geometry3d_sign(parameter) < 0 || geometry3d_sign(parameter - 1) > 0){
        return std::nullopt;
    }
    return segment.a + direction * parameter;
}

inline Line3 plane_plane_intersection(
    const Plane3& first,
    const Plane3& second
){
    const Point3 first_normal = plane3_unit_normal(first);
    const Point3 second_normal = plane3_unit_normal(second);
    const Point3 direction = cross(first_normal, second_normal);
    const long double denominator = norm(direction);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::domain_error("3D planes do not have a unique intersection line");
    }
    const long double first_offset = dot(first_normal, first.point);
    const long double second_offset = dot(second_normal, second.point);
    const Point3 point = cross(
        direction,
        second_offset * first_normal - first_offset * second_normal
    ) / denominator;
    return {point, point + direction};
}

inline std::vector<Point3> line_sphere_intersections(
    const Line3& line,
    const Sphere3& sphere
){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const Point3 direction = line3_direction(line);
    const Point3 base = projection(line, sphere.center);
    const long double squared_height =
        sphere.radius * sphere.radius - norm(base - sphere.center);
    const int sign = geometry3d_sign(squared_height);
    if(sign < 0) return {};
    if(sign == 0) return {base};
    const Point3 offset = unit(direction) * std::sqrt(std::max(0.0L, squared_height));
    return {base - offset, base + offset};
}

inline std::vector<Point3> ray_sphere_intersections(
    const Ray3& ray,
    const Sphere3& sphere
){
    std::vector<Point3> result;
    for(const Point3& point: line_sphere_intersections(
        {ray.origin, ray.through}, sphere
    )){
        if(on_ray(ray, point)) result.push_back(point);
    }
    return result;
}

inline std::vector<Point3> segment_sphere_intersections(
    const Segment3& segment,
    const Sphere3& sphere
){
    if(segment.a == segment.b){
        if(geometry3d_sign(abs(segment.a - sphere.center) - sphere.radius) == 0){
            return {segment.a};
        }
        return {};
    }
    std::vector<Point3> result;
    for(const Point3& point: line_sphere_intersections(
        {segment.a, segment.b}, sphere
    )){
        if(on_segment(segment, point)) result.push_back(point);
    }
    return result;
}

inline std::optional<Circle3> plane_sphere_intersection(
    const Plane3& plane,
    const Sphere3& sphere
){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const Point3 center = projection(plane, sphere.center);
    const long double squared_radius =
        sphere.radius * sphere.radius - norm(center - sphere.center);
    if(geometry3d_sign(squared_radius) < 0) return std::nullopt;
    return Circle3{
        center,
        plane3_unit_normal(plane),
        std::sqrt(std::max(0.0L, squared_radius)),
    };
}

inline std::optional<Circle3> sphere_sphere_intersection(
    const Sphere3& first,
    const Sphere3& second
){
    if(first.radius < 0 || second.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const Point3 difference = second.center - first.center;
    const long double center_distance = abs(difference);
    if(geometry3d_sign(center_distance) == 0){
        if(geometry3d_sign(first.radius - second.radius) == 0)[[unlikely]]{
            throw std::domain_error("coincident spheres have no unique intersection circle");
        }
        return std::nullopt;
    }
    if(
        geometry3d_sign(center_distance - first.radius - second.radius) > 0 ||
        geometry3d_sign(center_distance - std::abs(first.radius - second.radius)) < 0
    ){
        return std::nullopt;
    }
    const Point3 normal = difference / center_distance;
    const long double offset = (
        first.radius * first.radius - second.radius * second.radius +
        center_distance * center_distance
    ) / (2 * center_distance);
    const Point3 center = first.center + normal * offset;
    const long double squared_radius = first.radius * first.radius - offset * offset;
    return Circle3{
        center,
        normal,
        std::sqrt(std::max(0.0L, squared_radius)),
    };
}

inline Point3 triangle_normal(const Triangle3& triangle){
    return unit(cross(triangle.b - triangle.a, triangle.c - triangle.a));
}

inline long double area(const Triangle3& triangle){
    return abs(cross(triangle.b - triangle.a, triangle.c - triangle.a)) / 2;
}

inline Point3 centroid(const Triangle3& triangle){
    return (triangle.a + triangle.b + triangle.c) / 3;
}

inline std::array<long double, 3> barycentric_coordinates(
    const Triangle3& triangle,
    const Point3& point
){
    const Point3 first = triangle.b - triangle.a;
    const Point3 second = triangle.c - triangle.a;
    const Point3 offset = point - triangle.a;
    const long double d00 = dot(first, first);
    const long double d01 = dot(first, second);
    const long double d11 = dot(second, second);
    const long double d20 = dot(offset, first);
    const long double d21 = dot(offset, second);
    const long double denominator = d00 * d11 - d01 * d01;
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const long double second_weight = (d11 * d20 - d01 * d21) / denominator;
    const long double third_weight = (d00 * d21 - d01 * d20) / denominator;
    return {1 - second_weight - third_weight, second_weight, third_weight};
}

inline bool contains(const Triangle3& triangle, const Point3& point){
    const Plane3 plane{triangle.a, cross(
        triangle.b - triangle.a, triangle.c - triangle.a
    )};
    if(!on_plane(plane, point)) return false;
    const auto weights = barycentric_coordinates(triangle, point);
    return std::all_of(weights.begin(), weights.end(), [](long double weight){
        return geometry3d_sign(weight) >= 0;
    });
}

inline Point3 closest_point(const Triangle3& triangle, const Point3& point){
    const Point3 ab = triangle.b - triangle.a;
    const Point3 ac = triangle.c - triangle.a;
    if(geometry3d_sign(abs(cross(ab, ac))) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const Point3 ap = point - triangle.a;
    const long double d1 = dot(ab, ap);
    const long double d2 = dot(ac, ap);
    if(d1 <= 0 && d2 <= 0) return triangle.a;

    const Point3 bp = point - triangle.b;
    const long double d3 = dot(ab, bp);
    const long double d4 = dot(ac, bp);
    if(d3 >= 0 && d4 <= d3) return triangle.b;

    const long double vc = d1 * d4 - d3 * d2;
    if(vc <= 0 && d1 >= 0 && d3 <= 0){
        return triangle.a + ab * (d1 / (d1 - d3));
    }

    const Point3 cp = point - triangle.c;
    const long double d5 = dot(ab, cp);
    const long double d6 = dot(ac, cp);
    if(d6 >= 0 && d5 <= d6) return triangle.c;

    const long double vb = d5 * d2 - d1 * d6;
    if(vb <= 0 && d2 >= 0 && d6 <= 0){
        return triangle.a + ac * (d2 / (d2 - d6));
    }

    const long double va = d3 * d6 - d5 * d4;
    if(va <= 0 && d4 - d3 >= 0 && d5 - d6 >= 0){
        const Point3 edge = triangle.c - triangle.b;
        return triangle.b + edge * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));
    }

    const long double denominator = 1 / (va + vb + vc);
    return triangle.a + ab * (vb * denominator) + ac * (vc * denominator);
}

inline long double distance(const Triangle3& triangle, const Point3& point){
    return abs(point - closest_point(triangle, point));
}

inline long double signed_volume(const Tetrahedron3& tetrahedron){
    return scalar_triple(
        tetrahedron.b - tetrahedron.a,
        tetrahedron.c - tetrahedron.a,
        tetrahedron.d - tetrahedron.a
    ) / 6;
}

inline long double volume(const Tetrahedron3& tetrahedron){
    return std::abs(signed_volume(tetrahedron));
}

inline Point3 centroid(const Tetrahedron3& tetrahedron){
    return (
        tetrahedron.a + tetrahedron.b + tetrahedron.c + tetrahedron.d
    ) / 4;
}

inline std::array<long double, 4> barycentric_coordinates(
    const Tetrahedron3& tetrahedron,
    const Point3& point
){
    const Point3 first = tetrahedron.b - tetrahedron.a;
    const Point3 second = tetrahedron.c - tetrahedron.a;
    const Point3 third = tetrahedron.d - tetrahedron.a;
    const Point3 offset = point - tetrahedron.a;
    const long double denominator = scalar_triple(first, second, third);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const long double second_weight =
        scalar_triple(offset, second, third) / denominator;
    const long double third_weight =
        scalar_triple(first, offset, third) / denominator;
    const long double fourth_weight =
        scalar_triple(first, second, offset) / denominator;
    return {
        1 - second_weight - third_weight - fourth_weight,
        second_weight,
        third_weight,
        fourth_weight,
    };
}

inline bool contains(const Tetrahedron3& tetrahedron, const Point3& point){
    const auto weights = barycentric_coordinates(tetrahedron, point);
    return std::all_of(weights.begin(), weights.end(), [](long double weight){
        return geometry3d_sign(weight) >= 0;
    });
}

inline Circle3 circumcircle(const Triangle3& triangle){
    const Point3 first = triangle.b - triangle.a;
    const Point3 second = triangle.c - triangle.a;
    const Point3 normal = cross(first, second);
    const long double squared_normal = norm(normal);
    if(geometry3d_sign(squared_normal) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const Point3 offset = (
        norm(first) * cross(second, normal) +
        norm(second) * cross(normal, first)
    ) / (2 * squared_normal);
    const Point3 center = triangle.a + offset;
    return {center, unit(normal), abs(offset)};
}

inline Sphere3 circumsphere(const Tetrahedron3& tetrahedron){
    const Point3 first = tetrahedron.b - tetrahedron.a;
    const Point3 second = tetrahedron.c - tetrahedron.a;
    const Point3 third = tetrahedron.d - tetrahedron.a;
    const long double denominator = 2 * scalar_triple(first, second, third);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const Point3 offset = (
        norm(first) * cross(second, third) +
        norm(second) * cross(third, first) +
        norm(third) * cross(first, second)
    ) / denominator;
    const Point3 center = tetrahedron.a + offset;
    return {center, abs(offset)};
}

inline int contains(const Sphere3& sphere, const Point3& point){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    return -geometry3d_sign(abs(point - sphere.center) - sphere.radius);
}

inline bool on_circle(const Circle3& circle, const Point3& point){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    return on_plane({circle.center, circle.normal}, point) &&
        geometry3d_sign(abs(point - circle.center) - circle.radius) == 0;
}

inline long double sphere_surface_area(const Sphere3& sphere){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    return 4 * GEOMETRY3D_PI * sphere.radius * sphere.radius;
}

inline long double sphere_volume(const Sphere3& sphere){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    return 4 * GEOMETRY3D_PI * sphere.radius * sphere.radius * sphere.radius / 3;
}

inline long double sphere_cap_surface_area(
    const Sphere3& sphere,
    long double height
){
    if(
        sphere.radius < 0 || height < 0 ||
        height > 2 * sphere.radius
    )[[unlikely]]{
        throw std::invalid_argument("invalid sphere cap height");
    }
    return 2 * GEOMETRY3D_PI * sphere.radius * height;
}

inline long double sphere_cap_volume(
    const Sphere3& sphere,
    long double height
){
    if(
        sphere.radius < 0 || height < 0 ||
        height > 2 * sphere.radius
    )[[unlikely]]{
        throw std::invalid_argument("invalid sphere cap height");
    }
    return GEOMETRY3D_PI * height * height * (sphere.radius - height / 3);
}

inline long double sphere_intersection_volume(
    const Sphere3& first,
    const Sphere3& second
){
    if(first.radius < 0 || second.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const long double center_distance = abs(second.center - first.center);
    if(geometry3d_sign(center_distance - first.radius - second.radius) >= 0){
        return 0;
    }
    if(geometry3d_sign(center_distance - std::abs(first.radius - second.radius)) <= 0){
        const long double radius = std::min(first.radius, second.radius);
        return 4 * GEOMETRY3D_PI * radius * radius * radius / 3;
    }
    const long double sum = first.radius + second.radius;
    const long double difference = first.radius - second.radius;
    const long double overlap = sum - center_distance;
    return GEOMETRY3D_PI * overlap * overlap * (
        center_distance * center_distance + 2 * center_distance * sum -
        3 * difference * difference
    ) / (12 * center_distance);
}

inline long double angle(const Point3& first, const Point3& second){
    const long double denominator = abs(first) * abs(second);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("angle with zero 3D vector");
    }
    return std::acos(std::clamp(dot(first, second) / denominator, -1.0L, 1.0L));
}

inline Point3 rotate_around_axis(
    const Point3& point,
    const Point3& axis,
    long double theta
){
    const Point3 direction = unit(axis);
    const long double cosine = std::cos(theta);
    const long double sine = std::sin(theta);
    return point * cosine + cross(direction, point) * sine +
        direction * dot(direction, point) * (1 - cosine);
}

inline std::pair<Point3, Point3> plane_orthonormal_basis(const Plane3& plane){
    const Point3 normal = plane3_unit_normal(plane);
    Point3 first;
    if(std::abs(normal.x) <= std::abs(normal.y) &&
        std::abs(normal.x) <= std::abs(normal.z)){
        first = unit(cross(normal, {1, 0, 0}));
    }else if(std::abs(normal.y) <= std::abs(normal.z)){
        first = unit(cross(normal, {0, 1, 0}));
    }else{
        first = unit(cross(normal, {0, 0, 1}));
    }
    return {first, cross(normal, first)};
}
