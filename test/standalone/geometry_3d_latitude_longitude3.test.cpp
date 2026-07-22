// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/core/latitude_longitude3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!geometry3d_api_close(
                point3_from_latitude_longitude(0, GEOMETRY3D_PI / 2, 2), {0, 2, 0}
            ) || !geometry3d_api_close(
                point3_from_latitude_longitude(GEOMETRY3D_PI / 2, 123, 3), {0, 0, 3}
            )) return false;
        const LatitudeLongitude3 north = latitude_longitude_from_point3({0, 0, 8});
        if(!geometry3d_api_close(north.latitude, GEOMETRY3D_PI / 2)
            || north.longitude != 0 || !geometry3d_api_close(north.radius, 8)) return false;
        bool latitude_rejected = false;
        bool origin_rejected = false;
        try{ (void)point3_from_latitude_longitude(GEOMETRY3D_PI, 0); }
        catch(const std::invalid_argument&){ latitude_rejected = true; }
        try{ (void)latitude_longitude_from_point3({}); }
        catch(const std::invalid_argument&){ origin_rejected = true; }
        if(!latitude_rejected || !origin_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const long double latitude =
                (static_cast<long double>(random() % 1999) / 1998.0L - 0.5L) * GEOMETRY3D_PI;
            const long double longitude =
                (static_cast<long double>(random() % 4001) / 1000.0L - 2.0L) * GEOMETRY3D_PI;
            const long double radius = static_cast<long double>(random() % 1000 + 1) / 10.0L;
            const Point3 point = point3_from_latitude_longitude(latitude, longitude, radius);
            const LatitudeLongitude3 restored = latitude_longitude_from_point3(point);
            const Point3 roundtrip = point3_from_latitude_longitude(
                restored.latitude, restored.longitude, restored.radius
            );
            if(!geometry3d_api_close(point, roundtrip)) return false;
        }
        return true;
    });
}
