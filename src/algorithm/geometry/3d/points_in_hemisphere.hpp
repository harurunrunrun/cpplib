#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <vector>

#include "orthonormal_basis_3d.hpp"
#include "spherical_central_angle.hpp"

namespace points_in_hemisphere_detail{

struct ArcEvent{
    long double angle = 0.0L;
    int delta = 0;
};

struct SweepCandidates{
    std::optional<Point3> interior;
    std::optional<Point3> boundary;
};

inline long double normalized_angle(long double angle){
    const long double turn = 2.0L * std::acos(-1.0L);
    angle = std::fmod(angle, turn);
    if(angle < 0.0L) angle += turn;
    if(angle >= turn) angle -= turn;
    return angle;
}

inline SweepCandidates sweep_candidates(
    const Point3& anchor,
    const std::vector<Point3>& points,
    long double tolerance
){
    const auto basis = orthonormal_basis_3d(anchor);
    const Point3& first = basis[1];
    const Point3& second = basis[2];
    const long double turn = 2.0L * std::acos(-1.0L);
    std::vector<ArcEvent> events;
    events.reserve(points.size() * 2);
    int active = 0;
    for(const Point3& point: points){
        const long double x = dot(point, first);
        const long double y = dot(point, second);
        const long double radius = std::hypot(x, y);
        if(radius <= tolerance) continue;

        // A normal at angle theta is rejected exactly on the open arc
        // dot(normal, point) < -tolerance around the opposite projection.
        const long double half_width = std::acos(std::clamp(
            tolerance / radius, 0.0L, 1.0L
        ));
        const long double center = normalized_angle(std::atan2(-y, -x));
        const long double begin = normalized_angle(center - half_width);
        const long double end = normalized_angle(center + half_width);
        if(begin < end){
            events.push_back({begin, 1});
            events.push_back({end, -1});
        }else if(begin > end){
            ++active;
            events.push_back({begin, 1});
            events.push_back({end, -1});
        }
    }
    std::sort(events.begin(), events.end(), [](const ArcEvent& left,
                                               const ArcEvent& right){
        if(left.angle != right.angle) return left.angle < right.angle;
        return left.delta < right.delta;
    });

    long double best_begin = 0.0L;
    long double best_end = 0.0L;
    long double best_width = -1.0L;
    std::optional<long double> boundary_angle;
    long double previous = 0.0L;
    std::size_t index = 0;
    while(index < events.size()){
        const long double angle = events[index].angle;
        if(active == 0 && angle > previous
            && angle - previous > best_width){
            best_begin = previous;
            best_end = angle;
            best_width = angle - previous;
        }

        std::size_t end = index;
        while(end < events.size() && events[end].angle == angle) ++end;
        for(std::size_t event = index; event < end; ++event){
            if(events[event].delta < 0) active += events[event].delta;
        }
        if(active == 0 && !boundary_angle) boundary_angle = angle;
        for(std::size_t event = index; event < end; ++event){
            if(events[event].delta > 0) active += events[event].delta;
        }
        previous = angle;
        index = end;
    }
    if(active == 0 && turn - previous > best_width){
        best_begin = previous;
        best_end = turn;
        best_width = turn - previous;
    }

    const auto normal_at = [&](long double angle){
        return first * std::cos(angle) + second * std::sin(angle);
    };
    SweepCandidates result;
    if(best_width > 0.0L){
        result.interior = normal_at(
            best_begin / 2.0L + best_end / 2.0L
        );
    }
    if(boundary_angle) result.boundary = normal_at(*boundary_angle);
    return result;
}

}  // namespace points_in_hemisphere_detail

inline bool points_in_hemisphere(const std::vector<Point3>& points){
    if(points.empty()) return true;
    std::vector<Point3> unit;
    unit.reserve(points.size());
    for(const Point3& point: points){
        unit.push_back(geometry3d_spherical_detail::unit_direction(
            point, "invalid point in hemisphere containment"
        ));
    }
    const long double tolerance = 256.0L
        * std::numeric_limits<long double>::epsilon();
    const auto accepts = [&](const Point3& normal){
        for(const Point3& point: unit){
            if(dot(normal, point) < -tolerance) return false;
        }
        return true;
    };

    // Preserve the direct point-normal candidates used by the former
    // implementation. They also quickly settle strongly separated inputs.
    for(const Point3& point: unit){
        if(accepts(point)) return true;
    }
    for(const Point3& anchor: unit){
        const auto candidates = points_in_hemisphere_detail::sweep_candidates(
            anchor, unit, tolerance
        );
        if(candidates.interior && accepts(*candidates.interior)) return true;
        if(candidates.boundary && accepts(*candidates.boundary)) return true;
    }
    return false;
}
