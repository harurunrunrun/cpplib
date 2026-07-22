#ifndef CPPLIB_SRC_APPROXIMATE_PACKING_RECTANGLE_PACKING_COMMON_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_PACKING_RECTANGLE_PACKING_COMMON_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::packing {

template<class Size>
struct RectangleSize{
    Size width{};
    Size height{};
};

template<class Size>
struct RectanglePlacement{
    std::size_t bin{};
    Size x{};
    Size y{};
    Size width{};
    Size height{};
    bool rotated{};
};

template<class Size>
struct RectanglePackingResult{
    std::vector<RectanglePlacement<Size>> placements;
    std::size_t used_bin_count{};
    Size bin_width{};
    Size bin_height{};

    [[nodiscard]] std::size_t bin_count() const noexcept{
        return used_bin_count;
    }
};

namespace rectangle_packing_internal {

template<class Size>
[[nodiscard]] bool positive_finite(const Size& value){
    if(!(Size{} < value)) return false;
    if constexpr(std::is_floating_point_v<Size>){
        return std::isfinite(value);
    }
    return true;
}

template<class Size>
[[nodiscard]] bool fits_empty(
    const RectangleSize<Size>& rectangle,
    const Size& bin_width,
    const Size& bin_height
){
    return rectangle.width <= bin_width && rectangle.height <= bin_height;
}

template<class Size>
void validate_rectangles(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation
){
    if(!positive_finite(bin_width) || !positive_finite(bin_height))[[unlikely]]{
        throw std::invalid_argument("bin dimensions must be positive and finite");
    }
    for(const auto& rectangle: rectangles){
        if(!positive_finite(rectangle.width) || !positive_finite(rectangle.height))[[unlikely]]{
            throw std::invalid_argument("rectangle dimensions must be positive and finite");
        }
        const bool normal = fits_empty(rectangle, bin_width, bin_height);
        const RectangleSize<Size> rotated{rectangle.height, rectangle.width};
        if(!normal && !(allow_rotation && fits_empty(rotated, bin_width, bin_height)))[[unlikely]]{
            throw std::invalid_argument("rectangle does not fit an empty bin");
        }
    }
}

template<class Size, class Function>
void for_each_orientation(
    const RectangleSize<Size>& rectangle,
    bool allow_rotation,
    Function&& function
){
    function(rectangle.width, rectangle.height, false);
    if(allow_rotation && rectangle.width != rectangle.height){
        function(rectangle.height, rectangle.width, true);
    }
}

template<class Size>
[[nodiscard]] bool fits_extent(
    const Size& start,
    const Size& extent,
    const Size& limit
){
    if(start > limit || extent > limit || start > limit - extent) return false;
    if constexpr(std::is_floating_point_v<Size>){
        const Size end = start + extent;
        return std::isfinite(end) && end <= limit;
    }
    return true;
}

template<class Size>
[[nodiscard]] Size extent_between(const Size& start, const Size& end){
    Size extent = end - start;
    if constexpr(std::is_floating_point_v<Size>){
        const Size restored_end = start + extent;
        if(!std::isfinite(restored_end) || end < restored_end){
            extent = std::nextafter(extent, Size{});
        }
    }
    return extent;
}

template<class Size>
[[nodiscard]] bool fits_at(
    const Size& x,
    const Size& y,
    const Size& width,
    const Size& height,
    const Size& bin_width,
    const Size& bin_height
){
    return fits_extent(x, width, bin_width)
        && fits_extent(y, height, bin_height);
}

template<class Size>
[[nodiscard]] bool overlaps(
    const RectanglePlacement<Size>& first,
    const Size& x,
    const Size& y,
    const Size& width,
    const Size& height
){
    return first.x < x + width && x < first.x + first.width
        && first.y < y + height && y < first.y + first.height;
}

template<class Size>
[[nodiscard]] long double normalized_length(const Size& value, const Size& whole){
    return static_cast<long double>(value) / static_cast<long double>(whole);
}

template<class Size>
[[nodiscard]] long double normalized_area_waste(
    const Size& outer_width,
    const Size& outer_height,
    const Size& inner_width,
    const Size& inner_height,
    const Size& bin_width,
    const Size& bin_height
){
    return normalized_length(outer_width, bin_width)
            * normalized_length(outer_height, bin_height)
        - normalized_length(inner_width, bin_width)
            * normalized_length(inner_height, bin_height);
}

template<class Size>
[[nodiscard]] std::vector<std::size_t> rectangle_order(
    const std::vector<RectangleSize<Size>>& rectangles,
    bool decreasing_height
){
    std::vector<std::size_t> order(rectangles.size());
    for(std::size_t index = 0; index < order.size(); ++index) order[index] = index;
    if(decreasing_height){
        std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
            if(rectangles[first].height != rectangles[second].height){
                return rectangles[second].height < rectangles[first].height;
            }
            return rectangles[second].width < rectangles[first].width;
        });
    }
    return order;
}

template<class Size>
[[nodiscard]] RectanglePackingResult<Size> make_result(
    std::size_t rectangle_count,
    const Size& bin_width,
    const Size& bin_height
){
    RectanglePackingResult<Size> result;
    result.placements.resize(rectangle_count);
    result.bin_width = bin_width;
    result.bin_height = bin_height;
    return result;
}

}  // namespace rectangle_packing_internal

}  // namespace approximate::packing

#endif  // CPPLIB_SRC_APPROXIMATE_PACKING_RECTANGLE_PACKING_COMMON_HPP_INCLUDED
