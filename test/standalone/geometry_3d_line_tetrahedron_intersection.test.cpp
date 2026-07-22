// competitive-verifier: STANDALONE

#include <variant>
#include "../../src/algorithm/geometry/3d/query/line_tetrahedron_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 tetrahedron{{}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        const auto result = line_tetrahedron_intersection(
            {{-1, 0.2L, 0.2L}, {1, 0.2L, 0.2L}}, tetrahedron
        );
        if(!std::holds_alternative<Segment3>(result)) return false;
        const Segment3 segment = std::get<Segment3>(result);
        return geometry3d_api_close(segment.a, {0, 0.2L, 0.2L}) &&
            geometry3d_api_close(segment.b, {0.6L, 0.2L, 0.2L});
    });
}
