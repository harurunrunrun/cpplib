#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CONVOLUTION_GF2_64_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CONVOLUTION_GF2_64_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif

class Gf2_64{
    using Wide = unsigned __int128;

    std::uint64_t value_ = 0;

    static Wide portable_carryless_multiply(
        std::uint64_t first,
        std::uint64_t second
    ){
        Wide result = 0;
        while(first != 0){
            const unsigned bit =
                static_cast<unsigned>(__builtin_ctzll(first));
            result ^= static_cast<Wide>(second) << bit;
            first &= first - 1;
        }
        return result;
    }

#if defined(__x86_64__) || defined(__i386__)
    __attribute__((target("pclmul,sse2")))
    static Wide pclmul_carryless_multiply(
        std::uint64_t first,
        std::uint64_t second
    ){
        const __m128i product = _mm_clmulepi64_si128(
            _mm_cvtsi64_si128(static_cast<long long>(first)),
            _mm_cvtsi64_si128(static_cast<long long>(second)),
            0
        );
        alignas(16) std::array<std::uint64_t, 2> words{};
        _mm_storeu_si128(
            reinterpret_cast<__m128i*>(words.data()),
            product
        );
        return static_cast<Wide>(words[0])
            | (static_cast<Wide>(words[1]) << 64);
    }
#endif

    static Wide carryless_multiply(
        std::uint64_t first,
        std::uint64_t second
    ){
#if defined(__x86_64__) || defined(__i386__)
        static const bool has_pclmul = []{
            __builtin_cpu_init();
            return __builtin_cpu_supports("pclmul") != 0;
        }();
        if(has_pclmul){
            return pclmul_carryless_multiply(first, second);
        }
#endif
        return portable_carryless_multiply(first, second);
    }

    static std::uint64_t reduce(Wide polynomial){
        static constexpr std::uint64_t remainder = 0x1B;
        const std::uint64_t low =
            static_cast<std::uint64_t>(polynomial);
        const std::uint64_t high =
            static_cast<std::uint64_t>(polynomial >> 64);
        const Wide first_fold =
            carryless_multiply(high, remainder);
        const std::uint64_t folded_low =
            low ^ static_cast<std::uint64_t>(first_fold);
        const std::uint64_t overflow =
            static_cast<std::uint64_t>(first_fold >> 64);
        return folded_low ^ static_cast<std::uint64_t>(
            carryless_multiply(overflow, remainder)
        );
    }

public:
    Gf2_64() = default;

    explicit Gf2_64(std::uint64_t value) : value_(value) {}

    std::uint64_t value() const{
        return value_;
    }

    Gf2_64 pow(std::uint64_t exponent) const{
        Gf2_64 result{1};
        Gf2_64 base = *this;
        while(exponent > 0){
            if(exponent & 1U) result *= base;
            base *= base;
            exponent >>= 1;
        }
        return result;
    }

    Gf2_64 inverse() const{
        if(value_ == 0)[[unlikely]]{
            throw std::domain_error("zero has no inverse in GF(2^64)");
        }
        return pow(~std::uint64_t{0} - 1);
    }

    Gf2_64& operator+=(const Gf2_64& other){
        value_ ^= other.value_;
        return *this;
    }

    Gf2_64& operator-=(const Gf2_64& other){
        value_ ^= other.value_;
        return *this;
    }

    Gf2_64& operator*=(const Gf2_64& other){
        value_ = reduce(carryless_multiply(value_, other.value_));
        return *this;
    }

    friend Gf2_64 operator+(Gf2_64 first, const Gf2_64& second){
        return first += second;
    }

    friend Gf2_64 operator-(Gf2_64 first, const Gf2_64& second){
        return first -= second;
    }

    friend Gf2_64 operator*(Gf2_64 first, const Gf2_64& second){
        return first *= second;
    }

    Gf2_64 operator-() const{
        return *this;
    }

    friend bool operator==(const Gf2_64&, const Gf2_64&) = default;
};

inline constexpr std::size_t gf2_64_convolution_max_input_size = 524288;

namespace gf2_64_convolution_detail{

inline constexpr std::array<std::size_t, 14> powers_of_three{
    1,
    3,
    9,
    27,
    81,
    243,
    729,
    2187,
    6561,
    19683,
    59049,
    177147,
    531441,
    1594323,
};

struct QuadraticExtension{
    Gf2_64 constant;
    Gf2_64 linear;

