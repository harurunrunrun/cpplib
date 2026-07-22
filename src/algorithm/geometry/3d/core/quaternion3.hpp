#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_QUATERNION3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_QUATERNION3_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

struct Quaternion3{
    long double w = 0.0L;
    long double x = 0.0L;
    long double y = 0.0L;
    long double z = 0.0L;

    static constexpr Quaternion3 identity(){ return {1, 0, 0, 0}; }

    bool is_finite() const noexcept{
        return std::isfinite(w) && std::isfinite(x)
            && std::isfinite(y) && std::isfinite(z);
    }

    Quaternion3 operator+() const{ return *this; }
    Quaternion3 operator-() const{ return {-w, -x, -y, -z}; }

    Quaternion3& operator+=(const Quaternion3& other){
        *this = *this + other;
        return *this;
    }

    Quaternion3& operator-=(const Quaternion3& other){
        *this = *this - other;
        return *this;
    }

    Quaternion3& operator*=(const Quaternion3& other){
        *this = *this * other;
        return *this;
    }

    Quaternion3& operator*=(long double scalar){
        *this = *this * scalar;
        return *this;
    }

    Quaternion3& operator/=(long double scalar){
        *this = *this / scalar;
        return *this;
    }

    Quaternion3 conjugate() const{
        require_finite(*this, "non-finite quaternion conjugate");
        return {w, -x, -y, -z};
    }

    long double norm() const{
        require_finite(*this, "non-finite quaternion norm");
        const long double scale = std::max({
            std::abs(w), std::abs(x), std::abs(y), std::abs(z)
        });
        if(scale == 0.0L) return 0.0L;
        const long double sw = w / scale;
        const long double sx = x / scale;
        const long double sy = y / scale;
        const long double sz = z / scale;
        const long double result = scale * std::sqrt(
            sw * sw + sx * sx + sy * sy + sz * sz
        );
        if(!std::isfinite(result))[[unlikely]]{
            throw std::overflow_error("quaternion norm overflow");
        }
        return result;
    }

    Quaternion3 normalized() const{
        require_finite(*this, "non-finite quaternion normalization");
        const long double scale = std::max({
            std::abs(w), std::abs(x), std::abs(y), std::abs(z)
        });
        if(scale == 0.0L)[[unlikely]]{
            throw std::invalid_argument("normalization of zero quaternion");
        }
        const Quaternion3 scaled{w / scale, x / scale, y / scale, z / scale};
        const long double length = std::sqrt(
            scaled.w * scaled.w + scaled.x * scaled.x
                + scaled.y * scaled.y + scaled.z * scaled.z
        );
        return scaled / length;
    }

    Quaternion3 inverse() const{
        require_finite(*this, "non-finite quaternion inverse");
        const long double scale = std::max({
            std::abs(w), std::abs(x), std::abs(y), std::abs(z)
        });
        if(scale == 0.0L)[[unlikely]]{
            throw std::invalid_argument("inverse of zero quaternion");
        }
        const Quaternion3 scaled{w / scale, x / scale, y / scale, z / scale};
        const long double denominator = scaled.w * scaled.w + scaled.x * scaled.x
            + scaled.y * scaled.y + scaled.z * scaled.z;
        return checked({
            scaled.w / denominator / scale,
            -scaled.x / denominator / scale,
            -scaled.y / denominator / scale,
            -scaled.z / denominator / scale,
        }, "quaternion inverse overflow");
    }

    friend Quaternion3 operator+(const Quaternion3& first, const Quaternion3& second){
        require_finite(first, "non-finite quaternion addition");
        require_finite(second, "non-finite quaternion addition");
        return checked({
            first.w + second.w, first.x + second.x,
            first.y + second.y, first.z + second.z,
        }, "quaternion addition overflow");
    }

    friend Quaternion3 operator-(const Quaternion3& first, const Quaternion3& second){
        require_finite(first, "non-finite quaternion subtraction");
        require_finite(second, "non-finite quaternion subtraction");
        return checked({
            first.w - second.w, first.x - second.x,
            first.y - second.y, first.z - second.z,
        }, "quaternion subtraction overflow");
    }

    friend Quaternion3 operator*(const Quaternion3& first, const Quaternion3& second){
        require_finite(first, "non-finite quaternion multiplication");
        require_finite(second, "non-finite quaternion multiplication");
        return checked({
            first.w * second.w - first.x * second.x
                - first.y * second.y - first.z * second.z,
            first.w * second.x + first.x * second.w
                + first.y * second.z - first.z * second.y,
            first.w * second.y - first.x * second.z
                + first.y * second.w + first.z * second.x,
            first.w * second.z + first.x * second.y
                - first.y * second.x + first.z * second.w,
        }, "quaternion multiplication overflow");
    }

    friend Quaternion3 operator*(const Quaternion3& quaternion, long double scalar){
        require_finite(quaternion, "non-finite quaternion scaling");
        if(!std::isfinite(scalar))[[unlikely]]{
            throw std::invalid_argument("non-finite quaternion scalar");
        }
        return checked({
            quaternion.w * scalar, quaternion.x * scalar,
            quaternion.y * scalar, quaternion.z * scalar,
        }, "quaternion scaling overflow");
    }

    friend Quaternion3 operator*(long double scalar, const Quaternion3& quaternion){
        return quaternion * scalar;
    }

    friend Quaternion3 operator/(const Quaternion3& quaternion, long double scalar){
        require_finite(quaternion, "non-finite quaternion division");
        if(!std::isfinite(scalar) || scalar == 0.0L)[[unlikely]]{
            throw std::invalid_argument("invalid quaternion divisor");
        }
        return checked({
            quaternion.w / scalar, quaternion.x / scalar,
            quaternion.y / scalar, quaternion.z / scalar,
        }, "quaternion division overflow");
    }

private:
    static void require_finite(const Quaternion3& value, const char* message){
        if(!value.is_finite())[[unlikely]] throw std::invalid_argument(message);
    }

    static Quaternion3 checked(Quaternion3 value, const char* message){
        if(!value.is_finite())[[unlikely]] throw std::overflow_error(message);
        return value;
    }
};

inline long double quaternion3_dot(const Quaternion3& first, const Quaternion3& second){
    if(!first.is_finite() || !second.is_finite())[[unlikely]]{
        throw std::invalid_argument("non-finite quaternion dot product");
    }
    const long double result = first.w * second.w + first.x * second.x
        + first.y * second.y + first.z * second.z;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("quaternion dot product overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_QUATERNION3_HPP_INCLUDED
