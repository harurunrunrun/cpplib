#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_RECTANGLE_BATCH_COST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_RECTANGLE_BATCH_COST_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

struct RectangleBatchDimensions {
    long long width;
    long long height;
};

inline long long minimum_rectangle_batch_cost(
    std::vector<RectangleBatchDimensions> rectangles
){
    if(rectangles.empty()) return 0;
    for(const auto& rectangle: rectangles){
        if(rectangle.width <= 0 || rectangle.height <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: rectangle dimensions must be positive "
                "(minimum_rectangle_batch_cost)."
            );
        }
    }

    std::sort(
        rectangles.begin(),
        rectangles.end(),
        [](const auto& left, const auto& right){
            if(left.width != right.width) return left.width < right.width;
            return left.height > right.height;
        }
    );
    std::vector<RectangleBatchDimensions> distinct_widths;
    distinct_widths.reserve(rectangles.size());
    for(const auto& rectangle: rectangles){
        if(!distinct_widths.empty() &&
           distinct_widths.back().width == rectangle.width) continue;
        distinct_widths.push_back(rectangle);
    }

    std::vector<RectangleBatchDimensions> reduced;
    reduced.reserve(distinct_widths.size());
    for(const auto& rectangle: distinct_widths){
        while(!reduced.empty() && reduced.back().height <= rectangle.height){
            reduced.pop_back();
        }
        reduced.push_back(rectangle);
    }

    struct Line {
        long long slope;
        long long intercept;

        __int128 evaluate(long long x) const {
            return static_cast<__int128>(slope) * x + intercept;
        }
    };
    auto redundant = [](const Line& first, const Line& second, const Line& third){
        const __int128 left =
            (static_cast<__int128>(second.intercept) - first.intercept)
            * (static_cast<__int128>(second.slope) - third.slope);
        const __int128 right =
            (static_cast<__int128>(third.intercept) - second.intercept)
            * (static_cast<__int128>(first.slope) - second.slope);
        return left >= right;
    };

    std::vector<Line> hull;
    hull.reserve(reduced.size());
    std::size_t first_line = 0;
    hull.push_back({reduced.front().height, 0});

    long long answer = 0;
    for(std::size_t index = 0; index < reduced.size(); index++){
        const long long width = reduced[index].width;
        while(first_line + 1 < hull.size() &&
              hull[first_line + 1].evaluate(width) <=
                  hull[first_line].evaluate(width)){
            first_line++;
        }

        const __int128 value = hull[first_line].evaluate(width);
        if(value > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: result overflow "
                "(minimum_rectangle_batch_cost)."
            );
        }
        answer = static_cast<long long>(value);
        if(index + 1 == reduced.size()) break;

        const Line next{reduced[index + 1].height, answer};
        while(hull.size() >= first_line + 2 &&
              redundant(hull[hull.size() - 2], hull.back(), next)){
            hull.pop_back();
        }
        hull.push_back(next);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_RECTANGLE_BATCH_COST_HPP_INCLUDED
