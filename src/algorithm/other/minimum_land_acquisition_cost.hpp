#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct LandPlotDimensions {
    long long width;
    long long height;
};

inline long long minimum_land_acquisition_cost(
    std::vector<LandPlotDimensions> plots
){
    if(plots.empty()) return 0;
    for(const auto& plot: plots){
        if(plot.width <= 0 || plot.height <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: nonpositive dimension "
                "(minimum_land_acquisition_cost)."
            );
        }
    }
    std::sort(plots.begin(), plots.end(), [](const auto& left, const auto& right){
        if(left.width != right.width) return left.width < right.width;
        return left.height > right.height;
    });
    std::vector<LandPlotDimensions> reduced;
    reduced.reserve(plots.size());
    for(const auto& plot: plots){
        while(!reduced.empty() && reduced.back().height <= plot.height){
            reduced.pop_back();
        }
        reduced.push_back(plot);
    }

    struct Line {
        long long slope;
        long long intercept;
        __int128 evaluate(long long x) const {
            return static_cast<__int128>(slope) * x + intercept;
        }
    };
    std::vector<Line> hull;
    hull.reserve(reduced.size());
    std::size_t first = 0;
    auto redundant = [](const Line& a, const Line& b, const Line& c){
        return static_cast<__int128>(b.intercept - a.intercept) *
                   (b.slope - c.slope) >=
               static_cast<__int128>(c.intercept - b.intercept) *
                   (a.slope - b.slope);
    };
    hull.push_back({reduced.front().height, 0});
    long long answer = 0;
    for(std::size_t index = 0; index < reduced.size(); index++){
        const long long x = reduced[index].width;
        while(first + 1 < hull.size() &&
              hull[first + 1].evaluate(x) <= hull[first].evaluate(x)) first++;
        const __int128 value = hull[first].evaluate(x);
        if(value > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: result overflow "
                "(minimum_land_acquisition_cost)."
            );
        }
        answer = static_cast<long long>(value);
        if(index + 1 == reduced.size()) break;
        const Line next{reduced[index + 1].height, answer};
        while(hull.size() >= first + 2 &&
              redundant(hull[hull.size() - 2], hull.back(), next)){
            hull.pop_back();
        }
        hull.push_back(next);
    }
    return answer;
}
