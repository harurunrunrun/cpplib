#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAXIMUM_CONSECUTIVE_REPETITIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAXIMUM_CONSECUTIVE_REPETITIONS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../suffix/suffix_array.hpp"

namespace maximum_consecutive_repetitions_internal{

class PlusMinusOneRangeMinimum{
    int block_size_ = 1;
    std::vector<int> depth_;
    std::vector<unsigned int> signature_;
    std::vector<std::vector<unsigned char>> micro_;
    std::vector<int> logarithm_;
    std::vector<std::vector<int>> sparse_;

    int better(int first, int second) const{
        if(first < 0) return second;
        if(second < 0) return first;
        return depth_[static_cast<std::size_t>(first)] <=
            depth_[static_cast<std::size_t>(second)] ? first : second;
    }

    int local_minimum(int block, int left, int right) const{
        const auto& table = micro_[static_cast<std::size_t>(
            signature_[static_cast<std::size_t>(block)]
        )];
        const int offset = table[static_cast<std::size_t>(
            left * block_size_ + right
        )];
        return block * block_size_ + offset;
    }

public:
    void build(std::vector<int> depth){
        depth_ = std::move(depth);
        const int size = static_cast<int>(depth_.size());
        if(size == 0) return;

        int logarithm = 0;
        for(int value = size; value > 1; value >>= 1) ++logarithm;
        block_size_ = std::max(1, logarithm / 2);
        const int block_count =
            (size - 1) / block_size_ + 1;
        signature_.assign(static_cast<std::size_t>(block_count), 0);
        micro_.assign(
            static_cast<std::size_t>(
                1U << static_cast<unsigned int>(block_size_ - 1)
            ),
            {}
        );

        std::vector<int> block_minimum(
            static_cast<std::size_t>(block_count), -1
        );
        for(int block = 0; block < block_count; ++block){
            const int begin = block * block_size_;
            const int end = std::min(size, begin + block_size_);
            unsigned int signature = 0;
            for(int index = begin + 1; index < end; ++index){
                if(depth_[static_cast<std::size_t>(index)] >
                   depth_[static_cast<std::size_t>(index - 1)]){
                    signature |= 1U << static_cast<unsigned int>(
                        index - begin - 1
                    );
                }
            }
            signature_[static_cast<std::size_t>(block)] = signature;
            int minimum = begin;
            for(int index = begin + 1; index < end; ++index){
                minimum = better(minimum, index);
            }
            block_minimum[static_cast<std::size_t>(block)] = minimum;

            auto& table = micro_[static_cast<std::size_t>(signature)];
            if(!table.empty()) continue;
            table.assign(
                static_cast<std::size_t>(block_size_ * block_size_), 0
            );
            std::vector<int> relative(
                static_cast<std::size_t>(block_size_), 0
            );
            for(int offset = 1; offset < block_size_; ++offset){
                relative[static_cast<std::size_t>(offset)] =
                    relative[static_cast<std::size_t>(offset - 1)] +
                    ((signature >> static_cast<unsigned int>(offset - 1)) & 1U
                        ? 1 : -1);
            }
            for(int left = 0; left < block_size_; ++left){
                int minimum_offset = left;
                for(int right = left; right < block_size_; ++right){
                    if(relative[static_cast<std::size_t>(right)] <
                       relative[static_cast<std::size_t>(minimum_offset)]){
                        minimum_offset = right;
                    }
                    table[static_cast<std::size_t>(
                        left * block_size_ + right
                    )] = static_cast<unsigned char>(minimum_offset);
                }
            }
        }

        logarithm_.assign(
            static_cast<std::size_t>(block_count + 1), 0
        );
        for(int width = 2; width <= block_count; ++width){
            logarithm_[static_cast<std::size_t>(width)] =
                logarithm_[static_cast<std::size_t>(width / 2)] + 1;
        }
        sparse_.clear();
        sparse_.push_back(std::move(block_minimum));
        for(int level = 1;
            (1U << static_cast<unsigned int>(level)) <=
                static_cast<unsigned int>(block_count);
            ++level){
            const int width = 1 << level;
            const int half = width >> 1;
            std::vector<int> row(
                static_cast<std::size_t>(block_count - width + 1)
            );
            for(int left = 0; left + width <= block_count; ++left){
                row[static_cast<std::size_t>(left)] = better(
                    sparse_[static_cast<std::size_t>(level - 1)]
                        [static_cast<std::size_t>(left)],
                    sparse_[static_cast<std::size_t>(level - 1)]
                        [static_cast<std::size_t>(left + half)]
                );
            }
            sparse_.push_back(std::move(row));
        }
    }