    QuadraticExtension() = default;

    QuadraticExtension(Gf2_64 constant_value, Gf2_64 linear_value)
        : constant(constant_value), linear(linear_value) {}

    QuadraticExtension& operator+=(const QuadraticExtension& other){
        constant += other.constant;
        linear += other.linear;
        return *this;
    }

    QuadraticExtension& operator-=(const QuadraticExtension& other){
        return *this += other;
    }

    QuadraticExtension& operator*=(const QuadraticExtension& other){
        const Gf2_64 constant_product = constant * other.constant;
        const Gf2_64 linear_product = linear * other.linear;
        const Gf2_64 mixed =
            (constant + linear) * (other.constant + other.linear);
        constant = constant_product + linear_product;
        linear = mixed + constant_product;
        return *this;
    }

    friend QuadraticExtension operator+(
        QuadraticExtension first,
        const QuadraticExtension& second
    ){
        return first += second;
    }

    friend QuadraticExtension operator-(
        QuadraticExtension first,
        const QuadraticExtension& second
    ){
        return first -= second;
    }

    friend QuadraticExtension operator*(
        QuadraticExtension first,
        const QuadraticExtension& second
    ){
        return first *= second;
    }

    QuadraticExtension multiply_by_root() const{
        return {linear, constant + linear};
    }

    QuadraticExtension multiply_by_root_squared() const{
        return {constant + linear, constant};
    }

    QuadraticExtension conjugate() const{
        return {constant + linear, linear};
    }
};

class SchonhageMultiplier{
    using Extension = QuadraticExtension;

    std::array<std::vector<Extension>, powers_of_three.size()> workspace_;

    static void multiply_by_root_power(
        std::span<Extension> values,
        unsigned power
    ){
        power %= 3;
        if(power == 1){
            for(Extension& value: values){
                value = value.multiply_by_root();
            }
        }else if(power == 2){
            for(Extension& value: values){
                value = value.multiply_by_root_squared();
            }
        }
    }

    template<std::size_t BlockLog, bool Inverse>
    static void ternary_butterfly(
        std::size_t phase,
        std::span<Extension> first,
        std::span<Extension> second,
        std::span<Extension> third
    ){
        constexpr std::size_t block_size = powers_of_three[BlockLog];
        const std::size_t first_shift = phase % block_size;
        const unsigned first_twist =
            static_cast<unsigned>(phase / block_size);
        const std::size_t second_shift =
            (2 * phase) % block_size;
        const unsigned second_twist = static_cast<unsigned>(
            (2 * phase) / block_size % 3
        );

        if constexpr(!Inverse){
            std::rotate(
                second.begin(),
                second.end() - static_cast<std::ptrdiff_t>(first_shift),
                second.end()
            );
            std::rotate(
                third.begin(),
                third.end() - static_cast<std::ptrdiff_t>(second_shift),
                third.end()
            );
            multiply_by_root_power(
                second.first(first_shift),
                1 + first_twist
            );
            multiply_by_root_power(
                third.first(second_shift),
                1 + second_twist
            );
            multiply_by_root_power(
                second.subspan(first_shift),
                first_twist
            );
            multiply_by_root_power(
                third.subspan(second_shift),
                second_twist
            );
        }

        for(std::size_t index = 0; index < block_size; ++index){
            const Extension x = first[index];
            const Extension y = second[index];
            const Extension z = third[index];
            if constexpr(Inverse){
                first[index] = x + y + z;
                second[index] = x
                    + y.multiply_by_root_squared()
                    + z.multiply_by_root();
                third[index] = x
                    + y.multiply_by_root()
                    + z.multiply_by_root_squared();
            }else{
                first[index] = x + y + z;
                second[index] = x
                    + y.multiply_by_root()
                    + z.multiply_by_root_squared();
                third[index] = x
                    + y.multiply_by_root_squared()
                    + z.multiply_by_root();
            }
        }

        if constexpr(Inverse){
            multiply_by_root_power(
                second.first(first_shift),
                5U - first_twist
            );
            multiply_by_root_power(
                third.first(second_shift),
                5U - second_twist
            );
            multiply_by_root_power(
                second.subspan(first_shift),
                3U - first_twist
            );
            multiply_by_root_power(
                third.subspan(second_shift),
                3U - second_twist
            );
            std::rotate(
                second.begin(),
                second.begin()
                    + static_cast<std::ptrdiff_t>(first_shift),
                second.end()
            );
            std::rotate(
                third.begin(),
                third.begin()
                    + static_cast<std::ptrdiff_t>(second_shift),
                third.end()
            );
        }
    }

