// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/line_line/line_line_coincident.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/linear_coincident.hpp"
#include "../../src/algorithm/geometry/3d/predicate/ray_ray/ray_ray_coincident.hpp"
#include "../../src/algorithm/geometry/3d/predicate/segment_segment/segment_segment_coincident.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        if(!(coincident(
                Line3{{}, {1, 0, 0}},
                Line3{{2, 0, 0}, {-1, 0, 0}}
            ) &&
            !coincident(
                Line3{{}, {1, 0, 0}},
                Line3{{0, 1, 0}, {1, 1, 0}}
            ) &&
            coincident(Ray3{{}, {1, 0, 0}}, Ray3{{}, {3, 0, 0}}) &&
            !coincident(Ray3{{}, {1, 0, 0}}, Ray3{{}, {-1, 0, 0}}) &&
            coincident(
                Segment3{{}, {1, 0, 0}},
                Segment3{{1, 0, 0}, {}}
            ))){
            return false;
        }
        try{
            (void)coincident(Line3{{}, {}}, Line3{{}, {1, 0, 0}});
            return false;
        }catch(const std::invalid_argument&){
        }
        try{
            (void)coincident(Ray3{{}, {}}, Ray3{{1, 0, 0}, {2, 0, 0}});
            return false;
        }catch(const std::invalid_argument&){
        }
        return true;
    });
}
