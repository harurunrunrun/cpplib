// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/linear_coincident.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return coincident(Line3{{}, {1, 0, 0}}, Line3{{2, 0, 0}, {-1, 0, 0}}) &&
            !coincident(Line3{{}, {1, 0, 0}}, Line3{{0, 1, 0}, {1, 1, 0}}) &&
            coincident(Ray3{{}, {1, 0, 0}}, Ray3{{}, {3, 0, 0}}) &&
            !coincident(Ray3{{}, {1, 0, 0}}, Ray3{{}, {-1, 0, 0}}) &&
            coincident(Segment3{{}, {1, 0, 0}}, Segment3{{1, 0, 0}, {}});
    });
}
