#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long maximum_commando_effectiveness(
    const std::vector<int>& effectiveness,
    long long quadratic,
    long long linear,
    long long constant
){
    if(quadratic >= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: quadratic must be negative "
            "(maximum_commando_effectiveness)."
        );
    }
    struct Line {
        long long slope;
        long long intercept;
        __int128 evaluate(long long x) const {
            return static_cast<__int128>(slope) * x + intercept;
        }
    };
    std::vector<Line> hull;
    hull.reserve(effectiveness.size() + 1);
    std::size_t first = 0;
    hull.push_back({0, 0});
    long long prefix = 0;
    long long answer = 0;
    for(int value: effectiveness){
        if(value <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: nonpositive effectiveness "
                "(maximum_commando_effectiveness)."
            );
        }
        if(prefix > std::numeric_limits<long long>::max() - value)[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: prefix overflow "
                "(maximum_commando_effectiveness)."
            );
        }
        prefix += value;
        while(first + 1 < hull.size() &&
              hull[first + 1].evaluate(prefix) >= hull[first].evaluate(prefix)){
            first++;
        }
        const __int128 base = static_cast<__int128>(quadratic) * prefix * prefix +
                              static_cast<__int128>(linear) * prefix + constant;
        const __int128 best = base + hull[first].evaluate(prefix);
        if(best < std::numeric_limits<long long>::min() ||
           best > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: result overflow "
                "(maximum_commando_effectiveness)."
            );
        }
        answer = static_cast<long long>(best);
        const __int128 intercept = best +
            static_cast<__int128>(quadratic) * prefix * prefix -
            static_cast<__int128>(linear) * prefix;
        if(intercept < std::numeric_limits<long long>::min() ||
           intercept > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: hull overflow "
                "(maximum_commando_effectiveness)."
            );
        }
        const Line next{
            -2 * quadratic * prefix,
            static_cast<long long>(intercept)
        };
        while(hull.size() >= first + 2){
            const auto& a = hull[hull.size() - 2];
            const auto& b = hull.back();
            if(static_cast<__int128>(b.intercept - a.intercept) *
                   (b.slope - next.slope) >
               static_cast<__int128>(next.intercept - b.intercept) *
                   (a.slope - b.slope)) break;
            hull.pop_back();
        }
        hull.push_back(next);
    }
    return answer;
}
