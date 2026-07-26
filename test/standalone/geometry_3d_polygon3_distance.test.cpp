// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/polygon3_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

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
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{0, 0, 0}, {4, 0, 0}, {4, 2, 0}, {0, 2, 0}};
        if(!geometry3d_api_close(polygon3_distance(polygon, {2, 1, 3}), 3)
            || !geometry3d_api_close(polygon3_distance(polygon, {5, 1, 0}), 1)){
            return false;
        }

        const Polygon3 concave{
            {0, 0, 0}, {4, 0, 0}, {4, 4, 0}, {3, 4, 0},
            {3, 1, 0}, {1, 1, 0}, {1, 4, 0}, {0, 4, 0},
        };
        if(!geometry3d_api_close(
            polygon3_distance(concave, {2, 3, 2}), std::sqrt(5.0L)
        ) || !geometry3d_api_close(
            polygon3_distance(concave, {0.5L, 3, 2}), 2.0L
        )) return false;

        for(const long double scale: {1e-2000L, 1e2000L}){
            const Polygon3 scaled{
                {0, 0, 0},
                {4 * scale, 0, 0},
                {4 * scale, 2 * scale, 0},
                {0, 2 * scale, 0},
            };
            if(!geometry3d_api_close(
                polygon3_distance(
                    scaled, {2 * scale, scale, 3 * scale}
                ) / scale,
                3.0L,
                1e-8L
            )) return false;
        }

        const long double absolute_translation = 1e3000L;
        const long double absolute_local = 1e-3000L;
        const Polygon3 absolute_scale_polygon{
            {0, 0, absolute_translation},
            {absolute_local, 0, absolute_translation},
            {0, absolute_local, absolute_translation},
        };
        if(!geometry3d_api_close(
            polygon3_distance(
                absolute_scale_polygon,
                {2 * absolute_local, 0, absolute_translation}
            ) / absolute_local, 1.0L
        )) return false;

        const long double maximum = std::numeric_limits<long double>::max();
        const long double near_maximum = maximum / 16;
        const Polygon3 extreme{
            {0, 0, 0},
            {2 * near_maximum, 0, 0},
            {2 * near_maximum, 2 * near_maximum, 0},
            {0, 2 * near_maximum, 0},
        };
        if(!geometry3d_api_close(
            polygon3_distance(
                extreme, {near_maximum, near_maximum, 3 * near_maximum}
            ) / near_maximum,
            3.0L,
            1e-8L
        )) return false;

        const long double translation = 1e2000L;
        const long double ulp = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const long double edge = 128 * ulp;
        const Polygon3 translated{
            {translation, translation, translation},
            {translation + edge, translation, translation},
            {translation + edge, translation + edge, translation},
            {translation, translation + edge, translation},
        };
        if(!geometry3d_api_close(
            polygon3_distance(translated, {
                translation + edge / 2,
                translation + edge / 2,
                translation + edge / 4,
            }) / edge,
            0.25L,
            1e-5L
        )) return false;

        const long double subnormal =
            std::numeric_limits<long double>::denorm_min();
        if(subnormal > 0.0L){
            const Polygon3 mixed_scale_basis{
                {0, 0, 0},
                {subnormal, 0, 0},
                {1, 0, 0},
                {0, 0.5L, 0},
            };
            if(!geometry3d_api_close(
                polygon3_distance(mixed_scale_basis, {0.25L, 0.1L, 1}),
                1.0L
            )) return false;
        }

        Polygon3 long_collinear_prefix(512, Point3{0, 0, 0});
        for(std::size_t index = 1; index <= 512; ++index){
            long_collinear_prefix.push_back({
                static_cast<long double>(index), 0, 0
            });
        }
        long_collinear_prefix.push_back({512, 4, 0});
        long_collinear_prefix.push_back({0, 4, 0});
        if(!geometry3d_api_close(
            polygon3_distance(
                long_collinear_prefix, {256, 2, 3}
            ),
            3.0L
        )) return false;

        Polygon3 collinear;
        collinear.reserve(1024);
        for(std::size_t index = 0; index < 1024; ++index){
            collinear.push_back({
                static_cast<long double>(index),
                static_cast<long double>(index) * 2,
                static_cast<long double>(index) * -3,
            });
        }
        if(!throws_invalid_argument([&collinear](){
            (void)polygon3_distance(collinear, {0, 1, 0});
        })) return false;

        const long double nan = std::numeric_limits<long double>::quiet_NaN();
        const long double infinity =
            std::numeric_limits<long double>::infinity();
        return throws_invalid_argument([](){
            (void)polygon3_distance({}, {0, 0, 0});
        }) && throws_invalid_argument([](){
            (void)polygon3_distance({{0, 0, 0}, {1, 0, 0}}, {0, 0, 0});
        }) && throws_invalid_argument([](){
            (void)polygon3_distance(
                {{0, 0, 0}, {1, 0, 0}, {2, 0, 0}}, {0, 1, 0}
            );
        }) && throws_invalid_argument([](){
            (void)polygon3_distance(
                {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 1}},
                {0, 0, 0}
            );
        }) && throws_invalid_argument([nan](){
            (void)polygon3_distance(
                {{0, 0, 0}, {1, 0, 0}, {0, nan, 0}}, {0, 0, 0}
            );
        }) && throws_invalid_argument([infinity, &polygon](){
            (void)polygon3_distance(polygon, {infinity, 0, 0});
        });
    });
}
