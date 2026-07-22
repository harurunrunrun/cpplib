// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/query/barycentric_coordinates.hpp"
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
        const auto triangle = barycentric_coordinates(
            Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
            {0.5L, 0.5L, 0}
        );
        const auto tetrahedron = barycentric_coordinates(
            Tetrahedron3{
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
            },
            {0.1L, 0.2L, 0.3L}
        );
        const bool basic = geometry3d_close_value(triangle[0], 0.5L) &&
            geometry3d_close_value(triangle[1], 0.25L) &&
            geometry3d_close_value(triangle[2], 0.25L) &&
            geometry3d_close_value(tetrahedron[0], 0.4L) &&
            geometry3d_close_value(tetrahedron[1], 0.1L) &&
            geometry3d_close_value(tetrahedron[2], 0.2L) &&
            geometry3d_close_value(tetrahedron[3], 0.3L);
        if(!basic) return false;

        for(const long double scale: {1e-2000L, 1e2000L}){
            const auto scaled_triangle = barycentric_coordinates(
                Triangle3{
                    {0, 0, 0}, {2 * scale, 0, 0}, {0, 2 * scale, 0},
                },
                {0.5L * scale, 0.5L * scale, 3 * scale}
            );
            if(!geometry3d_close_value(scaled_triangle[0], 0.5L)
                || !geometry3d_close_value(scaled_triangle[1], 0.25L)
                || !geometry3d_close_value(scaled_triangle[2], 0.25L)){
                return false;
            }
            const auto scaled_tetrahedron = barycentric_coordinates(
                Tetrahedron3{
                    {0, 0, 0},
                    {scale, 0, 0},
                    {0, scale, 0},
                    {0, 0, scale},
                },
                {0.1L * scale, 0.2L * scale, 0.3L * scale}
            );
            if(!geometry3d_close_value(scaled_tetrahedron[0], 0.4L)
                || !geometry3d_close_value(scaled_tetrahedron[1], 0.1L)
                || !geometry3d_close_value(scaled_tetrahedron[2], 0.2L)
                || !geometry3d_close_value(scaled_tetrahedron[3], 0.3L)){
                return false;
            }
        }

        const long double absolute_translation = 1e3000L;
        const long double absolute_local = 1e-3000L;
        const auto absolute_scale_triangle = barycentric_coordinates(
            Triangle3{
                {0, 0, absolute_translation},
                {absolute_local, 0, absolute_translation},
                {0, absolute_local, absolute_translation},
            },
            {absolute_local / 4, absolute_local / 4, absolute_translation}
        );
        if(!geometry3d_close_value(absolute_scale_triangle[0], 0.5L)
            || !geometry3d_close_value(absolute_scale_triangle[1], 0.25L)
            || !geometry3d_close_value(absolute_scale_triangle[2], 0.25L)){
            return false;
        }

        const long double maximum = std::numeric_limits<long double>::max();
        const long double near_maximum = maximum * 0.75L;
        const auto extreme = barycentric_coordinates(
            Triangle3{
                {near_maximum, 0, 0},
                {-near_maximum, 0, 0},
                {0, near_maximum, 0},
            },
            {0, 0, 0}
        );
        if(!geometry3d_close_value(extreme[0], 0.5L)
            || !geometry3d_close_value(extreme[1], 0.5L)
            || !geometry3d_close_value(extreme[2], 0.0L)){
            return false;
        }

        const long double far = maximum / 8;
        const auto far_triangle = barycentric_coordinates(
            Triangle3{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}},
            {far, far, far}
        );
        if(!std::isfinite(far_triangle[0])
            || !geometry3d_close_value(far_triangle[1] / far, 1.0L, 1e-18L)
            || !geometry3d_close_value(far_triangle[2] / far, 1.0L, 1e-18L)){
            return false;
        }
        const auto far_tetrahedron = barycentric_coordinates(
            Tetrahedron3{
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
            },
            {far, far, far}
        );
        if(!std::isfinite(far_tetrahedron[0])
            || !geometry3d_close_value(
                far_tetrahedron[1] / far, 1.0L, 1e-18L
            ) || !geometry3d_close_value(
                far_tetrahedron[2] / far, 1.0L, 1e-18L
            ) || !geometry3d_close_value(
                far_tetrahedron[3] / far, 1.0L, 1e-18L
            )){
            return false;
        }

        const long double translation = 1e2000L;
        const long double ulp = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const long double edge = 128 * ulp;
        const auto translated_triangle = barycentric_coordinates(
            Triangle3{
                {translation, translation, translation},
                {translation + edge, translation, translation},
                {translation, translation + edge, translation},
            },
            {
                translation + edge / 4,
                translation + edge / 4,
                translation + edge,
            }
        );
        if(!geometry3d_close_value(translated_triangle[0], 0.5L, 1e-5L)
            || !geometry3d_close_value(translated_triangle[1], 0.25L, 1e-5L)
            || !geometry3d_close_value(translated_triangle[2], 0.25L, 1e-5L)){
            return false;
        }
        const auto translated_tetrahedron = barycentric_coordinates(
            Tetrahedron3{
                {translation, translation, translation},
                {translation + edge, translation, translation},
                {translation, translation + edge, translation},
                {translation, translation, translation + edge},
            },
            {
                translation + edge / 8,
                translation + edge / 4,
                translation + edge / 2,
            }
        );
        if(!geometry3d_close_value(translated_tetrahedron[0], 0.125L, 1e-5L)
            || !geometry3d_close_value(translated_tetrahedron[1], 0.125L, 1e-5L)
            || !geometry3d_close_value(translated_tetrahedron[2], 0.25L, 1e-5L)
            || !geometry3d_close_value(translated_tetrahedron[3], 0.5L, 1e-5L)){
            return false;
        }

        const long double nan = std::numeric_limits<long double>::quiet_NaN();
        const long double infinity =
            std::numeric_limits<long double>::infinity();
        return throws_invalid_argument([](){
            (void)barycentric_coordinates(
                Triangle3{{0, 0, 0}, {1, 0, 0}, {2, 0, 0}}, {0, 0, 0}
            );
        }) && throws_invalid_argument([](){
            (void)barycentric_coordinates(
                Tetrahedron3{
                    {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0},
                },
                {0, 0, 0}
            );
        }) && throws_invalid_argument([nan](){
            (void)barycentric_coordinates(
                Triangle3{{nan, 0, 0}, {1, 0, 0}, {0, 1, 0}}, {0, 0, 0}
            );
        }) && throws_invalid_argument([infinity](){
            (void)barycentric_coordinates(
                Tetrahedron3{
                    {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
                },
                {infinity, 0, 0}
            );
        });
    });
}
