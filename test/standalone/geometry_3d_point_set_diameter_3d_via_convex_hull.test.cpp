// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/point_set_diameter_3d_via_convex_hull.hpp"

namespace{

PointSetDiameter3DResult brute(const std::vector<Point3>& points){
    PointSetDiameter3DResult result{
        0,
        1,
        point_set_diameter_3d_detail::scaled_euclidean_distance(
            points[0], points[1]
        ),
    };
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            const long double distance =
                point_set_diameter_3d_detail::scaled_euclidean_distance(
                    points[first], points[second]
                );
            if(distance > result.distance ||
               (distance == result.distance &&
                std::pair{first, second} <
                    std::pair{result.first, result.second})){
                result = {first, second, distance};
            }
        }
    }
    return result;
}

bool equals(
    const std::optional<PointSetDiameter3DResult>& actual,
    const PointSetDiameter3DResult& expected
){
    return actual && actual->first == expected.first &&
        actual->second == expected.second &&
        actual->distance == expected.distance;
}

bool check_against_brute(
    const std::vector<Point3>& points,
    std::uint64_t seed
){
    if(points.size() < 2){
        return !point_set_diameter_3d_via_convex_hull(points) &&
            !point_set_diameter_3d_via_convex_hull_with_seed(points, seed);
    }
    const PointSetDiameter3DResult expected = brute(points);
    const auto fixed = point_set_diameter_3d_via_convex_hull(points);
    const auto seeded =
        point_set_diameter_3d_via_convex_hull_with_seed(points, seed);
    const auto second_seed =
        point_set_diameter_3d_via_convex_hull_with_seed(
            points, seed ^ 0x9e3779b97f4a7c15ULL
        );
    if(equals(fixed, expected) && equals(seeded, expected) &&
       equals(second_seed, expected)) return true;
    const auto print = [](const auto& value){
        if(!value){
            std::cerr << " null";
            return;
        }
        std::cerr << ' ' << value->first << ' ' << value->second << ' '
            << value->distance;
    };
    std::cerr << "mismatch size=" << points.size() << " expected "
        << expected.first << ' ' << expected.second << ' '
        << expected.distance << " fixed";
    print(fixed);
    std::cerr << " seeded";
    print(seeded);
    std::cerr << " second";
    print(second_seed);
    std::cerr << '\n';
    return false;
}

}  // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    if(!check_against_brute({}, seed) ||
       !check_against_brute({{1, 2, 3}}, seed)) return 1;

    const std::vector<Point3> identical(40, {7, -3, 11});
    if(!check_against_brute(identical, seed)) return 1;

    const std::vector<Point3> collinear{{
        {0, 0, 0}, {-4, 8, -12}, {4, -8, 12},
        {-4, 8, -12}, {4, -8, 12}, {1, -2, 3}, {-1, 2, -3},
    }};
    if(!check_against_brute(collinear, seed)) return 1;

    const std::vector<Point3> coplanar_tied{{
        {-1, -1, 5}, {1, 1, 5}, {-1, 1, 5}, {1, -1, 5},
        {-1, -1, 5}, {0, 0, 5}, {1, 1, 5},
    }};
    if(!check_against_brute(coplanar_tied, seed)) return 1;

    const std::vector<Point3> spatial_tied{{
        {-1, -1, -1}, {1, 1, 1}, {-1, -1, 1}, {1, 1, -1},
        {-1, 1, -1}, {1, -1, 1}, {-1, 1, 1}, {1, -1, -1},
        {-1, -1, -1}, {0, 0, 0}, {1, 1, 1},
    }};
    if(!check_against_brute(spatial_tied, seed)) return 1;

    const long double huge =
        std::numeric_limits<long double>::max() / 64;
    const std::vector<Point3> huge_points{{
        {huge, huge, huge}, {-huge, -huge, -huge},
        {huge, -huge, -huge}, {-huge, huge, -huge},
        {-huge, -huge, huge}, {0, 0, 0},
        {huge / 2, huge / 3, -huge / 4},
    }};
    if(!check_against_brute(huge_points, seed)) return 1;

    const long double maximum =
        std::numeric_limits<long double>::max();
    const std::vector<Point3> overflowing_collinear{{
        {maximum / 2, 0, 0}, {-maximum, 0, 0}, {maximum, 0, 0},
        {0, 0, 0},
    }};
    if(!check_against_brute(overflowing_collinear, seed)) return 1;
    const auto overflowing_fixed =
        point_set_diameter_3d_via_convex_hull(overflowing_collinear);
    const auto overflowing_seeded =
        point_set_diameter_3d_via_convex_hull_with_seed(
            overflowing_collinear, seed
        );
    if(!overflowing_fixed || !overflowing_seeded ||
       overflowing_fixed->first != 0 ||
       overflowing_fixed->second != 1 ||
       !std::isinf(overflowing_fixed->distance) ||
       overflowing_seeded->first != 0 ||
       overflowing_seeded->second != 1 ||
       !std::isinf(overflowing_seeded->distance)) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-100, 100);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size =
            2 + static_cast<std::size_t>(random() % 70);
        std::vector<Point3> points;
        points.reserve(size);
        const int mode = static_cast<int>(iteration % 4);
        const long double shift = iteration % 2 == 0 ? 1e14L : 0;
        for(std::size_t index = 0; index < size; ++index){
            if(index != 0 && index % 11 == 0){
                points.push_back(points[static_cast<std::size_t>(
                    random() % points.size()
                )]);
                continue;
            }
            const long double x = coordinate(random);
            const long double y = coordinate(random);
            const long double z = coordinate(random);
            if(mode == 0){
                points.push_back({
                    shift + x, -shift - 2 * x, shift + 3 * x
                });
            }else if(mode == 1){
                points.push_back({
                    shift + x, -shift + y, shift + x + 2 * y
                });
            }else{
                points.push_back({
                    shift + x, -shift + y, shift + z
                });
            }
        }
        if(!check_against_brute(points, random())) return 1;
    }

    std::vector<Point3> interior_heavy;
    interior_heavy.reserve(2502);
    interior_heavy.push_back({-1000000, 0, 0});
    interior_heavy.push_back({1000000, 0, 0});
    for(std::size_t index = 0; index < 2500; ++index){
        interior_heavy.push_back({
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
            static_cast<long double>(coordinate(random)),
        });
    }
    const PointSetDiameter3DResult expected{0, 1, 2000000};
    if(!equals(
           point_set_diameter_3d_via_convex_hull(interior_heavy), expected
       ) ||
       !equals(
           point_set_diameter_3d_via_convex_hull_with_seed(
               interior_heavy, random()
           ),
           expected
       )) return 1;

    bool rejected = false;
    try{
        static_cast<void>(point_set_diameter_3d_via_convex_hull({
            {0, 0, 0},
            {std::numeric_limits<long double>::infinity(), 0, 0},
        }));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 1;

    std::cout << "OK\n";
    return 0;
}