    template<std::size_t TotalLog, std::size_t BlockLog, bool Inverse>
    static void transform(
        std::span<Extension> values,
        bool conjugate_phase = false
    ){
        const auto recurse = [&](
            auto&& self,
            std::size_t remaining_log,
            std::size_t offset,
            std::size_t phase
        ) -> void {
            const std::size_t third_size =
                powers_of_three[remaining_log];
            constexpr std::size_t block_size =
                powers_of_three[BlockLog];
            const auto descend = [&]{
                for(std::size_t part = 0; part < 3; ++part){
                    self(
                        self,
                        remaining_log - 1,
                        offset + third_size * part,
                        (
                            phase / 3
                            + powers_of_three[BlockLog] * part
                        ) % powers_of_three[BlockLog + 1]
                    );
                }
            };
            if constexpr(Inverse){
                if(remaining_log >= BlockLog + 1) descend();
            }
            for(std::size_t inside = 0;
                inside < third_size;
                inside += block_size){
                ternary_butterfly<BlockLog, Inverse>(
                    phase / 3,
                    values.subspan(offset + inside, block_size),
                    values.subspan(
                        offset + inside + third_size,
                        block_size
                    ),
                    values.subspan(
                        offset + inside + 2 * third_size,
                        block_size
                    )
                );
            }
            if constexpr(!Inverse){
                if(remaining_log >= BlockLog + 1) descend();
            }
        };
        recurse(
            recurse,
            TotalLog - 1,
            0,
            (conjugate_phase ? 2U : 1U)
                * powers_of_three[BlockLog]
        );
    }

    std::array<std::span<Extension>, 2> acquire_workspace(
        std::size_t logarithm
    ){
        const std::size_t size = powers_of_three[logarithm];
        auto& storage = workspace_[logarithm];
        storage.assign(2 * size, Extension{});
        return {
            std::span<Extension>(storage).first(size),
            std::span<Extension>(storage).subspan(size, size),
        };
    }

    template<std::size_t Logarithm>
    static void base_cyclic_product(
        std::span<Extension> first,
        std::span<const Extension> second
    ){
        constexpr std::size_t size = powers_of_three[Logarithm];
        std::array<Extension, size> result{};
        for(std::size_t left = 0; left < size; ++left){
            for(std::size_t right = 0; right < size; ++right){
                Extension product = first[left] * second[right];
                const std::size_t degree = left + right;
                if(degree < size){
                    result[degree] += product;
                }else{
                    result[degree - size] +=
                        product.multiply_by_root();
                }
            }
        }
        std::copy(result.begin(), result.end(), first.begin());
    }

    template<std::size_t Logarithm>
    void cyclic_product(
        std::span<Extension> first,
        std::span<Extension> second
    ){
        constexpr std::size_t size = powers_of_three[Logarithm];
        if constexpr(Logarithm <= 3){
            base_cyclic_product<Logarithm>(first, second);
            return;
        }else{
            constexpr std::size_t block_log = (Logarithm + 1) / 2;
            constexpr std::size_t block_size =
                powers_of_three[block_log];
            auto [conjugate_first, conjugate_second] =
                acquire_workspace(Logarithm);
            for(std::size_t index = 0; index < size; ++index){
                conjugate_first[index] = first[index].conjugate();
                conjugate_second[index] = second[index].conjugate();
            }

            transform<Logarithm, block_log, false>(first);
            transform<Logarithm, block_log, false>(second);
            transform<Logarithm, block_log, false>(
                conjugate_first,
                true
            );
            transform<Logarithm, block_log, false>(
                conjugate_second,
                true
            );

            for(std::size_t offset = 0;
                offset < size;
                offset += block_size){
                cyclic_product<block_log>(
                    second.subspan(offset, block_size),
                    first.subspan(offset, block_size)
                );
                cyclic_product<block_log>(
                    conjugate_second.subspan(offset, block_size),
                    conjugate_first.subspan(offset, block_size)
                );
            }

            transform<Logarithm, block_log, true>(second);
            transform<Logarithm, block_log, true>(
                conjugate_second,
                true
            );
            for(Extension& value: conjugate_second){
                value = value.conjugate();
            }

            std::fill(first.begin(), first.end(), Extension{});
            for(std::size_t block = 0; block < size; block += block_size){
                for(std::size_t offset = 0;
                    offset < block_size;
                    ++offset){
                    const Extension first_half =
                        second[block + offset]
                            .multiply_by_root_squared()
                        + conjugate_second[block + offset]
                            .multiply_by_root();
                    const Extension second_half =
                        second[block + offset]
                        + conjugate_second[block + offset];
                    first[block + offset] += first_half;
                    const std::size_t destination =
                        block + offset + block_size;
                    if(destination < size){
                        first[destination] += second_half;
                    }else{
                        first[destination - size] +=
                            second_half.multiply_by_root();
                    }
                }
            }
        }
    }

