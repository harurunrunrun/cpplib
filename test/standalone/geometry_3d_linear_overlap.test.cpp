// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/linear_overlap.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 line{{}, {1, 0, 0}};
        return overlap(line, Line3{{2, 0, 0}, {3, 0, 0}}) &&
            overlap(line, Ray3{{2, 0, 0}, {3, 0, 0}}) &&
            overlap(line, Segment3{{-1, 0, 0}, {1, 0, 0}}) &&
            overlap(Ray3{{}, {1, 0, 0}}, Ray3{{2, 0, 0}, {1, 0, 0}}) &&
            overlap(Ray3{{}, {1, 0, 0}}, Segment3{{-1, 0, 0}, {2, 0, 0}}) &&
            overlap(Segment3{{}, {3, 0, 0}}, Segment3{{1, 0, 0}, {4, 0, 0}}) &&
            !overlap(Segment3{{}, {1, 0, 0}}, Segment3{{1, 0, 0}, {2, 0, 0}});
    });
}
