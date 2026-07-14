#pragma once

#include <algorithm>
#include <cmath>
#include <deque>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../other/mo.hpp"

inline std::vector<int> maximum_zero_sum_subarray_lengths(
    const std::vector<int>& values,
    const std::vector<std::pair<int, int>>& ranges
){
    class DistanceMaximum{
        int limit_;
        int block_size_;
        std::vector<int> frequency_;
        std::vector<int> block_frequency_;
    public:
        explicit DistanceMaximum(int limit)
            : limit_(limit),
              block_size_(std::max(1, static_cast<int>(std::sqrt(limit + 1.0L)))),
              frequency_(static_cast<std::size_t>(limit + 1)),
              block_frequency_(static_cast<std::size_t>(limit / block_size_ + 1)){}
        void add(int distance){
            ++frequency_[static_cast<std::size_t>(distance)];
            ++block_frequency_[static_cast<std::size_t>(distance / block_size_)];
        }
        void erase(int distance){
            --frequency_[static_cast<std::size_t>(distance)];
            --block_frequency_[static_cast<std::size_t>(distance / block_size_)];
        }
        int maximum() const{
            for(int block = static_cast<int>(block_frequency_.size()) - 1; block >= 0; --block){
                if(block_frequency_[static_cast<std::size_t>(block)] == 0) continue;
                for(int distance = std::min(limit_, (block + 1) * block_size_ - 1);
                    distance >= block * block_size_; --distance){
                    if(frequency_[static_cast<std::size_t>(distance)] != 0) return distance;
                }
            }
            return 0;
        }
    };

    const int size = static_cast<int>(values.size());
    std::vector<long long> prefix(static_cast<std::size_t>(size + 1));
    for(int index = 0; index < size; ++index){
        prefix[static_cast<std::size_t>(index + 1)] =
            prefix[static_cast<std::size_t>(index)]
            + values[static_cast<std::size_t>(index)];
    }
    std::vector<long long> coordinates = prefix;
    std::sort(coordinates.begin(), coordinates.end());
    coordinates.erase(std::unique(coordinates.begin(), coordinates.end()), coordinates.end());
    std::vector<int> id(prefix.size());
    for(int index = 0; index <= size; ++index){
        id[static_cast<std::size_t>(index)] = static_cast<int>(
            std::lower_bound(coordinates.begin(), coordinates.end(), prefix[static_cast<std::size_t>(index)])
            - coordinates.begin()
        );
    }
    Mo mo(size + 1);
    for(const auto [left, right]: ranges){
        if(left < 0 || right < left || size < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid range "
                "(maximum_zero_sum_subarray_lengths)."
            );
        }
        mo.add_query(left, right + 1);
    }
    std::vector<std::deque<int>> positions(coordinates.size());
    DistanceMaximum distances(size);
    std::vector<int> answer(ranges.size());
    const auto erase_span = [&](const auto& indices){
        if(!indices.empty()) distances.erase(indices.back() - indices.front());
    };
    const auto add_span = [&](const auto& indices){
        if(!indices.empty()) distances.add(indices.back() - indices.front());
    };
    const auto add_left = [&](int index){
        auto& indices = positions[static_cast<std::size_t>(id[static_cast<std::size_t>(index)])];
        erase_span(indices); indices.push_front(index); add_span(indices);
    };
    const auto add_right = [&](int index){
        auto& indices = positions[static_cast<std::size_t>(id[static_cast<std::size_t>(index)])];
        erase_span(indices); indices.push_back(index); add_span(indices);
    };
    const auto erase_left = [&](int index){
        auto& indices = positions[static_cast<std::size_t>(id[static_cast<std::size_t>(index)])];
        erase_span(indices); indices.pop_front(); add_span(indices);
    };
    const auto erase_right = [&](int index){
        auto& indices = positions[static_cast<std::size_t>(id[static_cast<std::size_t>(index)])];
        erase_span(indices); indices.pop_back(); add_span(indices);
    };
    mo.solve(add_left, add_right, erase_left, erase_right, [&](int query){
        answer[static_cast<std::size_t>(query)] = distances.maximum();
    });
    return answer;
}
