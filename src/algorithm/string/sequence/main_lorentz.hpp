#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAIN_LORENTZ_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAIN_LORENTZ_HPP_INCLUDED

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "run_enumeration.hpp"

struct TandemRepeat{
    int begin;
    int half_length;

    friend bool operator==(const TandemRepeat&, const TandemRepeat&) = default;
};

inline std::vector<TandemRepeat> main_lorentz_tandem_repeats(
    const std::string& text
){
    std::vector<TandemRepeat> result;
    for(const StringRun& run: enumerate_runs(text)){
        const int run_length = run.right - run.left;
        for(int half = run.period; half <= run_length / 2;
            half += run.period){
            for(int begin = run.left; begin <= run.right - half * 2; ++begin){
                result.push_back({begin, half});
            }
        }
    }
    std::sort(result.begin(), result.end(), [](const TandemRepeat& left,
                                              const TandemRepeat& right){
        return std::pair(left.begin, left.half_length)
            < std::pair(right.begin, right.half_length);
    });
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAIN_LORENTZ_HPP_INCLUDED
