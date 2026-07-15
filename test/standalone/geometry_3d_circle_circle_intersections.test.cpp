// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circle_circle_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 first{{}, {0, 0, 1}, 1};
        const auto coplanar = circle_circle_intersections(first, {{1, 0, 0}, {0, 0, -2}, 1});
        const auto tangent = circle_circle_intersections(first, {{2, 0, 0}, {0, 0, 1}, 1});
        const auto noncoplanar = circle_circle_intersections(first, {{}, {1, 0, 0}, 1});
        if(coplanar.size() != 2 || tangent.size() != 1 || noncoplanar.size() != 2) return false;
        for(const Point3& point: coplanar){
            if(!geometry3d_api_close(point.x, 0.5L) ||
                !geometry3d_api_close(abs(point), 1)) return false;
        }
        try{ (void)circle_circle_intersections(first, first); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
