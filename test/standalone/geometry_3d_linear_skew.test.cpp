// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/linear_skew.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 x_axis{{}, {1, 0, 0}};
        return skew(x_axis, Line3{{0, 1, 1}, {0, 2, 1}}) &&
            !skew(x_axis, Line3{{0, -1, 0}, {0, 1, 0}}) &&
            !skew(x_axis, Line3{{0, 1, 0}, {1, 1, 0}}) &&
            skew(Ray3{{}, {1, 0, 0}}, Segment3{{0, 1, 1}, {0, 2, 1}});
    });
}
