#pragma once

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include "rectangle_packing_common.hpp"

namespace approximate::packing {

enum class GuillotineSplit{
    shorter_leftover_axis,
    longer_leftover_axis,
    shorter_axis,
    longer_axis,
};

namespace rectangle_packing_internal {

template<class Size>
struct GuillotineFreeRectangle{
    Size x{};
    Size y{};
    Size width{};
    Size height{};
};

template<class Size>
struct GuillotineCandidate{
    bool valid{};
    std::size_t bin{};
    std::size_t free_rectangle{};
    Size width{};
    Size height{};
    bool rotated{};
    long double area_waste{};
    long double short_side{};
};

template<class Size>
[[nodiscard]] bool better_guillotine_candidate(
    const GuillotineCandidate<Size>& candidate,
    const GuillotineCandidate<Size>& current
){
    if(!current.valid) return true;
    return std::tie(
        candidate.area_waste,
        candidate.short_side,
        candidate.bin,
        candidate.free_rectangle,
        candidate.rotated
    ) < std::tie(
        current.area_waste,
        current.short_side,
        current.bin,
        current.free_rectangle,
        current.rotated
    );
}

template<class Size>
void split_guillotine_free_rectangle(
    std::vector<GuillotineFreeRectangle<Size>>& free_rectangles,
    std::size_t index,
    const Size& width,
    const Size& height,
    GuillotineSplit split
){
    const auto free = free_rectangles[index];
    free_rectangles.erase(free_rectangles.begin() + static_cast<std::ptrdiff_t>(index));
    const Size free_right = free.x + free.width;
    const Size free_top = free.y + free.height;
    const Size placed_right = free.x + width;
    const Size placed_top = free.y + height;
    const Size width_left = extent_between(placed_right, free_right);
    const Size height_left = extent_between(placed_top, free_top);
    bool full_height_right = false;
    if(split == GuillotineSplit::shorter_leftover_axis){
        full_height_right = width_left <= height_left;
    }else if(split == GuillotineSplit::longer_leftover_axis){
        full_height_right = height_left < width_left;
    }else if(split == GuillotineSplit::shorter_axis){
        full_height_right = free.width <= free.height;
    }else{
        full_height_right = free.height < free.width;
    }
    if(full_height_right){
        if(Size{} < width_left){
            free_rectangles.push_back({
                placed_right, free.y, width_left, free.height,
            });
        }
        if(Size{} < height_left){
            free_rectangles.push_back({
                free.x, placed_top, width, height_left,
            });
        }
    }else{
        if(Size{} < width_left){
            free_rectangles.push_back({
                placed_right, free.y, width_left, height,
            });
        }
        if(Size{} < height_left){
            free_rectangles.push_back({
                free.x, placed_top, free.width, height_left,
            });
        }
    }
}

}  // namespace rectangle_packing_internal

template<class Size>
RectanglePackingResult<Size> guillotine_best_area_fit_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false,
    GuillotineSplit split = GuillotineSplit::shorter_leftover_axis
){
    using namespace rectangle_packing_internal;
    validate_rectangles(rectangles, bin_width, bin_height, allow_rotation);
    auto result = make_result<Size>(rectangles.size(), bin_width, bin_height);
    std::vector<std::vector<GuillotineFreeRectangle<Size>>> bins;

    for(std::size_t item = 0; item < rectangles.size(); ++item){
        GuillotineCandidate<Size> chosen;
        for(std::size_t bin = 0; bin < bins.size(); ++bin){
            for(std::size_t free_index = 0; free_index < bins[bin].size(); ++free_index){
                const auto& free = bins[bin][free_index];
                for_each_orientation(rectangles[item], allow_rotation,
                    [&](const Size& width, const Size& height, bool rotated){
                        if(width > free.width || height > free.height
                            || !fits_extent(free.x, width, bin_width)
                            || !fits_extent(free.y, height, bin_height)) return;
                        const Size width_left = free.width - width;
                        const Size height_left = free.height - height;
                        GuillotineCandidate<Size> candidate;
                        candidate.valid = true;
                        candidate.bin = bin;
                        candidate.free_rectangle = free_index;
                        candidate.width = width;
                        candidate.height = height;
                        candidate.rotated = rotated;
                        candidate.area_waste = normalized_area_waste(
                            free.width,
                            free.height,
                            width,
                            height,
                            bin_width,
                            bin_height
                        );
                        candidate.short_side = std::min(
                            static_cast<long double>(width_left),
                            static_cast<long double>(height_left)
                        );
                        if(better_guillotine_candidate(candidate, chosen)) chosen = candidate;
                    }
                );
            }
        }
        if(!chosen.valid){
            const std::size_t new_bin = bins.size();
            bins.push_back({GuillotineFreeRectangle<Size>{
                Size{}, Size{}, bin_width, bin_height,
            }});
            for_each_orientation(rectangles[item], allow_rotation,
                [&](const Size& width, const Size& height, bool rotated){
                    if(chosen.valid || width > bin_width || height > bin_height) return;
                    chosen = GuillotineCandidate<Size>{
                        true, new_bin, 0, width, height, rotated, 0, 0,
                    };
                }
            );
        }
        const auto& free = bins[chosen.bin][chosen.free_rectangle];
        const Size x = free.x;
        const Size y = free.y;
        split_guillotine_free_rectangle(
            bins[chosen.bin],
            chosen.free_rectangle,
            chosen.width,
            chosen.height,
            split
        );
        result.placements[item] = RectanglePlacement<Size>{
            chosen.bin,
            x,
            y,
            chosen.width,
            chosen.height,
            chosen.rotated,
        };
    }
    result.used_bin_count = bins.size();
    return result;
}

}  // namespace approximate::packing
