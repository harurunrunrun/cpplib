// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <variant>
#include <vector>

#include "../../src/algorithm/geometry/3d/plane_arrangement_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

void add_unique_point(std::vector<Point3>& points, const Point3& point){
    if(std::none_of(
        points.begin(), points.end(),
        [&](const Point3& other){ return point == other; }
    )){
        points.push_back(point);
    }
}

std::uint64_t slow_line_region_count(const std::vector<Line3>& lines){
    using plane_arrangement_3d_detail::checked_add;
    std::uint64_t regions = 1;
    for(std::size_t line = 0; line < lines.size(); ++line){
        std::vector<Point3> intersections;
        for(std::size_t previous = 0; previous < line; ++previous){
            const LinearIntersection3 intersection =
                line_line_intersection(lines[line], lines[previous]);
            if(const auto* point = std::get_if<Point3>(&intersection)){
                add_unique_point(intersections, *point);
            }
        }
        regions = checked_add(
            regions, static_cast<std::uint64_t>(intersections.size()) + 1
        );
    }
    return regions;
}

PlaneArrangement3 brute_arrangement(const std::vector<Plane3>& input){
    using namespace plane_arrangement_3d_detail;
    PlaneArrangement3 result;
    for(const Plane3& plane: input){
        const Plane3 normalized{plane.point, plane3_unit_normal(plane)};
        if(std::any_of(
            result.planes.begin(), result.planes.end(),
            [&](const Plane3& other){
                return same_plane(normalized, other);
            }
        )){
            continue;
        }
        std::vector<Line3> induced;
        for(const Plane3& previous: result.planes){
            if(parallel(normalized.normal, previous.normal)) continue;
            add_unique_line(
                induced, plane_plane_intersection(normalized, previous)
            );
        }
        result.region_count = checked_add(
            result.region_count, slow_line_region_count(induced)
        );
        result.planes.push_back(normalized);
    }
    for(std::size_t first = 0; first < result.planes.size(); ++first){
        for(std::size_t second = first + 1;
            second < result.planes.size(); ++second){
            if(parallel(
                result.planes[first].normal,
                result.planes[second].normal
            )){
                continue;
            }
            add_unique_line(
                result.intersection_lines,
                plane_plane_intersection(
                    result.planes[first], result.planes[second]
                )
            );
            for(std::size_t third = second + 1;
                third < result.planes.size(); ++third){
                const ThreePlaneIntersection3 intersection =
                    three_plane_intersection(
                        result.planes[first],
                        result.planes[second],
                        result.planes[third]
                    );
                if(const auto* point = std::get_if<Point3>(&intersection)){
                    add_unique_point(result.intersection_points, *point);
                }
            }
        }
    }
    return result;
}

