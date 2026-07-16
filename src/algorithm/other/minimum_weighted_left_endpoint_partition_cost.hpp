#pragma once

#include "../math/exact_integer.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_weighted_left_endpoint_partition_cost(
    const std::vector<long long>& weights,
    int group_count
){
    if(weights.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "minimum_weighted_left_endpoint_partition_cost input is too long"
        );
    }
    const int size = static_cast<int>(weights.size());
    if(group_count <= 0 || group_count > size)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid group count "
            "(minimum_weighted_left_endpoint_partition_cost)."
        );
    }

    using Wide = __int128_t;
    using Exact = ExactInteger;
    const std::size_t state_count = weights.size() + 1;
    std::vector<Wide> prefix_weight(state_count);
    std::vector<Wide> prefix_moment(state_count);
    for(int index = 1; index <= size; ++index){
        const long long weight =
            weights[static_cast<std::size_t>(index - 1)];
        if(weight <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: weights must be positive "
                "(minimum_weighted_left_endpoint_partition_cost)."
            );
        }
        prefix_weight[static_cast<std::size_t>(index)] =
            prefix_weight[static_cast<std::size_t>(index - 1)] +
            static_cast<Wide>(weight);
        prefix_moment[static_cast<std::size_t>(index)] =
            prefix_moment[static_cast<std::size_t>(index - 1)] +
            static_cast<Wide>(index) * weight;
    }

    const Wide infinity = Wide(1) << 126;
    std::vector<Wide> previous(state_count, infinity);
    std::vector<Wide> current(state_count, infinity);
    previous[0] = 0;
    struct Line {
        Wide slope;
        Wide intercept;

        Wide evaluate(Wide value) const {
            return slope * value + intercept;
        }
    };

    for(int group = 1; group <= group_count; ++group){
        std::fill(current.begin(), current.end(), infinity);
        std::vector<Line> hull;
        hull.reserve(static_cast<std::size_t>(size - group + 1));
        std::size_t first_line = 0;
        const auto append = [&](int split){
            const Wide coefficient = static_cast<Wide>(split) + 1;
            const Line next{
                -coefficient,
                previous[static_cast<std::size_t>(split)] -
                    prefix_moment[static_cast<std::size_t>(split)] +
                    coefficient *
                        prefix_weight[static_cast<std::size_t>(split)]
            };
            while(hull.size() >= first_line + 2){
                const Line& first = hull[hull.size() - 2];
                const Line& second = hull.back();
                if((Exact(second.intercept) - first.intercept) *
                       (Exact(second.slope) - next.slope) <
                   (Exact(next.intercept) - second.intercept) *
                       (Exact(first.slope) - second.slope)){
                    break;
                }
                hull.pop_back();
            }
            hull.push_back(next);
        };
        append(group - 1);

        for(int end = group; end <= size; ++end){
            const Wide weight =
                prefix_weight[static_cast<std::size_t>(end)];
            while(first_line + 1 < hull.size() &&
                  hull[first_line + 1].evaluate(weight) <=
                      hull[first_line].evaluate(weight)){
                ++first_line;
            }
            current[static_cast<std::size_t>(end)] =
                prefix_moment[static_cast<std::size_t>(end)] +
                hull[first_line].evaluate(weight);
            if(end < size) append(end);
        }
        previous.swap(current);
    }

    const Wide answer = previous[static_cast<std::size_t>(size)];
    if(answer > static_cast<Wide>(
        std::numeric_limits<long long>::max()
    ))[[unlikely]]{
        throw std::overflow_error(
            "minimum_weighted_left_endpoint_partition_cost result overflow"
        );
    }
    return static_cast<long long>(answer);
}
