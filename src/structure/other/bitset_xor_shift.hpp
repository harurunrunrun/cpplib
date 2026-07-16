#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

template<std::size_t MAX_BITS>
class BitsetXorShift{
    static constexpr std::size_t WORD_BITS = 64;
    static constexpr std::size_t MAX_WORDS =
        MAX_BITS / WORD_BITS + static_cast<std::size_t>(MAX_BITS % WORD_BITS != 0);

    std::size_t bit_size = 0;
    std::array<std::uint64_t, MAX_WORDS> words{};

    std::size_t word_count() const noexcept{
        return bit_size / WORD_BITS +
            static_cast<std::size_t>(bit_size % WORD_BITS != 0);
    }

    void check_position(std::size_t position, const char* message) const{
        if(position >= bit_size)[[unlikely]] throw std::runtime_error(message);
    }

    void trim() noexcept{
        const std::size_t count = word_count();
        if(count == 0) return;
        const std::size_t remainder = bit_size % WORD_BITS;
        if(remainder != 0){
            words[count - 1] &= (std::uint64_t{1} << remainder) - 1;
        }
    }

public:
    explicit BitsetXorShift(std::size_t size = MAX_BITS): bit_size(size){
        if(size > MAX_BITS)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (BitsetXorShift constructor)."
            );
        }
    }

    static constexpr std::size_t capacity() noexcept{ return MAX_BITS; }
    std::size_t size() const noexcept{ return bit_size; }
    bool empty() const noexcept{ return bit_size == 0; }

    bool test(std::size_t position) const{
        check_position(
            position,
            "library assertion fault: range violation (BitsetXorShift::test)."
        );
        return ((words[position / WORD_BITS] >> (position % WORD_BITS)) & 1U) != 0;
    }

    bool operator[](std::size_t position) const{ return test(position); }

    BitsetXorShift& set(std::size_t position, bool value = true){
        check_position(
            position,
            "library assertion fault: range violation (BitsetXorShift::set)."
        );
        const std::uint64_t mask = std::uint64_t{1} << (position % WORD_BITS);
        if(value) words[position / WORD_BITS] |= mask;
        else words[position / WORD_BITS] &= ~mask;
        return *this;
    }

    BitsetXorShift& reset(std::size_t position){ return set(position, false); }

    BitsetXorShift& flip(std::size_t position){
        check_position(
            position,
            "library assertion fault: range violation (BitsetXorShift::flip)."
        );
        words[position / WORD_BITS] ^= std::uint64_t{1} << (position % WORD_BITS);
        return *this;
    }

    BitsetXorShift& set(){
        for(std::size_t index = 0; index < word_count(); ++index){
            words[index] = ~std::uint64_t{0};
        }
        trim();
        return *this;
    }

    BitsetXorShift& reset() noexcept{
        for(std::size_t index = 0; index < word_count(); ++index) words[index] = 0;
        return *this;
    }

    BitsetXorShift& flip(){
        for(std::size_t index = 0; index < word_count(); ++index){
            words[index] = ~words[index];
        }
        trim();
        return *this;
    }

    std::size_t count() const noexcept{
        std::size_t result = 0;
        for(std::size_t index = 0; index < word_count(); ++index){
            result += static_cast<std::size_t>(std::popcount(words[index]));
        }
        return result;
    }

    bool any() const noexcept{
        for(std::size_t index = 0; index < word_count(); ++index){
            if(words[index] != 0) return true;
        }
        return false;
    }

    bool none() const noexcept{ return !any(); }
    bool all() const noexcept{ return count() == bit_size; }

    BitsetXorShift& xor_left_shift(std::size_t shift) noexcept{
        if(shift >= bit_size) return *this;
        const std::size_t count = word_count();
        const std::size_t word_shift = shift / WORD_BITS;
        const unsigned bit_shift = static_cast<unsigned>(shift % WORD_BITS);

        for(std::size_t destination = count; destination-- > 0;){
            std::uint64_t shifted = 0;
            if(destination >= word_shift){
                shifted = words[destination - word_shift] << bit_shift;
                if(bit_shift != 0 && destination > word_shift){
                    shifted |= words[destination - word_shift - 1] >>
                        (WORD_BITS - bit_shift);
                }
            }
            words[destination] ^= shifted;
        }
        trim();
        return *this;
    }

    BitsetXorShift& xor_right_shift(std::size_t shift) noexcept{
        if(shift >= bit_size) return *this;
        const std::size_t count = word_count();
        const std::size_t word_shift = shift / WORD_BITS;
        const unsigned bit_shift = static_cast<unsigned>(shift % WORD_BITS);

        for(std::size_t destination = 0; destination < count; ++destination){
            std::uint64_t shifted = 0;
            const std::size_t source = destination + word_shift;
            if(source < count){
                shifted = words[source] >> bit_shift;
                if(bit_shift != 0 && source + 1 < count){
                    shifted |= words[source + 1] << (WORD_BITS - bit_shift);
                }
            }
            words[destination] ^= shifted;
        }
        trim();
        return *this;
    }

    BitsetXorShift& operator^=(const BitsetXorShift& other){
        if(bit_size != other.bit_size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size violation (BitsetXorShift::operator^=)."
            );
        }
        for(std::size_t index = 0; index < word_count(); ++index){
            words[index] ^= other.words[index];
        }
        return *this;
    }

    friend BitsetXorShift operator^(BitsetXorShift left, const BitsetXorShift& right){
        left ^= right;
        return left;
    }

    friend bool operator==(const BitsetXorShift& left, const BitsetXorShift& right){
        if(left.bit_size != right.bit_size) return false;
        for(std::size_t index = 0; index < left.word_count(); ++index){
            if(left.words[index] != right.words[index]) return false;
        }
        return true;
    }

    friend bool operator!=(const BitsetXorShift& left, const BitsetXorShift& right){
        return !(left == right);
    }
};