bool same_arrangement(
    const PlaneArrangement3& first,
    const PlaneArrangement3& second
){
    if(first.planes.size() != second.planes.size()
        || first.intersection_lines.size()
            != second.intersection_lines.size()
        || first.intersection_points.size()
            != second.intersection_points.size()
        || first.region_count != second.region_count){
        return false;
    }
    for(const Line3& line: first.intersection_lines){
        if(std::none_of(
            second.intersection_lines.begin(),
            second.intersection_lines.end(),
            [&](const Line3& other){ return coincident(line, other); }
        )){
            return false;
        }
    }
    for(const Point3& point: first.intersection_points){
        if(std::none_of(
            second.intersection_points.begin(),
            second.intersection_points.end(),
            [&](const Point3& other){ return point == other; }
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
        const PlaneArrangement3 empty = plane_arrangement_3d({});
        if(empty.region_count != 1) return false;
        const PlaneArrangement3 coordinate = plane_arrangement_3d({
            {{0, 0, 0}, {1, 0, 0}},
            {{0, 0, 0}, {0, 1, 0}},
            {{0, 0, 0}, {0, 0, 1}},
        });
        if(coordinate.region_count != 8
            || coordinate.intersection_lines.size() != 3
            || coordinate.intersection_points.size() != 1){
            return false;
        }
        const PlaneArrangement3 general = plane_arrangement_3d({
            {{0, 0, 0}, {1, 0, 0}},
            {{0, 0, 0}, {0, 1, 0}},
            {{0, 0, 0}, {0, 0, 1}},
            {{1, 0, 0}, {1, 1, 1}},
            {{0, 0, 0}, {2, 0, 0}},
        });
        if(general.planes.size() != 4 || general.region_count != 15
            || general.intersection_points.size() != 4){
            return false;
        }
        const PlaneArrangement3 parallel = plane_arrangement_3d({
            {{0, 0, 0}, {1, 0, 0}},
            {{1, 0, 0}, {1, 0, 0}},
            {{2, 0, 0}, {-1, 0, 0}},
        });
        if(parallel.region_count != 4
            || !parallel.intersection_lines.empty()){
            return false;
        }

        // The exact quotient for x is -3.  Converting its numerator and
        // denominator separately loses one ulp and makes the line fail the
        // exact plane-membership check.
        const Line3 exactly_rounded = plane_plane_intersection(
            {{-3, -2, -1}, {1, 0, 0}},
            {{-1, -2, -3}, unit(Point3{0, 2, -3})}
        );
        if(exactly_rounded.a.x != -3 || exactly_rounded.b.x != -3
            || !on_plane(
                {{-3, -2, -1}, {1, 0, 0}}, exactly_rounded.a
            )
            || !on_plane(
                {{-1, -2, -3}, unit(Point3{0, 2, -3})},
                exactly_rounded.b
            )){
            return false;
        }

        using geometry3d_adaptive_detail::ExactDyadic;
        using geometry3d_plane_numeric_detail::exact_ratio;
        constexpr int precision =
            std::numeric_limits<long double>::digits;
        const ExactInteger ratio_scale = ExactInteger(1) << precision;
        const ExactDyadic ratio_denominator{1, 0};
        const long double next = std::nextafter(
            1.0L, std::numeric_limits<long double>::infinity()
        );
        const long double next_twice = std::nextafter(
            next, std::numeric_limits<long double>::infinity()
        );
        if(exact_ratio(
                {ratio_scale + 1, -precision}, ratio_denominator,
                "exact ratio tie-to-even test overflow"
            ) != 1.0L
            || exact_ratio(
                {ratio_scale + 3, -precision}, ratio_denominator,
                "exact ratio odd tie test overflow"
            ) != next_twice
            || exact_ratio(
                {ratio_scale + 1, -precision}, {-1, 0},
                "exact ratio sign test overflow"
            ) != -1.0L
            || exact_ratio(
                {1, std::numeric_limits<long double>::min_exponent - 1},
                ratio_denominator, "exact ratio minimum test overflow"
            ) != (std::numeric_limits<long double>::min)()
            || exact_ratio(
                {
                    (ExactInteger(1) << precision) - 1,
                    std::numeric_limits<long double>::max_exponent - precision,
                },
                ratio_denominator, "exact ratio maximum test overflow"
            ) != (std::numeric_limits<long double>::max)()){
            return false;
        }
        if constexpr(std::numeric_limits<long double>::has_denorm
            != std::denorm_absent){
            if(exact_ratio(
                    {
                        1,
                        std::numeric_limits<long double>::min_exponent
                            - precision,
                    },
                    ratio_denominator, "exact ratio subnormal test overflow"
                ) != (std::numeric_limits<long double>::denorm_min)()){
                return false;
            }
        }

        const std::size_t iterations = std::min<std::size_t>(rounds, 40);
        for(std::size_t iteration = 0; iteration < iterations; ++iteration){
            std::vector<Plane3> planes;
            const std::size_t size = random() % 9;
            while(planes.size() < size){
                const auto coordinate = [&]{
                    return static_cast<long double>(
                        static_cast<long long>(random() % 9) - 4
                    );
                };
                const Point3 normal{
                    coordinate(), coordinate(), coordinate()
                };
                if(normal.x == 0 && normal.y == 0 && normal.z == 0){
                    continue;
                }
                planes.push_back({
                    {coordinate(), coordinate(), coordinate()}, normal
                });
            }
            if(!same_arrangement(
                plane_arrangement_3d(planes),
                brute_arrangement(planes)
            )){
                return false;
            }
        }

        for(std::size_t iteration = 0; iteration < iterations; ++iteration){
            std::vector<Plane3> planes{
                {{0, 0, 0}, {1, 0, 0}},
                {{0, 0, 0}, {0, 1, 0}},
                {{0, 0, 0}, {0, 0, 1}},
                {{1, 0, 0}, {1, 1, 1}},
            };
            std::shuffle(planes.begin(), planes.end(), random);
            if(plane_arrangement_3d(planes).region_count != 15){
                return false;
            }
        }

        if(rounds == 32){
            constexpr std::size_t size = 48;
            std::vector<Plane3> planes;
            planes.reserve(size);
            for(std::size_t index = 1; index <= size; ++index){
                const long double value =
                    static_cast<long double>(index);
                planes.push_back({
                    {0, 0, value * value * value},
                    {value, value * value, -1},
                });
            }
            const PlaneArrangement3 arrangement =
                plane_arrangement_3d(planes);
            const std::uint64_t lines =
                size * (size - 1) / 2;
            const std::uint64_t points =
                size * (size - 1) * (size - 2) / 6;
            if(arrangement.planes.size() != size
                || arrangement.intersection_lines.size() != lines
                || arrangement.intersection_points.size() != points
                || arrangement.region_count
                    != 1 + size + lines + points){
                return false;
            }
        }
        return true;
    });
}
