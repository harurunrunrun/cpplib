// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <variant>
#include <vector>

#include "../../src/algorithm/geometry/3d/polygon3_line_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

Polygon3 concave_polygon(){
    return {
        {-3, -2, 0}, {3, -2, 0}, {3, 2, 0}, {1, 2, 0},
        {1, 0, 0}, {-1, 0, 0}, {-1, 2, 0}, {-3, 2, 0},
    };
}

Polygon3LinearIntersection3 brute_intersection(
    const Polygon3& polygon,
    const Line3& line,
    long double lower = -std::numeric_limits<long double>::infinity(),
    long double upper = std::numeric_limits<long double>::infinity()
){
    using polygon3_line_intersection_detail::at;
    using polygon3_line_intersection_detail::near;
    using polygon3_line_intersection_detail::parameter;
    const Point3 direction = line3_direction(line);
    const Plane3 plane{polygon.front(), polygon3_normal(polygon)};
    Polygon3LinearIntersection3 result;
    if(geometry3d_sign(dot(direction, plane.normal)) != 0){
        const Point3 point = line_plane_intersection(line, plane);
        const long double value = parameter(line, direction, point);
        if(value >= lower && value <= upper
            && polygon3_contains(polygon, point) != 0){
            result.points.push_back(point);
        }
        return result;
    }
    if(!on_plane(plane, line.a)) return result;

    std::vector<long double> events;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Segment3 edge{
            polygon[index], polygon[(index + 1) % polygon.size()]
        };
        const LinearIntersection3 intersection =
            line_segment_intersection(line, edge);
        if(const auto* point = std::get_if<Point3>(&intersection)){
            events.push_back(parameter(line, direction, *point));
        }else if(const auto* segment = std::get_if<Segment3>(&intersection)){
            events.push_back(parameter(line, direction, segment->a));
            events.push_back(parameter(line, direction, segment->b));
        }
    }
    if(std::isfinite(lower)) events.push_back(lower);
    if(std::isfinite(upper)) events.push_back(upper);
    std::sort(events.begin(), events.end());
    events.erase(std::unique(events.begin(), events.end(), near), events.end());
    for(std::size_t index = 0; index + 1 < events.size(); ++index){
        const long double begin = std::max(events[index], lower);
        const long double end = std::min(events[index + 1], upper);
        if(begin > end || near(begin, end)) continue;
        const long double middle = begin / 2.0L + end / 2.0L;
        if(polygon3_contains(
            polygon, at(line, direction, middle)
        ) == 0){
            continue;
        }
        const Segment3 segment{
            at(line, direction, begin), at(line, direction, end)
        };
        if(!result.segments.empty()
            && result.segments.back().b == segment.a){
            result.segments.back().b = segment.b;
        }else{
            result.segments.push_back(segment);
        }
    }
    for(long double event: events){
        if(event < lower || event > upper) continue;
        const Point3 point = at(line, direction, event);
        if(polygon3_contains(polygon, point) == 0) continue;
        bool covered = false;
        for(const Segment3& segment: result.segments){
            const long double first =
                parameter(line, direction, segment.a);
            const long double second =
                parameter(line, direction, segment.b);
            if(event >= std::min(first, second)
                && event <= std::max(first, second)){
                covered = true;
                break;
            }
        }
        if(!covered) result.points.push_back(point);
    }
    return result;
}

std::vector<Point3> isolated_points(
    const Polygon3LinearIntersection3& intersection
){
    std::vector<Point3> result;
    for(const Point3& point: intersection.points){
        const bool covered = std::any_of(
            intersection.segments.begin(), intersection.segments.end(),
            [&](const Segment3& segment){
                return on_segment(segment, point);
            }
        );
        if(!covered) result.push_back(point);
    }
    return result;
}

