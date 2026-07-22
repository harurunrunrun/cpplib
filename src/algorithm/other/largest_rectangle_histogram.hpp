#ifndef CPPLIB_SRC_ALGORITHM_OTHER_LARGEST_RECTANGLE_HISTOGRAM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_LARGEST_RECTANGLE_HISTOGRAM_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<class T>
struct LargestRectangleHistogramResult{
    using area_type = std::common_type_t<T, long long>;

    area_type area{};
    int left = 0;
    int right = 0;
    T height{};
};

template<class T>
LargestRectangleHistogramResult<T> largest_rectangle_histogram(
    const std::vector<T>& heights
){
    using Result = LargestRectangleHistogramResult<T>;
    using Area = typename Result::area_type;

    const int n = static_cast<int>(heights.size());
    std::vector<int> stack;
    stack.reserve(heights.size());
    Result result;

    for(int i = 0; i <= n; i++){
        const T current = i == n ? T() : heights[static_cast<std::size_t>(i)];
        if(i < n && current < T())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative height (largest_rectangle_histogram)."
            );
        }
        while(!stack.empty() && !(heights[static_cast<std::size_t>(stack.back())] < current)){
            const int height_index = stack.back();
            stack.pop_back();
            const int left = stack.empty() ? 0 : stack.back() + 1;
            const Area area = static_cast<Area>(heights[static_cast<std::size_t>(height_index)]) *
                static_cast<Area>(i - left);
            if(result.area < area){
                result.area = area;
                result.left = left;
                result.right = i;
                result.height = heights[static_cast<std::size_t>(height_index)];
            }
        }
        if(i < n) stack.push_back(i);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_LARGEST_RECTANGLE_HISTOGRAM_HPP_INCLUDED
