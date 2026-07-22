// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/points_in_hemisphere.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool brute_points_in_hemisphere(const std::vector<Point3>& points){
    if(points.empty()) return true;
    std::vector<Point3> unit;
    for(const Point3& point: points){
        unit.push_back(geometry3d_spherical_detail::unit_direction(
            point, "invalid brute hemisphere point"
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
    for(const Point3& point: unit){
        if(accepts(point)) return true;
    }
    for(std::size_t first = 0; first < unit.size(); ++first){
        for(std::size_t second = first + 1; second < unit.size(); ++second){
            const Point3 product = cross(unit[first], unit[second]);
            const long double length = std::hypot(
                product.x, product.y, product.z
            );
            if(length <= tolerance) continue;
            const Point3 normal = product / length;
            if(accepts(normal) || accepts(-normal)) return true;
        }
    }
    bool all_parallel = true;
    for(std::size_t index = 1; index < unit.size(); ++index){
        const Point3 product = cross(unit[0], unit[index]);
        if(std::hypot(product.x, product.y, product.z) > tolerance){
            all_parallel = false;
            break;
        }
    }
    return all_parallel;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](
        std::mt19937_64& random, std::size_t rounds
    ){
        if(!points_in_hemisphere({})
            || !points_in_hemisphere({{1, 0, 0}, {-1, 0, 0}})
            || !points_in_hemisphere({
                {1, 0, 0}, {0, 1, 0}, {-1, 0, 0}, {0, -1, 0}
            }) || points_in_hemisphere({
                {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}
            }) || points_in_hemisphere({
                {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
                {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
            })) return false;

        const long double machine_epsilon =
            std::numeric_limits<long double>::epsilon();
        for(const long double multiplier: {290.0L, 300.0L, 310.0L}){
            const std::vector<Point3> nearly_infeasible{
                {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
                {-1, -1, -multiplier * machine_epsilon},
            };
            const bool first_result =
                points_in_hemisphere(nearly_infeasible);
            for(std::size_t repetition = 0; repetition < 4096; ++repetition){
                if(points_in_hemisphere(nearly_infeasible)
                    != first_result) return false;
            }
        }
        const std::vector<Point3> separated_beyond_tolerance{
            {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
            {-1, -1, -10000.0L * machine_epsilon},
        };
        const bool separated_result =
            points_in_hemisphere(separated_beyond_tolerance);
        for(std::size_t repetition = 0; repetition < 4096; ++repetition){
            if(points_in_hemisphere(separated_beyond_tolerance)
                != separated_result) return false;
        }

        bool zero_rejected = false;
        try{
            static_cast<void>(points_in_hemisphere({{0, 0, 0}}));
        }catch(const std::invalid_argument&){
            zero_rejected = true;
        }
        if(!zero_rejected) return false;

        const std::vector<Point3> catalogue{
            {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0},
            {0, 0, 1}, {0, 0, -1}, {1, 1, 1}, {1, -1, -1},
            {-1, 1, -1}, {-1, -1, 1}, {2, 1, -3}, {-2, -1, 3},
        };
        for(std::size_t mask = 0;
            mask < (std::size_t{1} << catalogue.size());
            ++mask){
            std::vector<Point3> subset;
            for(std::size_t index = 0; index < catalogue.size(); ++index){
                if((mask >> index) % 2 != 0){
                    subset.push_back(catalogue[index]);
                }
            }
            if(points_in_hemisphere(subset)
                != brute_points_in_hemisphere(subset)) return false;
        }

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 1 + random() % 11;
            while(points.size() < count){
                const auto coordinate = [&]{
                    return static_cast<long double>(
                        static_cast<long long>(random() % 15) - 7
                    );
                };
                const Point3 point{coordinate(), coordinate(), coordinate()};
                if(point.x != 0 || point.y != 0 || point.z != 0){
                    points.push_back(point);
                }
            }
            if(points_in_hemisphere(points)
                != brute_points_in_hemisphere(points)){
                return false;
            }
        }

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 3 + random() % 20;
            for(std::size_t index = 0; index < count; ++index){
                points.push_back({
                    static_cast<long double>(
                        static_cast<int>(random() % 201) - 100
                    ),
                    static_cast<long double>(
                        static_cast<int>(random() % 201) - 100
                    ),
                    static_cast<long double>(random() % 100 + 1),
                });
            }
            if(!points_in_hemisphere(points)) return false;
        }

        if(rounds == 32){
            const Point3 tetrahedron[4]{
                {1, 1, 1}, {1, -1, -1},
                {-1, 1, -1}, {-1, -1, 1}
            };
            std::vector<Point3> adversarial;
            constexpr std::size_t size = 100000;
            adversarial.reserve(size);
            for(std::size_t index = 0; index < size; ++index){
                adversarial.push_back(tetrahedron[index % 4]);
            }
            if(points_in_hemisphere(adversarial)) return false;

            std::vector<Point3> upper;
            upper.reserve(size);
            for(std::size_t index = 0; index < size; ++index){
                const long double x =
                    static_cast<long double>(index % 101) - 50.0L;
                const long double y =
                    static_cast<long double>((index * 37) % 103) - 51.0L;
                upper.push_back({x, y, 1.0L});
            }
            if(!points_in_hemisphere(upper)) return false;
        }
        return true;
    });
}
