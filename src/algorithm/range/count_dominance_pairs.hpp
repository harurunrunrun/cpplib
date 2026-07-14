#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace count_dominance_pairs_internal{

struct FenwickTree{
    std::vector<long long> data;

    explicit FenwickTree(std::size_t size): data(size + 1, 0){}

    void add(std::size_t index, long long value){
        for(index++; index < data.size(); index += index & -index){
            data[index] += value;
        }
    }

    long long prefix_sum(std::size_t right) const{
        long long result = 0;
        for(; right > 0; right -= right & -right){
            result += data[right];
        }
        return result;
    }
};

} // namespace count_dominance_pairs_internal

template<class A, class B>
long long count_dominance_pairs(
    const std::vector<A>& first,
    const std::vector<B>& second
){
    if(first.size() != second.size())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size mismatch (count_dominance_pairs)."
        );
    }
    constexpr std::size_t MAX_SAFE_SIZE = 3'037'000'499ULL;
    if(first.size() > MAX_SAFE_SIZE)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: result does not fit in long long "
            "(count_dominance_pairs)."
        );
    }

    std::vector<B> compressed = second;
    std::sort(compressed.begin(), compressed.end());
    compressed.erase(
        std::unique(compressed.begin(), compressed.end()), compressed.end()
    );

    std::vector<std::pair<A, B>> points;
    points.reserve(first.size());
    for(std::size_t index = 0; index < first.size(); index++){
        points.emplace_back(first[index], second[index]);
    }
    std::sort(points.begin(), points.end(), [](const auto& left, const auto& right){
        return left.first > right.first;
    });

    count_dominance_pairs_internal::FenwickTree fenwick(compressed.size());
    long long answer = 0;
    for(std::size_t begin = 0; begin < points.size();){
        std::size_t end = begin;
        while(end < points.size() && points[end].first == points[begin].first){
            const std::size_t index = static_cast<std::size_t>(
                std::lower_bound(
                    compressed.begin(), compressed.end(), points[end].second
                ) - compressed.begin()
            );
            fenwick.add(index, 1);
            end++;
        }
        for(std::size_t index = begin; index < end; index++){
            const std::size_t right = static_cast<std::size_t>(
                std::upper_bound(
                    compressed.begin(), compressed.end(), points[index].second
                ) - compressed.begin()
            );
            answer += fenwick.prefix_sum(right);
        }
        begin = end;
    }
    return answer;
}