bool same_result(
    const Polygon3LinearIntersection3& first,
    const Polygon3LinearIntersection3& second
){
    const auto first_points = isolated_points(first);
    const auto second_points = isolated_points(second);
    if(first_points.size() != second_points.size()
        || first.segments.size() != second.segments.size()){
        return false;
    }
    for(std::size_t index = 0; index < first_points.size(); ++index){
        if(!geometry3d_api_close(
            first_points[index], second_points[index], 1.0e-8L
        )){
            return false;
        }
    }
    for(std::size_t index = 0; index < first.segments.size(); ++index){
        if(!geometry3d_api_close(
                first.segments[index].a,
                second.segments[index].a,
                1.0e-8L
            ) || !geometry3d_api_close(
                first.segments[index].b,
                second.segments[index].b,
                1.0e-8L
            )){
            return false;
        }
    }
    return true;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](
        std::mt19937_64& random, std::size_t rounds
    ){
        const auto coplanar = polygon3_line_intersection(
            concave_polygon(), {{-4, 1, 0}, {4, 1, 0}}
        );
        const auto transverse = polygon3_line_intersection(
            concave_polygon(), {{0, -1, -2}, {0, -1, 2}}
        );
        if(!coplanar.points.empty() || coplanar.segments.size() != 2
            || transverse.points.size() != 1
            || !transverse.segments.empty()){
            return false;
        }

        const Polygon3 square{
            {0, 0, 0}, {4, 0, 0}, {4, 4, 0}, {0, 4, 0}
        };
        const auto boundary = polygon3_line_intersection(
            square, {{-1, 0, 0}, {5, 0, 0}}
        );
        const auto tangent = polygon3_line_intersection(
            square, {{-1, 1, 0}, {1, -1, 0}}
        );
        if(!boundary.points.empty() || boundary.segments.size() != 1
            || tangent.points.size() != 1
            || !tangent.segments.empty()){
            return false;
        }

        const long double turn = 2.0L * std::acos(-1.0L);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const std::size_t size = 3 + random() % 13;
            Polygon3 polygon;
            for(std::size_t index = 0; index < size; ++index){
                const long double angle =
                    turn * static_cast<long double>(index)
                    / static_cast<long double>(size);
                const long double radius =
                    5.0L + static_cast<long double>(random() % 11);
                polygon.push_back({
                    radius * std::cos(angle),
                    radius * std::sin(angle),
                    0.0L,
                });
            }
            Point3 first{
                static_cast<long double>(
                    static_cast<long long>(random() % 31) - 15
                ),
                static_cast<long double>(
                    static_cast<long long>(random() % 31) - 15
                ),
                0,
            };
            Point3 second{
                static_cast<long double>(
                    static_cast<long long>(random() % 31) - 15
                ),
                static_cast<long double>(
                    static_cast<long long>(random() % 31) - 15
                ),
                0,
            };
            if(first == second) second.x += 1.0L;
            const Line3 line{first, second};
            const auto fast_line = polygon3_line_intersection(polygon, line);
            const auto slow_line = brute_intersection(polygon, line);
            if(!same_result(fast_line, slow_line)){
                return false;
            }
            long double lower =
                static_cast<long double>(random() % 100) / 100.0L;
            long double upper =
                static_cast<long double>(random() % 100) / 100.0L;
            if(upper < lower) std::swap(lower, upper);
            const auto fast_bounded =
                polygon3_line_intersection_detail::intersect(
                    polygon, line, lower, upper
                );
            const auto slow_bounded =
                brute_intersection(polygon, line, lower, upper);
            if(!same_result(fast_bounded, slow_bounded)){
                return false;
            }
        }

        if(rounds == 32){
            constexpr std::size_t teeth = 6000;
            Polygon3 comb{
                {0, 0, 0},
                {2.0L * static_cast<long double>(teeth), 0, 0},
                {2.0L * static_cast<long double>(teeth), 3, 0},
            };
            comb.reserve(4 * teeth + 2);
            for(std::size_t tooth = teeth; tooth-- > 0;){
                const long double x =
                    2.0L * static_cast<long double>(tooth);
                comb.push_back({x + 1.0L, 3, 0});
                comb.push_back({x + 1.0L, 1, 0});
                comb.push_back({x, 1, 0});
                if(tooth != 0) comb.push_back({x, 3, 0});
            }
            const auto many = polygon3_line_intersection(
                comb,
                {
                    {-1, 2, 0},
                    {2.0L * static_cast<long double>(teeth) + 1.0L, 2, 0}
                }
            );
            if(!many.points.empty() || many.segments.size() != teeth){
                return false;
            }
            for(const Segment3& segment: many.segments){
                if(!geometry3d_api_close(segment.a.y, 2.0L)
                    || !geometry3d_api_close(segment.b.y, 2.0L)
                    || !geometry3d_api_close(
                        segment.b.x - segment.a.x, 1.0L
                    )){
                    return false;
                }
            }
        }
        return true;
    });
}
