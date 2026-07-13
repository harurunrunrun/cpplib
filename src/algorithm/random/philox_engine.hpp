#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

template<
    class UInt,
    std::size_t WordSize,
    std::size_t WordCount,
    std::size_t RoundCount,
    UInt... Constants
>
class PhiloxEngine{
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(std::numeric_limits<UInt>::digits <= 64);
    static_assert(0 < WordSize);
    static_assert(WordSize <= std::numeric_limits<UInt>::digits);
    static_assert(WordCount == 2 || WordCount == 4);
    static_assert(0 < RoundCount);
    static_assert(sizeof...(Constants) == WordCount);

public:
    using result_type = UInt;
    using counter_type = std::array<result_type, WordCount>;
    using key_type = std::array<result_type, WordCount / 2>;
    using block_type = std::array<result_type, WordCount>;

    static constexpr std::size_t word_size = WordSize;
    static constexpr std::size_t word_count = WordCount;
    static constexpr std::size_t round_count = RoundCount;
    static constexpr result_type default_seed =
        static_cast<result_type>(20111115U);

private:
    using wide_type = std::conditional_t<
        (WordSize <= 32),
        std::uint64_t,
        unsigned __int128
    >;

    inline static constexpr std::array<result_type, WordCount>
        constants_{Constants...};

    static constexpr key_type make_multipliers() noexcept{
        key_type result{};
        for(std::size_t index = 0; index < WordCount / 2; ++index){
            result[index] = constants_[2 * index];
        }
        return result;
    }

    static constexpr key_type make_round_constants() noexcept{
        key_type result{};
        for(std::size_t index = 0; index < WordCount / 2; ++index){
            result[index] = constants_[2 * index + 1];
        }
        return result;
    }

public:
    inline static constexpr key_type multipliers = make_multipliers();
    inline static constexpr key_type round_consts =
        make_round_constants();

private:
    counter_type counter_{};
    key_type key_{};
    block_type buffer_{};
    std::size_t index_ = WordCount - 1;

    static constexpr wide_type wide_mask() noexcept{
        return (wide_type(1) << WordSize) - 1;
    }

    static constexpr result_type mask_word(result_type value) noexcept{
        return static_cast<result_type>(
            static_cast<wide_type>(value) & wide_mask()
        );
    }

    static constexpr std::pair<result_type, result_type> multiply_halves(
        result_type left,
        result_type right
    ) noexcept{
        const wide_type product =
            static_cast<wide_type>(mask_word(left)) * mask_word(right);
        return {
            static_cast<result_type>((product >> WordSize) & wide_mask()),
            static_cast<result_type>(product & wide_mask()),
        };
    }

    static constexpr block_type permute(const block_type& value) noexcept{
        if constexpr(WordCount == 2){
            return value;
        }else{
            return {value[2], value[1], value[0], value[3]};
        }
    }

    static constexpr block_type generate_block(
        counter_type counter,
        const key_type& key
    ) noexcept{
        block_type current = counter;
        for(std::size_t round = 0; round < RoundCount; ++round){
            const block_type permuted = permute(current);
            block_type next{};
            for(std::size_t index = 0; index < WordCount / 2; ++index){
                const auto [high, low] = multiply_halves(
                    permuted[2 * index],
                    multipliers[index]
                );
                const result_type round_key = static_cast<result_type>(
                    (static_cast<wide_type>(key[index])
                        + static_cast<wide_type>(round)
                            * round_consts[index])
                    & wide_mask()
                );
                next[2 * index] = mask_word(
                    high ^ round_key ^ permuted[2 * index + 1]
                );
                next[2 * index + 1] = low;
            }
            current = next;
        }
        return current;
    }

    constexpr void increment_counter() noexcept{
        wide_type carry = 1;
        for(std::size_t index = 0;
            index < WordCount && carry != 0;
            ++index){
            const wide_type sum =
                static_cast<wide_type>(counter_[index]) + carry;
            counter_[index] =
                static_cast<result_type>(sum & wide_mask());
            carry = sum >> WordSize;
        }
    }

    constexpr void add_counter(unsigned long long blocks) noexcept{
        wide_type remaining = blocks;
        for(std::size_t index = 0;
            index < WordCount && remaining != 0;
            ++index){
            const wide_type sum =
                static_cast<wide_type>(counter_[index])
                + (remaining & wide_mask());
            counter_[index] =
                static_cast<result_type>(sum & wide_mask());
            remaining = (remaining >> WordSize) + (sum >> WordSize);
        }
    }

