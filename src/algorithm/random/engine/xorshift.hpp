#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_ENGINE_XORSHIFT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_ENGINE_XORSHIFT_HPP_INCLUDED

#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>

class XorShift32{
public:
    using result_type = std::uint32_t;

    static constexpr result_type default_seed = 2463534242U;

private:
    result_type state_ = default_seed;

public:
    constexpr XorShift32() = default;

    constexpr explicit XorShift32(result_type seed_value){
        seed(seed_value);
    }

    constexpr void seed(result_type value = default_seed){
        if(value == 0)[[unlikely]]{
            throw std::invalid_argument("XorShift32 requires a nonzero seed");
        }
        state_ = value;
    }

    constexpr result_type operator()() noexcept{
        result_type value = state_;
        value ^= value << 13;
        value ^= value >> 17;
        value ^= value << 5;
        state_ = value;
        return value;
    }

    constexpr result_type state() const noexcept{
        return state_;
    }

    static constexpr result_type min() noexcept{
        return 1;
    }

    static constexpr result_type max() noexcept{
        return std::numeric_limits<result_type>::max();
    }
};

class XorShift64{
public:
    using result_type = std::uint64_t;

    static constexpr result_type default_seed = 88172645463325252ULL;

private:
    result_type state_ = default_seed;

public:
    constexpr XorShift64() = default;

    constexpr explicit XorShift64(result_type seed_value){
        seed(seed_value);
    }

    constexpr void seed(result_type value = default_seed){
        if(value == 0)[[unlikely]]{
            throw std::invalid_argument("XorShift64 requires a nonzero seed");
        }
        state_ = value;
    }

    constexpr result_type operator()() noexcept{
        result_type value = state_;
        value ^= value << 13;
        value ^= value >> 7;
        value ^= value << 17;
        state_ = value;
        return value;
    }

    constexpr result_type state() const noexcept{
        return state_;
    }

    static constexpr result_type min() noexcept{
        return 1;
    }

    static constexpr result_type max() noexcept{
        return std::numeric_limits<result_type>::max();
    }
};

class XorShift128{
public:
    using result_type = std::uint32_t;
    using state_type = std::array<result_type, 4>;

    static constexpr state_type default_seed{
        123456789U,
        362436069U,
        521288629U,
        88675123U,
    };

private:
    state_type state_ = default_seed;

    static constexpr bool all_zero(const state_type& value) noexcept{
        return value[0] == 0 && value[1] == 0
            && value[2] == 0 && value[3] == 0;
    }

public:
    constexpr XorShift128() = default;

    constexpr explicit XorShift128(const state_type& seed_value){
        seed(seed_value);
    }

    constexpr XorShift128(
        result_type x,
        result_type y,
        result_type z,
        result_type w
    ){
        seed({x, y, z, w});
    }

    constexpr void seed(const state_type& value = default_seed){
        if(all_zero(value))[[unlikely]]{
            throw std::invalid_argument(
                "XorShift128 requires a nonzero state"
            );
        }
        state_ = value;
    }

    constexpr result_type operator()() noexcept{
        const result_type temporary = state_[0] ^ (state_[0] << 11);
        state_[0] = state_[1];
        state_[1] = state_[2];
        state_[2] = state_[3];
        state_[3] = state_[3] ^ (state_[3] >> 19)
            ^ temporary ^ (temporary >> 8);
        return state_[3];
    }

    constexpr state_type state() const noexcept{
        return state_;
    }

    static constexpr result_type min() noexcept{
        return 0;
    }

    static constexpr result_type max() noexcept{
        return std::numeric_limits<result_type>::max();
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_ENGINE_XORSHIFT_HPP_INCLUDED
