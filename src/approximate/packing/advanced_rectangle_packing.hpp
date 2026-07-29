#ifndef CPPLIB_SRC_APPROXIMATE_PACKING_ADVANCED_RECTANGLE_PACKING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_PACKING_ADVANCED_RECTANGLE_PACKING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

#include "bottom_left_packing.hpp"
#include "guillotine_packing.hpp"
#include "rectangle_packing_common.hpp"

namespace approximate::packing {

namespace advanced_rectangle_packing_internal {

template<class Size>
RectanglePackingResult<Size> restore_order(
    RectanglePackingResult<Size> reordered,
    const std::vector<std::size_t>& order
) {
    RectanglePackingResult<Size> result;
    result.placements.resize(order.size());
    result.used_bin_count = reordered.used_bin_count;
    result.bin_width = reordered.bin_width;
    result.bin_height = reordered.bin_height;
    for(std::size_t position = 0; position < order.size(); ++position){
        result.placements[order[position]] = reordered.placements[position];
    }
    return result;
}

template<class Size>
struct FloorCeilingBin {
    Size floor_boundary{};
    Size ceiling_boundary{};
    Size floor_x{};
    Size floor_height{};
    Size floor_y{};
    Size ceiling_x{};
    Size ceiling_height{};
    Size ceiling_y{};
    bool has_floor = false;
    bool has_ceiling = false;
    bool next_floor = true;
};

template<class Size>
bool try_floor_ceiling_orientation(
    FloorCeilingBin<Size>& bin,
    const Size& width,
    const Size& height,
    const Size& bin_width,
    RectanglePlacement<Size>& placement,
    std::size_t bin_index,
    bool rotated
) {
    if(bin.has_floor && height <= bin.floor_height
       && rectangle_packing_internal::fits_extent(
            bin.floor_x, width, bin_width
       )){
        placement = {
            bin_index, bin.floor_x, bin.floor_y,
            width, height, rotated
        };
        bin.floor_x += width;
        return true;
    }
    if(bin.has_ceiling && height <= bin.ceiling_height
       && rectangle_packing_internal::fits_extent(
            bin.ceiling_x, width, bin_width
       )){
        placement = {
            bin_index, bin.ceiling_x, bin.ceiling_y,
            width, height, rotated
        };
        bin.ceiling_x += width;
        return true;
    }
    if(bin.ceiling_boundary < bin.floor_boundary
       || bin.ceiling_boundary - bin.floor_boundary < height){
        return false;
    }
    const bool use_floor = bin.next_floor;
    if(use_floor){
        placement = {
            bin_index, Size{}, bin.floor_boundary,
            width, height, rotated
        };
        bin.floor_y = bin.floor_boundary;
        bin.floor_height = height;
        bin.floor_x = width;
        bin.floor_boundary += height;
        bin.has_floor = true;
    }else{
        bin.ceiling_boundary -= height;
        placement = {
            bin_index, Size{}, bin.ceiling_boundary,
            width, height, rotated
        };
        bin.ceiling_y = bin.ceiling_boundary;
        bin.ceiling_height = height;
        bin.ceiling_x = width;
        bin.has_ceiling = true;
    }
    bin.next_floor = !bin.next_floor;
    return true;
}

}  // namespace advanced_rectangle_packing_internal

template<class Size>
[[nodiscard]] RectanglePackingResult<Size> split_fit_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
) {
    rectangle_packing_internal::validate_rectangles(
        rectangles, bin_width, bin_height, allow_rotation
    );
    std::vector<std::size_t> order(rectangles.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    const long double half_width =
        static_cast<long double>(bin_width) * 0.5L;
    std::stable_sort(
        order.begin(), order.end(),
        [&](std::size_t first, std::size_t second){
            const bool first_wide =
                static_cast<long double>(rectangles[first].width)
                > half_width;
            const bool second_wide =
                static_cast<long double>(rectangles[second].width)
                > half_width;
            if(first_wide != second_wide) return first_wide;
            if(rectangles[first].height != rectangles[second].height){
                return rectangles[second].height
                    < rectangles[first].height;
            }
            return first < second;
        }
    );
    std::vector<RectangleSize<Size>> reordered;
    reordered.reserve(rectangles.size());
    for(const std::size_t item : order) reordered.push_back(rectangles[item]);
    return advanced_rectangle_packing_internal::restore_order(
        bottom_left_fill_rectangle_packing(
            reordered, bin_width, bin_height, allow_rotation
        ),
        order
    );
}

template<class Size>
[[nodiscard]] RectanglePackingResult<Size> floor_ceiling_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
) {
    using namespace advanced_rectangle_packing_internal;
    rectangle_packing_internal::validate_rectangles(
        rectangles, bin_width, bin_height, allow_rotation
    );
    const std::vector<std::size_t> order =
        rectangle_packing_internal::rectangle_order(rectangles, true);
    RectanglePackingResult<Size> result =
        rectangle_packing_internal::make_result<Size>(
            rectangles.size(), bin_width, bin_height
        );
    std::vector<FloorCeilingBin<Size>> bins;
    for(const std::size_t item : order){
        bool placed = false;
        for(std::size_t bin_index = 0; bin_index < bins.size() && !placed;
            ++bin_index){
            rectangle_packing_internal::for_each_orientation(
                rectangles[item], allow_rotation,
                [&](const Size& width, const Size& height, bool rotated){
                    if(placed) return;
                    FloorCeilingBin<Size> trial = bins[bin_index];
                    RectanglePlacement<Size> candidate;
                    if(try_floor_ceiling_orientation(
                        trial, width, height, bin_width,
                        candidate, bin_index, rotated
                    )){
                        bins[bin_index] = trial;
                        result.placements[item] = candidate;
                        placed = true;
                    }
                }
            );
        }
        if(placed) continue;
        FloorCeilingBin<Size> bin;
        bin.ceiling_boundary = bin_height;
        const std::size_t bin_index = bins.size();
        RectanglePlacement<Size> candidate;
        bool initialized = false;
        rectangle_packing_internal::for_each_orientation(
            rectangles[item], allow_rotation,
            [&](const Size& width, const Size& height, bool rotated){
                if(initialized) return;
                initialized = try_floor_ceiling_orientation(
                    bin, width, height, bin_width,
                    candidate, bin_index, rotated
                );
            }
        );
        bins.push_back(bin);
        result.placements[item] = candidate;
    }
    result.used_bin_count = bins.size();
    return result;
}

