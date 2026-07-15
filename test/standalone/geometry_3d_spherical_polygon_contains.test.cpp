// competitive-verifier: STANDALONE

#include <vector>

#include "../../src/algorithm/geometry/3d/spherical_polygon_contains.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const std::vector<Point3> cap{
            {1, 0, 1}, {0, 1, 1}, {-1, 0, 1}, {0, -1, 1}
        };
        if(!spherical_polygon_contains(cap, {0, 0, 1})
            || !spherical_polygon_contains(cap, {1, 1, 2})
            || spherical_polygon_contains(cap, {0, 0, -1})
            || spherical_polygon_contains({{1, 0, 0}, {0, 1, 0}}, {1, 1, 0})){
            return false;
        }
        bool antipodal_edge_rejected = false;
        try{
            (void)spherical_polygon_contains(
                {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}}, {0, 0, 1}
            );
        }catch(const std::domain_error&){ antipodal_edge_rejected = true; }
        if(!antipodal_edge_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const long double positive = static_cast<long double>(random() % 1000 + 1);
            if(!spherical_polygon_contains(cap, {0, 0, positive})
                || spherical_polygon_contains(cap, {0, 0, -positive})) return false;
        }
        return true;
    });
}
