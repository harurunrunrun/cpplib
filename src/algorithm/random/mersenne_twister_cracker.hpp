#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_MERSENNE_TWISTER_CRACKER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_MERSENNE_TWISTER_CRACKER_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

template<
    class UInt,
    std::size_t WordSize,
    std::size_t StateSize,
    std::size_t ShiftSize,
    std::size_t MaskBits,
    UInt XorMask,
    std::size_t TemperingU,
    UInt TemperingD,
    std::size_t TemperingS,
    UInt TemperingB,
    std::size_t TemperingT,
    UInt TemperingC,
    std::size_t TemperingL
>
class MersenneTwisterCracker{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(0 < WordSize);
    static_assert(WordSize <= std::numeric_limits<UInt>::digits);
    static_assert(0 < StateSize);
    static_assert(0 < ShiftSize && ShiftSize <= StateSize);
    static_assert(MaskBits <= WordSize);
    static_assert(0 < TemperingU && TemperingU <= WordSize);
    static_assert(0 < TemperingS && TemperingS <= WordSize);
    static_assert(0 < TemperingT && TemperingT <= WordSize);
    static_assert(0 < TemperingL && TemperingL <= WordSize);

public:
    using result_type = UInt;
    using state_type = std::array<result_type, StateSize>;

private:
    state_type state_{};
    std::size_t head_ = 0;

    static constexpr result_type word_mask() noexcept{
        if constexpr(WordSize == std::numeric_limits<result_type>::digits){
            return std::numeric_limits<result_type>::max();
        }else{
            return static_cast<result_type>(
                (result_type(1) << WordSize) - 1
            );
        }
    }

    static constexpr bool valid_word(result_type value) noexcept{
        return (value & static_cast<result_type>(~word_mask())) == 0;
    }

    static constexpr result_type low_mask() noexcept{
        if constexpr(MaskBits == 0){
            return 0;
        }else if constexpr(MaskBits == WordSize){
            return word_mask();
        }else{
            return static_cast<result_type>(
                (result_type(1) << MaskBits) - 1
            );
        }
    }

    static constexpr result_type shift_right(
        result_type value,
        std::size_t shift
    ) noexcept{
        if(shift >= std::numeric_limits<result_type>::digits) return 0;
        return static_cast<result_type>(value >> shift);
    }

    static constexpr result_type shift_left(
        result_type value,
        std::size_t shift
    ) noexcept{
        if(shift >= std::numeric_limits<result_type>::digits) return 0;
        return static_cast<result_type>(value << shift);
    }

    static constexpr result_type undo_right(
        result_type value,
        std::size_t shift,
        result_type mask
    ) noexcept{
        result_type result = value & word_mask();
        for(std::size_t covered = shift; covered < WordSize; covered += shift){
            result = static_cast<result_type>(
                value ^ (shift_right(result, shift) & mask)
            ) & word_mask();
        }
        return result;
    }

    static constexpr result_type undo_left(
        result_type value,
        std::size_t shift,
        result_type mask
    ) noexcept{
        result_type result = value & word_mask();
        for(std::size_t covered = shift; covered < WordSize; covered += shift){
            result = static_cast<result_type>(
                value ^ (shift_left(result, shift) & mask)
            ) & word_mask();
        }
        return result;
    }

    static constexpr result_type untemper(result_type value) noexcept{
        value = undo_right(value, TemperingL, word_mask());
        value = undo_left(value, TemperingT, TemperingC);
        value = undo_left(value, TemperingS, TemperingB);
        return undo_right(value, TemperingU, TemperingD);
    }

    static constexpr result_type temper(result_type value) noexcept{
        value = static_cast<result_type>(
            value ^ (shift_right(value, TemperingU) & TemperingD)
        ) & word_mask();
        value = static_cast<result_type>(
            value ^ (shift_left(value, TemperingS) & TemperingB)
        ) & word_mask();
        value = static_cast<result_type>(
            value ^ (shift_left(value, TemperingT) & TemperingC)
        ) & word_mask();
        value = static_cast<result_type>(
            value ^ shift_right(value, TemperingL)
        );
        return value & word_mask();
    }

public:
    template<std::size_t Count>
    constexpr explicit MersenneTwisterCracker(
        const std::array<result_type, Count>& consecutive_outputs
    ){
        static_assert(Count >= StateSize);
        for(result_type value: consecutive_outputs){
            if(!valid_word(value)){
                throw std::invalid_argument(
                    "Mersenne Twister output exceeds word size"
                );
            }
        }
        for(std::size_t index = 0; index < StateSize; ++index){
            state_[index] = untemper(consecutive_outputs[index]);
        }
        for(std::size_t index = StateSize; index < Count; ++index){
            if(next() != consecutive_outputs[index]){
                throw std::invalid_argument(
                    "outputs do not belong to one Mersenne Twister stream"
                );
            }
        }
    }

    constexpr result_type operator()() noexcept{
        return next();
    }

    constexpr result_type next() noexcept{
        std::size_t next_index = head_ + 1;
        if(next_index == StateSize) next_index = 0;
        std::size_t shifted_index = head_ + ShiftSize;
        if(shifted_index >= StateSize){
            shifted_index -= StateSize;
        }
        const result_type combined = static_cast<result_type>(
            (state_[head_] & static_cast<result_type>(~low_mask()))
            | (state_[next_index] & low_mask())
        ) & word_mask();
        result_type generated = static_cast<result_type>(
            state_[shifted_index] ^ (combined >> 1)
        );
        if((combined & result_type(1)) != 0) generated ^= XorMask;
        generated &= word_mask();
        state_[head_] = generated;
        head_ = next_index;
        return temper(generated);
    }

    constexpr void discard(unsigned long long count) noexcept{
        while(count-- != 0) (void)next();
    }

    constexpr state_type state() const noexcept{
        state_type result{};
        std::size_t position = head_;
        for(std::size_t index = 0; index < StateSize; ++index){
            result[index] = state_[position];
            ++position;
            if(position == StateSize) position = 0;
        }
        return result;
    }

    static constexpr result_type min() noexcept{
        return 0;
    }

    static constexpr result_type max() noexcept{
        return word_mask();
    }
};

using Mt19937Cracker = MersenneTwisterCracker<
    std::uint32_t,
    32,
    624,
    397,
    31,
    0x9908B0DFU,
    11,
    0xFFFFFFFFU,
    7,
    0x9D2C5680U,
    15,
    0xEFC60000U,
    18
>;

using Mt19937_64Cracker = MersenneTwisterCracker<
    std::uint64_t,
    64,
    312,
    156,
    31,
    0xB5026F5AA96619E9ULL,
    29,
    0x5555555555555555ULL,
    17,
    0x71D67FFFEDA60000ULL,
    37,
    0xFFF7EEE000000000ULL,
    43
>;

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_MERSENNE_TWISTER_CRACKER_HPP_INCLUDED
