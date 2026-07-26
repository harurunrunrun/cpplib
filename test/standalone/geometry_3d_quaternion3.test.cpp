// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>

#include "../../src/algorithm/geometry/3d/quaternion3.hpp"
#include "../../src/algorithm/geometry/3d/quaternion3_dot.hpp"
#include "../../src/algorithm/geometry/3d/quaternion3_type.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool close_quaternion(const Quaternion3& a, const Quaternion3& b){
    return geometry3d_api_close(a.w, b.w) && geometry3d_api_close(a.x, b.x)
        && geometry3d_api_close(a.y, b.y) && geometry3d_api_close(a.z, b.z);
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Quaternion3 identity = Quaternion3::identity();
        if(!identity.is_finite()
            || !close_quaternion(+identity, identity)
            || !close_quaternion(-(-identity), identity)
            || !close_quaternion(identity.conjugate(), identity)
            || !geometry3d_api_close(identity.norm(), 1.0L)) return false;
        bool zero_normalization_rejected = false;
        bool zero_inverse_rejected = false;
        bool zero_division_rejected = false;
        bool non_finite_rejected = false;
        bool inverse_overflow_rejected = false;
        try{ (void)Quaternion3{}.normalized(); }
        catch(const std::invalid_argument&){ zero_normalization_rejected = true; }
        try{ (void)Quaternion3{}.inverse(); }
        catch(const std::invalid_argument&){ zero_inverse_rejected = true; }
        try{ (void)(identity / 0.0L); }
        catch(const std::invalid_argument&){ zero_division_rejected = true; }
        try{
            (void)Quaternion3{
                std::numeric_limits<long double>::infinity(), 0, 0, 0
            }.norm();
        }catch(const std::invalid_argument&){ non_finite_rejected = true; }
        try{
            (void)Quaternion3{
                std::numeric_limits<long double>::denorm_min(), 0, 0, 0
            }.inverse();
        }catch(const std::overflow_error&){ inverse_overflow_rejected = true; }
        if(!zero_normalization_rejected || !zero_inverse_rejected
            || !zero_division_rejected || !non_finite_rejected
            || !inverse_overflow_rejected) return false;

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const auto sample = [&](){
                return static_cast<long double>(
                    static_cast<int>(random() % 2001) - 1000
                ) / 100.0L;
            };
            Quaternion3 a{sample(), sample(), sample(), sample()};
            Quaternion3 b{sample(), sample(), sample(), sample()};
            if(a.norm() == 0.0L) a.w = 1.0L;
            if(b.norm() == 0.0L) b.x = 1.0L;

            Quaternion3 add_assign = a;
            add_assign += b;
            Quaternion3 subtract_assign = a;
            subtract_assign -= b;
            Quaternion3 multiply_assign = a;
            multiply_assign *= b;
            Quaternion3 scale_assign = a;
            scale_assign *= 2.0L;
            scale_assign /= 2.0L;

            if(!geometry3d_api_close(a.normalized().norm(), 1.0L)
                || !close_quaternion(a * a.inverse(), identity)
                || !close_quaternion(
                    (a * b).conjugate(), b.conjugate() * a.conjugate()
                )
                || !close_quaternion(add_assign, a + b)
                || !close_quaternion(subtract_assign, a - b)
                || !close_quaternion(multiply_assign, a * b)
                || !close_quaternion(scale_assign, a)
                || !close_quaternion(a * 2.0L, 2.0L * a)
                || !close_quaternion((a * 2.0L) / 2.0L, a)
                || !geometry3d_api_close(
                    quaternion3_dot(a, b),
                    a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z
                )) return false;
        }
        const long double maximum = std::numeric_limits<long double>::max();
        const Quaternion3 huge{maximum, maximum, 0, 0};
        if(!geometry3d_api_close(huge.normalized().norm(), 1.0L)) return false;
        return true;
    });
}