template<class Size>
[[nodiscard]] RectanglePackingResult<Size> steinberg_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
) {
    rectangle_packing_internal::validate_rectangles(
        rectangles, bin_width, bin_height, allow_rotation
    );
    std::vector<std::size_t> order(rectangles.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    const long double normalized_width =
        static_cast<long double>(bin_width);
    const long double normalized_height =
        static_cast<long double>(bin_height);
    std::stable_sort(
        order.begin(), order.end(),
        [&](std::size_t first, std::size_t second){
            const auto priority = [&](std::size_t item){
                const long double width =
                    static_cast<long double>(rectangles[item].width);
                const long double height =
                    static_cast<long double>(rectangles[item].height);
                const long double excess =
                    std::max(2.0L * width - normalized_width, 0.0L)
                    * std::max(2.0L * height - normalized_height, 0.0L);
                const long double area = width * height;
                return std::pair{excess, area};
            };
            const auto first_priority = priority(first);
            const auto second_priority = priority(second);
            if(first_priority != second_priority){
                return second_priority < first_priority;
            }
            return first < second;
        }
    );
    std::vector<RectangleSize<Size>> reordered;
    reordered.reserve(rectangles.size());
    for(const std::size_t item : order) reordered.push_back(rectangles[item]);
    return advanced_rectangle_packing_internal::restore_order(
        guillotine_best_area_fit_rectangle_packing(
            reordered, bin_width, bin_height, allow_rotation,
            GuillotineSplit::shorter_leftover_axis
        ),
        order
    );
}

}  // namespace approximate::packing

#endif  // CPPLIB_SRC_APPROXIMATE_PACKING_ADVANCED_RECTANGLE_PACKING_HPP_INCLUDED
