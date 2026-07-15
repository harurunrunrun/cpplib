// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/plane_arrangement_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](auto& random, std::size_t rounds){
        const PlaneArrangement3 empty = plane_arrangement_3d({});
        if(empty.region_count != 1) return false;
        const PlaneArrangement3 coordinate = plane_arrangement_3d({
            {{0, 0, 0}, {1, 0, 0}},
            {{0, 0, 0}, {0, 1, 0}},
            {{0, 0, 0}, {0, 0, 1}},
        });
        if(coordinate.region_count != 8
            || coordinate.intersection_lines.size() != 3
            || coordinate.intersection_points.size() != 1) return false;
        const PlaneArrangement3 general = plane_arrangement_3d({
            {{0, 0, 0}, {1, 0, 0}},
            {{0, 0, 0}, {0, 1, 0}},
            {{0, 0, 0}, {0, 0, 1}},
            {{1, 0, 0}, {1, 1, 1}},
            {{0, 0, 0}, {2, 0, 0}},
        });
        if(general.planes.size() != 4 || general.region_count != 15
            || general.intersection_points.size() != 4) return false;
        const PlaneArrangement3 parallel = plane_arrangement_3d({
            {{0, 0, 0}, {1, 0, 0}}, {{1, 0, 0}, {1, 0, 0}},
            {{2, 0, 0}, {-1, 0, 0}},
        });
        if(parallel.region_count != 4 || !parallel.intersection_lines.empty()){
            return false;
        }

        const std::size_t iterations = std::min<std::size_t>(rounds, 30);
        for(std::size_t iteration = 0; iteration < iterations; ++iteration){
            std::vector<Plane3> planes{
                {{0, 0, 0}, {1, 0, 0}},
                {{0, 0, 0}, {0, 1, 0}},
                {{0, 0, 0}, {0, 0, 1}},
                {{1, 0, 0}, {1, 1, 1}},
            };
            std::shuffle(planes.begin(), planes.end(), random);
            if(plane_arrangement_3d(planes).region_count != 15) return false;
        }
        return true;
    });
}