    int query(int left, int right) const{
        const int left_block = left / block_size_;
        const int right_block = right / block_size_;
        if(left_block == right_block){
            return local_minimum(
                left_block, left % block_size_, right % block_size_
            );
        }
        int answer = local_minimum(
            left_block, left % block_size_, block_size_ - 1
        );
        answer = better(answer, local_minimum(
            right_block, 0, right % block_size_
        ));
        if(left_block + 1 < right_block){
            const int first = left_block + 1;
            const int last = right_block - 1;
            const int width = last - first + 1;
            const int level = logarithm_[static_cast<std::size_t>(width)];
            answer = better(answer, better(
                sparse_[static_cast<std::size_t>(level)]
                    [static_cast<std::size_t>(first)],
                sparse_[static_cast<std::size_t>(level)]
                    [static_cast<std::size_t>(last - (1 << level) + 1)]
            ));
        }
        return answer;
    }
};

class LinearRangeMinimum{
    std::vector<int> value_;
    std::vector<int> first_;
    std::vector<int> euler_node_;
    PlusMinusOneRangeMinimum euler_minimum_;

public:
    void build(std::vector<int> value){
        value_ = std::move(value);
        const int size = static_cast<int>(value_.size());
        if(size == 0) return;

        std::vector<int> left(static_cast<std::size_t>(size), -1);
        std::vector<int> right(static_cast<std::size_t>(size), -1);
        std::vector<int> stack;
        stack.reserve(static_cast<std::size_t>(size));
        for(int index = 0; index < size; ++index){
            int last = -1;
            while(!stack.empty() &&
                  value_[static_cast<std::size_t>(index)] <
                      value_[static_cast<std::size_t>(stack.back())]){
                last = stack.back();
                stack.pop_back();
            }
            if(!stack.empty()){
                right[static_cast<std::size_t>(stack.back())] = index;
            }
            if(last != -1) left[static_cast<std::size_t>(index)] = last;
            stack.push_back(index);
        }
        const int root = stack.front();

        struct Frame{
            int node;
            int depth;
            unsigned char stage;
        };
        std::vector<Frame> traversal;
        traversal.reserve(static_cast<std::size_t>(size));
        first_.assign(static_cast<std::size_t>(size), -1);
        euler_node_.clear();
        const std::size_t euler_size =
            static_cast<std::size_t>(size) * 2 - 1;
        euler_node_.reserve(euler_size);
        std::vector<int> euler_depth;
        euler_depth.reserve(euler_size);
        const auto append = [&](int node, int depth){
            if(first_[static_cast<std::size_t>(node)] == -1){
                first_[static_cast<std::size_t>(node)] =
                    static_cast<int>(euler_node_.size());
            }
            euler_node_.push_back(node);
            euler_depth.push_back(depth);
        };
        append(root, 0);
        traversal.push_back({root, 0, 0});
        while(!traversal.empty()){
            const int node = traversal.back().node;
            const int depth = traversal.back().depth;
            if(traversal.back().stage == 0){
                traversal.back().stage = 1;
                const int child = left[static_cast<std::size_t>(node)];
                if(child != -1){
                    append(child, depth + 1);
                    traversal.push_back({child, depth + 1, 0});
                }
            }else if(traversal.back().stage == 1){
                traversal.back().stage = 2;
                const int child = right[static_cast<std::size_t>(node)];
                if(child != -1){
                    append(child, depth + 1);
                    traversal.push_back({child, depth + 1, 0});
                }
            }else{
                traversal.pop_back();
                if(!traversal.empty()){
                    append(traversal.back().node, traversal.back().depth);
                }
            }
        }
        euler_minimum_.build(std::move(euler_depth));
    }