    template<std::size_t Logarithm = 0>
    void dispatch(
        std::span<Extension> first,
        std::span<Extension> second
    ){
        if(first.size() == powers_of_three[Logarithm]){
            cyclic_product<Logarithm>(first, second);
        }else{
            if constexpr(Logarithm + 1 < powers_of_three.size()){
                dispatch<Logarithm + 1>(first, second);
            }else{
                throw std::length_error(
                    "GF(2^64) convolution transform is too large"
                );
            }
        }
    }

public:
    std::vector<Gf2_64> multiply(
        const std::vector<Gf2_64>& first,
        const std::vector<Gf2_64>& second
    ){
        if(first.empty() || second.empty()) return {};
        const std::size_t result_size =
            first.size() + second.size() - 1;
        std::size_t logarithm = 0;
        while(
            2 * powers_of_three[logarithm] < result_size
        ){
            ++logarithm;
            if(logarithm >= powers_of_three.size())[[unlikely]]{
                throw std::length_error(
                    "GF(2^64) convolution transform is too large"
                );
            }
        }
        const std::size_t transform_size =
            powers_of_three[logarithm];

        if(first.size() <= transform_size
            && second.size() <= transform_size
            && first.size() * second.size()
                <= transform_size * (logarithm + 1) * 8){
            std::vector<Gf2_64> result(result_size);
            for(std::size_t left = 0; left < first.size(); ++left){
                for(std::size_t right = 0;
                    right < second.size();
                    ++right){
                    result[left + right] +=
                        first[left] * second[right];
                }
            }
            return result;
        }

        std::vector<Extension> left(transform_size);
        std::vector<Extension> right(transform_size);
        for(std::size_t index = 0; index < first.size(); ++index){
            if(index < transform_size){
                left[index].constant = first[index];
            }else{
                left[index - transform_size].linear = first[index];
            }
        }
        for(std::size_t index = 0; index < second.size(); ++index){
            if(index < transform_size){
                right[index].constant = second[index];
            }else{
                right[index - transform_size].linear = second[index];
            }
        }
        dispatch(
            std::span<Extension>(left),
            std::span<Extension>(right)
        );

        std::vector<Gf2_64> result(result_size);
        for(std::size_t index = 0; index < result_size; ++index){
            result[index] = index < transform_size
                ? left[index].constant
                : left[index - transform_size].linear;
        }
        return result;
    }
};

} // namespace gf2_64_convolution_detail

inline std::vector<std::uint64_t> gf2_64_convolution(
    const std::vector<std::uint64_t>& first,
    const std::vector<std::uint64_t>& second
){
    if(first.empty() || second.empty()) return {};
    if(first.size() > gf2_64_convolution_max_input_size
        || second.size() > gf2_64_convolution_max_input_size)[[unlikely]]{
        throw std::length_error(
            "GF(2^64) convolution input exceeds the supported size"
        );
    }
    std::vector<Gf2_64> left(first.size()), right(second.size());
    for(std::size_t index = 0; index < first.size(); ++index){
        left[index] = Gf2_64{first[index]};
    }
    for(std::size_t index = 0; index < second.size(); ++index){
        right[index] = Gf2_64{second[index]};
    }
    gf2_64_convolution_detail::SchonhageMultiplier multiplier;
    const std::vector<Gf2_64> product =
        multiplier.multiply(left, right);
    std::vector<std::uint64_t> result(product.size());
    for(std::size_t index = 0; index < product.size(); ++index){
        result[index] = product[index].value();
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CONVOLUTION_GF2_64_HPP_INCLUDED
