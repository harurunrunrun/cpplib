#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_DICTIONARY_OF_BASIC_FACTORS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_DICTIONARY_OF_BASIC_FACTORS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class DictionaryOfBasicFactors{
    int size_;
    std::vector<std::vector<int>> identifier_;

public:
    explicit DictionaryOfBasicFactors(const std::string& text)
        : size_(static_cast<int>(text.size())){
        if(size_ == 0) return;
        identifier_.push_back(
            std::vector<int>(static_cast<std::size_t>(size_))
        );
        std::array<int, 256> byte_id;
        byte_id.fill(-1);
        int classes = 0;
        for(int position = 0; position < size_; ++position){
            const unsigned char symbol =
                static_cast<unsigned char>(text[static_cast<std::size_t>(
                    position
                )]);
            if(byte_id[static_cast<std::size_t>(symbol)] == -1){
                byte_id[static_cast<std::size_t>(symbol)] = classes++;
            }
            identifier_[0][static_cast<std::size_t>(position)] =
                byte_id[static_cast<std::size_t>(symbol)];
        }
        for(int level = 1;
            (std::uint64_t{1} << level)
                <= static_cast<std::uint64_t>(size_);
            ++level){
            const int half = static_cast<int>(
                std::uint64_t{1} << (level - 1)
            );
            const int width =
                static_cast<int>(std::uint64_t{1} << level);
            const int count = size_ - width + 1;
            const auto& previous_row =
                identifier_[static_cast<std::size_t>(level - 1)];
            std::vector<int> order(static_cast<std::size_t>(count));
            std::iota(order.begin(), order.end(), 0);
            std::vector<int> buffer(static_cast<std::size_t>(count));
            const auto counting_sort = [&](int offset){
                std::vector<int> frequency(
                    static_cast<std::size_t>(classes), 0
                );
                for(int position: order){
                    ++frequency[static_cast<std::size_t>(
                        previous_row[static_cast<std::size_t>(
                            position + offset
                        )]
                    )];
                }
                int sum = 0;
                for(int& value: frequency){
                    const int amount = value;
                    value = sum;
                    sum += amount;
                }
                for(int position: order){
                    const int key = previous_row[static_cast<std::size_t>(
                        position + offset
                    )];
                    buffer[static_cast<std::size_t>(
                        frequency[static_cast<std::size_t>(key)]++
                    )] = position;
                }
                order.swap(buffer);
            };
            counting_sort(half);
            counting_sort(0);
            std::vector<int> row(static_cast<std::size_t>(count));
            int next_classes = 0;
            int previous_position = -1;
            for(int position: order){
                if(previous_position == -1
                    || previous_row[static_cast<std::size_t>(position)]
                        != previous_row[static_cast<std::size_t>(
                            previous_position
                        )]
                    || previous_row[static_cast<std::size_t>(position + half)]
                        != previous_row[static_cast<std::size_t>(
                            previous_position + half
                        )]) ++next_classes;
                row[static_cast<std::size_t>(position)] = next_classes - 1;
                previous_position = position;
            }
            identifier_.push_back(std::move(row));
            classes = next_classes;
        }
    }

    int size() const noexcept{
        return size_;
    }

    int rank(int position, int level) const{
        if(level < 0 || static_cast<int>(identifier_.size()) <= level
            || position < 0
            || static_cast<int>(
                identifier_[static_cast<std::size_t>(level)].size()
            ) <= position)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(DictionaryOfBasicFactors::rank)."
            );
        }
        return identifier_[static_cast<std::size_t>(level)]
            [static_cast<std::size_t>(position)];
    }

    bool equal(int first, int second, int length) const{
        if(length < 0 || first < 0 || second < 0
            || size_ - length < first || size_ - length < second)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(DictionaryOfBasicFactors::equal)."
            );
        }
        if(length == 0) return true;
        const int level = 31 - __builtin_clz(
            static_cast<unsigned int>(length)
        );
        const int block = 1 << level;
        return rank(first, level) == rank(second, level)
            && rank(first + length - block, level)
                == rank(second + length - block, level);
    }

    int lce(int first, int second) const{
        if(first < 0 || size_ < first || second < 0 || size_ < second)
            [[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(DictionaryOfBasicFactors::lce)."
            );
        }
        int result = 0;
        const int limit = std::min(size_ - first, size_ - second);
        for(int level = static_cast<int>(identifier_.size()); level-- > 0;){
            const int block = 1 << level;
            if(result + block <= limit
                && rank(first + result, level)
                    == rank(second + result, level)){
                result += block;
            }
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_DICTIONARY_OF_BASIC_FACTORS_HPP_INCLUDED