    int query(int left, int right) const{
        if(left > right) std::swap(left, right);
        int first = first_[static_cast<std::size_t>(left)];
        int second = first_[static_cast<std::size_t>(right)];
        if(first > second) std::swap(first, second);
        const int position = euler_minimum_.query(first, second);
        return value_[static_cast<std::size_t>(
            euler_node_[static_cast<std::size_t>(position)]
        )];
    }
};

class LcpQuery{
    int length_;
    std::vector<int> rank_;
    LinearRangeMinimum minimum_;

public:
    explicit LcpQuery(const std::string& text):
        length_(static_cast<int>(text.size())){
        const std::vector<int> suffixes = suffix_array(text);
        rank_.resize(static_cast<std::size_t>(length_));
        for(int index = 0; index < length_; ++index){
            rank_[static_cast<std::size_t>(
                suffixes[static_cast<std::size_t>(index)]
            )] = index;
        }
        minimum_.build(lcp_array(text, suffixes));
    }

    const std::vector<int>& ranks() const{
        return rank_;
    }

    int query(int first, int second) const{
        if(first == second) return length_ - first;
        int left = rank_[static_cast<std::size_t>(first)];
        int right = rank_[static_cast<std::size_t>(second)];
        if(left > right) std::swap(left, right);
        return minimum_.query(left, right - 1);
    }
};

inline void inspect_lyndon_roots(
    const std::vector<int>& rank,
    const LcpQuery& forward,
    const LcpQuery& backward,
    int& answer
){
    const int length = static_cast<int>(rank.size());
    std::vector<int> next_smaller(static_cast<std::size_t>(length), length);
    std::vector<int> stack;
    stack.reserve(static_cast<std::size_t>(length));
    for(int index = 0; index < length; ++index){
        while(!stack.empty() &&
              rank[static_cast<std::size_t>(index)] <
                  rank[static_cast<std::size_t>(stack.back())]){
            next_smaller[static_cast<std::size_t>(stack.back())] = index;
            stack.pop_back();
        }
        stack.push_back(index);
    }

    for(int left = 0; left < length; ++left){
        const int right = next_smaller[static_cast<std::size_t>(left)];
        if(right == length) continue;
        const int period = right - left;
        const int common_right = forward.query(left, right);
        const int common_left = left == 0 ? 0 : backward.query(
            length - left, length - right
        );
        if(common_left + common_right >= period){
            answer = std::max(
                answer, 1 + (common_left + common_right) / period
            );
        }
    }
}

} // namespace maximum_consecutive_repetitions_internal

inline int maximum_consecutive_repetitions(const std::string& text){
    constexpr std::size_t maximum_length =
        (static_cast<std::size_t>(std::numeric_limits<int>::max()) + 3) / 2;
    if(text.size() > maximum_length)[[unlikely]]{
        throw std::length_error(
            "maximum_consecutive_repetitions input is too long"
        );
    }
    const int length = static_cast<int>(text.size());
    if(length == 0) return 0;

    const maximum_consecutive_repetitions_internal::LcpQuery forward(text);
    const std::string reversed(text.rbegin(), text.rend());
    const maximum_consecutive_repetitions_internal::LcpQuery backward(reversed);

    int answer = 1;
    std::vector<int> rank = forward.ranks();
    maximum_consecutive_repetitions_internal::inspect_lyndon_roots(
        rank, forward, backward, answer
    );
    for(int& value: rank) value = length - 1 - value;
    maximum_consecutive_repetitions_internal::inspect_lyndon_roots(
        rank, forward, backward, answer
    );
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAXIMUM_CONSECUTIVE_REPETITIONS_HPP_INCLUDED