    constexpr void reset_counter() noexcept{
        counter_.fill(0);
        buffer_.fill(0);
        index_ = WordCount - 1;
    }

public:
    constexpr PhiloxEngine() noexcept{
        seed(default_seed);
    }

    constexpr explicit PhiloxEngine(result_type seed_value) noexcept{
        seed(seed_value);
    }

    template<class SeedSequence>
    requires requires(
        SeedSequence& sequence,
        std::uint_least32_t* first,
        std::uint_least32_t* last
    ){
        sequence.generate(first, last);
    }
    explicit PhiloxEngine(SeedSequence& sequence){
        seed(sequence);
    }

    constexpr void seed(result_type value = default_seed) noexcept{
        key_.fill(0);
        key_[0] = mask_word(value);
        reset_counter();
    }

    template<class SeedSequence>
    requires requires(
        SeedSequence& sequence,
        std::uint_least32_t* first,
        std::uint_least32_t* last
    ){
        sequence.generate(first, last);
    }
    void seed(SeedSequence& sequence){
        constexpr std::size_t pieces = (WordSize + 31) / 32;
        std::array<std::uint_least32_t, (WordCount / 2) * pieces>
            generated{};
        sequence.generate(
            generated.data(),
            generated.data() + generated.size()
        );
        for(std::size_t key_index = 0;
            key_index < WordCount / 2;
            ++key_index){
            wide_type value = 0;
            for(std::size_t piece = 0; piece < pieces; ++piece){
                value |= static_cast<wide_type>(
                    generated[key_index * pieces + piece]
                ) << (32 * piece);
            }
            key_[key_index] =
                static_cast<result_type>(value & wide_mask());
        }
        reset_counter();
    }

    constexpr void set_counter(const counter_type& value) noexcept{
        for(std::size_t index = 0; index < WordCount; ++index){
            counter_[index] = mask_word(value[WordCount - 1 - index]);
        }
        buffer_.fill(0);
        index_ = WordCount - 1;
    }

    constexpr counter_type counter() const noexcept{
        counter_type result{};
        for(std::size_t index = 0; index < WordCount; ++index){
            result[index] = counter_[WordCount - 1 - index];
        }
        return result;
    }

    constexpr key_type key() const noexcept{
        return key_;
    }

    constexpr std::size_t index() const noexcept{
        return index_;
    }

    constexpr result_type operator()() noexcept{
        ++index_;
        if(index_ == WordCount){
            buffer_ = generate_block(counter_, key_);
            increment_counter();
            index_ = 0;
        }
        return buffer_[index_];
    }

    constexpr void discard(unsigned long long count) noexcept{
        if(index_ < WordCount - 1){
            const auto buffered = static_cast<unsigned long long>(
                WordCount - 1 - index_
            );
            const auto consumed = std::min(count, buffered);
            index_ += static_cast<std::size_t>(consumed);
            count -= consumed;
        }
        if(count == 0) return;

        const unsigned long long blocks = count / WordCount;
        const std::size_t remainder =
            static_cast<std::size_t>(count % WordCount);
        add_counter(blocks);
        if(remainder == 0){
            index_ = WordCount - 1;
            return;
        }
        buffer_ = generate_block(counter_, key_);
        increment_counter();
        index_ = remainder - 1;
    }

    friend constexpr bool operator==(
        const PhiloxEngine& left,
        const PhiloxEngine& right
    ) noexcept{
        return left.counter_ == right.counter_
            && left.key_ == right.key_
            && left.index_ == right.index_;
    }

    static constexpr result_type min() noexcept{
        return 0;
    }

    static constexpr result_type max() noexcept{
        return static_cast<result_type>(wide_mask());
    }
};

using Philox4x32 = PhiloxEngine<
    std::uint_fast32_t,
    32,
    4,
    10,
    0xCD9E8D57ULL,
    0x9E3779B9ULL,
    0xD2511F53ULL,
    0xBB67AE85ULL
>;

using Philox4x64 = PhiloxEngine<
    std::uint_fast64_t,
    64,
    4,
    10,
    0xCA5A826395121157ULL,
    0x9E3779B97F4A7C15ULL,
    0xD2E7470EE14C6C93ULL,
    0xBB67AE8584CAA73BULL
>;
