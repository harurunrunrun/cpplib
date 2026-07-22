// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/linear_parallel.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 line{{}, {1, 0, 0}};
        const Ray3 ray{{0, 2, 0}, {-2, 2, 0}};
        const Segment3 segment{{0, 0, 1}, {4, 0, 1}};
        if(!parallel(line, ray) || !parallel(ray, segment) ||
            !parallel(segment, line) || parallel(line, Line3{{}, {0, 1, 0}})){
            return false;
        }
        try{ (void)parallel(line, Segment3{{}, {}}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
