// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/line_circle_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{}, {0, 0, 1}, 1};
        const auto secant = line_circle_intersections({{-2, 0, 0}, {2, 0, 0}}, circle);
        const auto tangent = line_circle_intersections({{-2, 1, 0}, {2, 1, 0}}, circle);
        const auto miss = line_circle_intersections({{0, 0, -2}, {0, 0, 2}}, circle);
        return secant.size() == 2 &&
            geometry3d_api_close(secant[0], {-1, 0, 0}) &&
            geometry3d_api_close(secant[1], {1, 0, 0}) &&
            tangent.size() == 1 && geometry3d_api_close(tangent[0], {0, 1, 0}) &&
            miss.empty();
    });
}
