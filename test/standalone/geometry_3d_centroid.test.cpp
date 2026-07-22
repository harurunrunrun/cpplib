// competitive-verifier: STANDALONE

#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/point/centroid.hpp"
#include "geometry_3d_test_common.hpp"

namespace{

template<class Function>
bool throws_invalid_argument(Function function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

}  // namespace

int main(){
    return geometry3d_test_main([](){
        if(!geometry3d_close_point(
            centroid(Triangle3{{0, 0, 0}, {3, 0, 0}, {0, 3, 0}}),
            {1, 1, 0}
        ) && geometry3d_close_point(
            centroid(Tetrahedron3{
                {0, 0, 0}, {4, 0, 0}, {0, 4, 0}, {0, 0, 4}
            }),
            {1, 1, 1}
        )) return false;

        const long double maximum = std::numeric_limits<long double>::max();
        const long double large = maximum * 0.75L;
        const Point3 large_triangle = centroid(Triangle3{
            {large, large, large},
            {large, large, large},
            {large, large, large},
        });
        if(!geometry3d_close_point(
            large_triangle, {large, large, large}, 1e-18L
        ) || !geometry3d_close_point(
            centroid(Tetrahedron3{
                {large, 0, 0},
                {large, 0, 0},
                {-large, 0, 0},
                {-large, 0, 0},
            }),
            {0, 0, 0},
            1e-18L
        )) return false;

        for(const long double scale: {1e-2000L, 1e2000L}){
            if(!geometry3d_close_value(
                centroid(Triangle3{
                    {scale, 0, 0}, {2 * scale, 0, 0}, {3 * scale, 0, 0},
                }).x / scale,
                2.0L
            ) || !geometry3d_close_value(
                centroid(Tetrahedron3{
                    {scale, 0, 0},
                    {2 * scale, 0, 0},
                    {3 * scale, 0, 0},
                    {4 * scale, 0, 0},
                }).x / scale,
                2.5L
            )) return false;
        }

        const long double nan = std::numeric_limits<long double>::quiet_NaN();
        const long double infinity =
            std::numeric_limits<long double>::infinity();
        return throws_invalid_argument([nan](){
            (void)centroid(Triangle3{
                {0, 0, 0}, {nan, 0, 0}, {0, 1, 0},
            });
        }) && throws_invalid_argument([infinity](){
            (void)centroid(Tetrahedron3{
                {0, 0, 0}, {1, 0, 0}, {0, infinity, 0}, {0, 0, 1},
            });
        }) && geometry3d_close_point(
            centroid(Triangle3{{2, 3, 4}, {2, 3, 4}, {2, 3, 4}}),
            {2, 3, 4}
        );
    });
}
