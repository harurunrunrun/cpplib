#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_QUADRATIC_PARTITION_SCORE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_QUADRATIC_PARTITION_SCORE_HPP_INCLUDED

#include "../../math/integer/exact_integer.hpp"

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long maximum_quadratic_partition_score(
    const std::vector<long long>& segment_values,
    long long quadratic_coefficient,
    long long linear_coefficient,
    long long constant_coefficient
){
    if(quadratic_coefficient >= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: quadratic_coefficient must be negative "
            "(maximum_quadratic_partition_score)."
        );
    }
    using Wide = ExactInteger;
    using Cross = ExactInteger;
    struct Line {
        Wide slope;
        Wide intercept;
        Wide evaluate(long long x) const {
            return slope * x + intercept;
        }
    };
    std::vector<Line> hull;
    hull.reserve(segment_values.size() + 1);
    std::size_t first = 0;
    hull.push_back({0, 0});
    long long prefix = 0;
    long long answer = 0;
    for(long long value: segment_values){
        if(value <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: segment values must be positive "
                "(maximum_quadratic_partition_score)."
            );
        }
        if(prefix > std::numeric_limits<long long>::max() - value)[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: prefix overflow "
                "(maximum_quadratic_partition_score)."
            );
        }
        prefix += value;
        while(first + 1 < hull.size() &&
              hull[first + 1].evaluate(prefix) >= hull[first].evaluate(prefix)){
            first++;
        }
        const Wide base = Wide(quadratic_coefficient) * prefix * prefix +
                          Wide(linear_coefficient) * prefix + constant_coefficient;
        const Wide best = base + hull[first].evaluate(prefix);
        if(best < std::numeric_limits<long long>::min() ||
           best > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: result overflow "
                "(maximum_quadratic_partition_score)."
            );
        }
        answer = best.checked_to<long long>();
        const Wide intercept = best +
            Wide(quadratic_coefficient) * prefix * prefix -
            Wide(linear_coefficient) * prefix;
        const Line next{
            Wide(-2) * quadratic_coefficient * prefix,
            intercept
        };
        while(hull.size() >= first + 2){
            const auto& a = hull[hull.size() - 2];
            const auto& b = hull.back();
            if(Cross(b.intercept - a.intercept) * Cross(b.slope - next.slope) >
               Cross(next.intercept - b.intercept) * Cross(a.slope - b.slope)) break;
            hull.pop_back();
        }
        hull.push_back(next);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_QUADRATIC_PARTITION_SCORE_HPP_INCLUDED
