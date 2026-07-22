// competitive-verifier: STANDALONE

#include <limits>

#include "../../src/algorithm/geometry/3d/scalar/distance.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        if(!geometry3d_close_value(
            distance(Point3{0, 0, 0}, Point3{1, 2, 2}), 3
        ) || !geometry3d_close_value(
            distance(Line3{{0, 0, 0}, {1, 0, 0}}, {0, 3, 4}), 5
        ) || !geometry3d_close_value(
            distance(Ray3{{0, 0, 0}, {1, 0, 0}}, {-2, 0, 0}), 2
        ) || !geometry3d_close_value(
            distance(Segment3{{0, 0, 0}, {2, 0, 0}}, {3, 0, 0}), 1
        ) || !geometry3d_close_value(
            distance(Plane3{{0, 0, 0}, {0, 0, 1}}, {0, 0, -3}), 3
        ) || !geometry3d_close_value(
            distance(
                Line3{{0, 0, 0}, {1, 0, 0}},
                Line3{{0, 2, 0}, {1, 2, 0}}
            ),
            2
        ) || !geometry3d_close_value(
            distance(
                Segment3{{0, 0, 0}, {1, 0, 0}},
                Segment3{{3, 0, 0}, {4, 0, 0}}
            ),
            2
        ) || !geometry3d_close_value(
            distance(
                Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
                {0.5L, 0.5L, 3}
            ),
            3
        )) return false;
        const long double r = GEOMETRY3D_LINEAR_TEST_LOCAL;
        const long double t = GEOMETRY3D_LINEAR_TEST_TRANSLATION;
        if(!geometry3d_linear_test_close(
                distance(Point3{0, 0, t}, Point3{r, 0, t}), r)
            || !geometry3d_linear_test_close(
                distance(
                    geometry3d_linear_test_line(),
                    geometry3d_linear_test_parallel_line()
                ),
                r
            )
            || !geometry3d_linear_test_close(
                distance(
                    geometry3d_linear_test_triangle(), {2 * r, 2 * r, t}
                ),
                3 * r / std::sqrt(2.0L)
            )){
            return false;
        }
        const long double epsilon =
            1000 * std::numeric_limits<long double>::denorm_min();
        if(distance(
            Line3{{0, -1, 0}, {1, -1, 0}},
            Line3{{0, 0, 0}, {1, epsilon, 0}}
        ) != 0.0L){
            return false;
        }
        bool local_underflow = false;
        try{
            static_cast<void>(distance(
                Segment3{{0, 0, 0}, {0, 0, 0}},
                Segment3{{0, r, t}, {0, r, 0}}
            ));
        }catch(const std::overflow_error&){
            local_underflow = true;
        }
        if(!local_underflow) return false;
        const long double maximum =
            0.75L * std::numeric_limits<long double>::max();
        try{
            static_cast<void>(distance(
                Point3{-maximum, 0, 0}, Point3{maximum, 0, 0}
            ));
        }catch(const std::overflow_error&){
            return true;
        }
        return false;
    });
}
