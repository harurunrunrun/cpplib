#ifndef CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_GF2_LINEAR_STATE_CRACKER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_GF2_LINEAR_STATE_CRACKER_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

// Recovers a binary state from one observed output bit per transition.
// Step must implement bool(state_type&, std::size_t output_index), and both
// the transition and returned bit must be linear over GF(2).
template<std::size_t StateBits>
class Gf2LinearStateCracker{
    static_assert(StateBits > 0);

public:
    static constexpr std::size_t state_bits = StateBits;
    static constexpr std::size_t word_count = (StateBits + 63) / 64;
    using state_type = std::array<std::uint64_t, word_count>;

private:
    using matrix_type = std::array<state_type, StateBits>;

    matrix_type inverse_rows_{};
    std::size_t rank_ = 0;

    static constexpr bool test_bit(
        const state_type& bits,
        std::size_t position
    ) noexcept{
        return ((bits[position / 64] >> (position % 64)) & 1U) != 0;
    }

    static constexpr void set_bit(
        state_type& bits,
        std::size_t position
    ) noexcept{
        bits[position / 64] |= std::uint64_t{1} << (position % 64);
    }

    static constexpr void xor_row(
        state_type& destination,
        const state_type& source
    ) noexcept{
        for(std::size_t word = 0; word < word_count; ++word){
            destination[word] ^= source[word];
        }
    }

public:
    template<class Step>
    explicit Gf2LinearStateCracker(Step step){
        matrix_type equations{};
        for(std::size_t state_bit = 0; state_bit < StateBits; ++state_bit){
            state_type basis{};
            set_bit(basis, state_bit);
            for(std::size_t output = 0; output < StateBits; ++output){
                if(static_cast<bool>(step(basis, output))){
                    set_bit(equations[output], state_bit);
                }
            }
        }

        for(std::size_t row = 0; row < StateBits; ++row){
            set_bit(inverse_rows_[row], row);
        }
        for(std::size_t column = 0; column < StateBits; ++column){
            std::size_t pivot = rank_;
            while(pivot < StateBits
                  && !test_bit(equations[pivot], column)){
                ++pivot;
            }
            if(pivot == StateBits) continue;
            if(pivot != rank_){
                const state_type equation_temporary = equations[pivot];
                equations[pivot] = equations[rank_];
                equations[rank_] = equation_temporary;
                const state_type inverse_temporary = inverse_rows_[pivot];
                inverse_rows_[pivot] = inverse_rows_[rank_];
                inverse_rows_[rank_] = inverse_temporary;
            }
            for(std::size_t row = 0; row < StateBits; ++row){
                if(row != rank_ && test_bit(equations[row], column)){
                    xor_row(equations[row], equations[rank_]);
                    xor_row(inverse_rows_[row], inverse_rows_[rank_]);
                }
            }
            ++rank_;
        }
    }

    [[nodiscard]] constexpr std::size_t rank() const noexcept{
        return rank_;
    }

    [[nodiscard]] constexpr bool uniquely_recoverable() const noexcept{
        return rank_ == StateBits;
    }

    [[nodiscard]] state_type recover(
        std::span<const std::uint8_t> consecutive_output_bits
    ) const{
        if(consecutive_output_bits.size() != StateBits){
            throw std::invalid_argument(
                "GF(2) state recovery requires exactly StateBits observations"
            );
        }
        if(!uniquely_recoverable()){
            throw std::logic_error(
                "the observed output bit does not determine the whole state"
            );
        }

        state_type observations{};
        for(std::size_t index = 0; index < StateBits; ++index){
            const std::uint8_t bit = consecutive_output_bits[index];
            if(bit > 1){
                throw std::invalid_argument(
                    "GF(2) observations must be either 0 or 1"
                );
            }
            if(bit != 0) set_bit(observations, index);
        }

        state_type recovered{};
        for(std::size_t state_bit = 0; state_bit < StateBits; ++state_bit){
            unsigned int parity = 0;
            for(std::size_t word = 0; word < word_count; ++word){
                parity ^= std::popcount(
                    inverse_rows_[state_bit][word] & observations[word]
                ) & 1U;
            }
            if(parity != 0) set_bit(recovered, state_bit);
        }
        return recovered;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANDOM_CRACK_GF2_LINEAR_STATE_CRACKER_HPP_INCLUDED
