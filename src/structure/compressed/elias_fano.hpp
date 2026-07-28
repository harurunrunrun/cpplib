#ifndef CPPLIB_SRC_STRUCTURE_COMPRESSED_ELIAS_FANO_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_COMPRESSED_ELIAS_FANO_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

class EliasFano {
    std::size_t size_ = 0;
    std::uint64_t universe_ = 0;
    int low_bits_ = 0;
    std::vector<std::uint64_t> lows_;
    std::vector<std::uint64_t> high_bits_;
    std::vector<std::size_t> select_samples_;
    static constexpr std::size_t SAMPLE = 256;

    [[nodiscard]] std::uint64_t low_at(std::size_t index) const {
        if (low_bits_ == 0) return 0;
        const std::size_t bit = index * static_cast<std::size_t>(low_bits_);
        const std::size_t word = bit / 64;
        const int offset = static_cast<int>(bit % 64);
        std::uint64_t value = lows_[word] >> offset;
        if (offset + low_bits_ > 64) {
            value |= lows_[word + 1] << (64 - offset);
        }
        return value & ((std::uint64_t{1} << low_bits_) - 1);
    }
    [[nodiscard]] std::size_t select_high(std::size_t index) const {
        const std::size_t block = index / SAMPLE;
        std::size_t position = select_samples_[block];
        std::size_t rank = block * SAMPLE;
        if (rank == index) return position;
        ++position;
        std::size_t word_index = position / 64;
        std::uint64_t word =
            high_bits_[word_index] & (~std::uint64_t{0} << (position % 64));
        while (true) {
            const std::size_t count = static_cast<std::size_t>(
                std::popcount(word)
            );
            if (rank + count >= index) {
                std::size_t needed = index - rank;
                while (needed > 1) {
                    word &= word - 1;
                    --needed;
                }
                return word_index * 64
                    + static_cast<std::size_t>(std::countr_zero(word));
            }
            rank += count;
            ++word_index;
            word = high_bits_[word_index];
        }
    }

public:
    EliasFano() = default;
    explicit EliasFano(const std::vector<std::uint64_t>& values) {
        build(values);
    }
    void build(const std::vector<std::uint64_t>& values) {
        for (std::size_t i = 1; i < values.size(); ++i) {
            if (values[i - 1] > values[i]) {
                throw std::invalid_argument("EliasFano values not sorted");
            }
        }
        size_ = values.size();
        universe_ = values.empty() ? 0 : values.back();
        low_bits_ = 0;
        if (size_ != 0 && universe_ / size_ > 1) {
            low_bits_ = static_cast<int>(
                std::bit_width(universe_ / size_) - 1
            );
        }
        const std::size_t low_total =
            size_ * static_cast<std::size_t>(low_bits_);
        lows_.assign((low_total + 63) / 64 + 1, 0);
        const std::uint64_t low_mask = low_bits_ == 0
            ? 0 : (std::uint64_t{1} << low_bits_) - 1;
        for (std::size_t i = 0; i < size_; ++i) {
            if (low_bits_ != 0) {
                const std::size_t bit =
                    i * static_cast<std::size_t>(low_bits_);
                lows_[bit / 64] |= (values[i] & low_mask) << (bit % 64);
                if (bit % 64 + static_cast<std::size_t>(low_bits_) > 64) {
                    lows_[bit / 64 + 1] |=
                        (values[i] & low_mask) >> (64 - bit % 64);
                }
            }
        }
        const std::size_t high_length = size_ == 0 ? 1
            : static_cast<std::size_t>(universe_ >> low_bits_) + size_ + 1;
        high_bits_.assign((high_length + 63) / 64, 0);
        select_samples_.clear();
        for (std::size_t i = 0; i < size_; ++i) {
            const std::size_t position =
                static_cast<std::size_t>(values[i] >> low_bits_) + i;
            high_bits_[position / 64] |=
                std::uint64_t{1} << (position % 64);
            if (i % SAMPLE == 0) select_samples_.push_back(position);
        }
    }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::uint64_t select(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("EliasFano::select");
        const std::uint64_t high =
            static_cast<std::uint64_t>(select_high(index) - index);
        return (high << low_bits_) | low_at(index);
    }
    [[nodiscard]] std::size_t lower_bound(std::uint64_t value) const {
        std::size_t left = 0, right = size_;
        while (left < right) {
            const std::size_t middle = (left + right) / 2;
            if (select(middle) < value) left = middle + 1;
            else right = middle;
        }
        return left;
    }
    [[nodiscard]] std::size_t rank(std::uint64_t value) const {
        if (value == UINT64_MAX) return size_;
        return lower_bound(value + 1);
    }
    [[nodiscard]] std::uint64_t universe() const noexcept { return universe_; }
};

#endif  // CPPLIB_SRC_STRUCTURE_COMPRESSED_ELIAS_FANO_HPP_INCLUDED
