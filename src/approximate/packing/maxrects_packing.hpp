#pragma once

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include "rectangle_packing_common.hpp"

namespace approximate::packing {

enum class MaxRectsFit{
    best_short_side_fit,
    best_long_side_fit,
    best_area_fit,
};

namespace rectangle_packing_internal {

template<class Size>
struct MaxRectsFreeRectangle{
    Size x{};
    Size y{};
    Size width{};
    Size height{};
};

template<class Size>
struct MaxRectsCandidate{
    bool valid{};
    std::size_t bin{};
    std::size_t free_rectangle{};
    Size x{};
    Size y{};
    Size width{};
    Size height{};
    bool rotated{};
    long double primary{};
    long double secondary{};
};

template<class Size>
[[nodiscard]] bool maxrects_contains(
    const MaxRectsFreeRectangle<Size>& outer,
    const MaxRectsFreeRectangle<Size>& inner
){
    const Size outer_right = outer.x + outer.width;
    const Size outer_top = outer.y + outer.height;
    return outer.x <= inner.x && inner.x <= outer_right
        && outer.y <= inner.y && inner.y <= outer_top
        && inner.width <= outer_right - inner.x
        && inner.height <= outer_top - inner.y;
}

template<class Size>
[[nodiscard]] bool maxrects_intersects(
    const MaxRectsFreeRectangle<Size>& free,
    const Size& x,
    const Size& y,
    const Size& width,
    const Size& height
){
    return free.x < x + width && x < free.x + free.width
        && free.y < y + height && y < free.y + free.height;
}

template<class Size>
void prune_maxrects_free_rectangles(
    std::vector<MaxRectsFreeRectangle<Size>>& free_rectangles
){
    std::vector<MaxRectsFreeRectangle<Size>> pruned;
    pruned.reserve(free_rectangles.size());
    for(std::size_t first = 0; first < free_rectangles.size(); ++first){
        bool contained = false;
        for(std::size_t second = 0; second < free_rectangles.size(); ++second){
            if(first == second) continue;
            if(maxrects_contains(free_rectangles[second], free_rectangles[first])
                && (!maxrects_contains(free_rectangles[first], free_rectangles[second])
                    || second < first)){
                contained = true;
                break;
            }
        }
        if(!contained) pruned.push_back(free_rectangles[first]);
    }
    free_rectangles.swap(pruned);
}

template<class Size>
void split_maxrects_free_rectangles(
    std::vector<MaxRectsFreeRectangle<Size>>& free_rectangles,
    const Size& x,
    const Size& y,
    const Size& width,
    const Size& height
){
    const Size right = x + width;
    const Size top = y + height;
    std::vector<MaxRectsFreeRectangle<Size>> split;
    split.reserve(free_rectangles.size() * 2 + 4);
    for(const auto& free: free_rectangles){
        if(!maxrects_intersects(free, x, y, width, height)){
            split.push_back(free);
            continue;
        }
        const Size free_right = free.x + free.width;
        const Size free_top = free.y + free.height;
        if(free.x < x){
            split.push_back({
                free.x, free.y, extent_between(free.x, x), free.height,
            });
        }
        if(right < free_right){
            split.push_back({
                right, free.y, extent_between(right, free_right), free.height,
            });
        }
        if(free.y < y){
            split.push_back({
                free.x, free.y, free.width, extent_between(free.y, y),
            });
        }
        if(top < free_top){
            split.push_back({
                free.x, top, free.width, extent_between(top, free_top),
            });
        }
    }
    prune_maxrects_free_rectangles(split);
    free_rectangles.swap(split);
}

template<class Size>
[[nodiscard]] bool better_maxrects_candidate(
    const MaxRectsCandidate<Size>& candidate,
    const MaxRectsCandidate<Size>& current
){
    if(!current.valid) return true;
    return std::tie(
        candidate.primary,
        candidate.secondary,
        candidate.bin,
        candidate.free_rectangle,
        candidate.rotated
    ) < std::tie(
        current.primary,
        current.secondary,
        current.bin,
        current.free_rectangle,
        current.rotated
    );
}

}  // namespace rectangle_packing_internal

template<class Size>
RectanglePackingResult<Size> maxrects_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    MaxRectsFit fit,
    bool allow_rotation = false
){
    using namespace rectangle_packing_internal;
    validate_rectangles(rectangles, bin_width, bin_height, allow_rotation);
    auto result = make_result<Size>(rectangles.size(), bin_width, bin_height);
    std::vector<std::vector<MaxRectsFreeRectangle<Size>>> bins;

    for(std::size_t item = 0; item < rectangles.size(); ++item){
        MaxRectsCandidate<Size> chosen;
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
                        const long double short_side = std::min(
                            static_cast<long double>(width_left),
                            static_cast<long double>(height_left)
                        );
                        const long double long_side = std::max(
                            static_cast<long double>(width_left),
                            static_cast<long double>(height_left)
                        );
                        MaxRectsCandidate<Size> candidate;
                        candidate.valid = true;
                        candidate.bin = bin;
                        candidate.free_rectangle = free_index;
                        candidate.x = free.x;
                        candidate.y = free.y;
                        candidate.width = width;
                        candidate.height = height;
                        candidate.rotated = rotated;
                        if(fit == MaxRectsFit::best_short_side_fit){
                            candidate.primary = short_side;
                            candidate.secondary = long_side;
                        }else if(fit == MaxRectsFit::best_long_side_fit){
                            candidate.primary = long_side;
                            candidate.secondary = short_side;
                        }else{
                            candidate.primary = normalized_area_waste(
                                free.width,
                                free.height,
                                width,
                                height,
                                bin_width,
                                bin_height
                            );
                            candidate.secondary = short_side;
                        }
                        if(better_maxrects_candidate(candidate, chosen)) chosen = candidate;
                    }
                );
            }
        }
        if(!chosen.valid){
            const std::size_t new_bin = bins.size();
            bins.push_back({MaxRectsFreeRectangle<Size>{
                Size{}, Size{}, bin_width, bin_height,
            }});
            for_each_orientation(rectangles[item], allow_rotation,
                [&](const Size& width, const Size& height, bool rotated){
                    if(chosen.valid || width > bin_width || height > bin_height) return;
                    chosen = MaxRectsCandidate<Size>{
                        true,
                        new_bin,
                        0,
                        Size{},
                        Size{},
                        width,
                        height,
                        rotated,
                        0,
                        0,
                    };
                }
            );
        }
        split_maxrects_free_rectangles(
            bins[chosen.bin],
            chosen.x,
            chosen.y,
            chosen.width,
            chosen.height
        );
        result.placements[item] = RectanglePlacement<Size>{
            chosen.bin,
            chosen.x,
            chosen.y,
            chosen.width,
            chosen.height,
            chosen.rotated,
        };
    }
    result.used_bin_count = bins.size();
    return result;
}

template<class Size>
RectanglePackingResult<Size> maxrects_best_short_side_fit(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return maxrects_rectangle_packing(
        rectangles, bin_width, bin_height, MaxRectsFit::best_short_side_fit, allow_rotation
    );
}

template<class Size>
RectanglePackingResult<Size> maxrects_best_long_side_fit(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return maxrects_rectangle_packing(
        rectangles, bin_width, bin_height, MaxRectsFit::best_long_side_fit, allow_rotation
    );
}

template<class Size>
RectanglePackingResult<Size> maxrects_best_area_fit(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return maxrects_rectangle_packing(
        rectangles, bin_width, bin_height, MaxRectsFit::best_area_fit, allow_rotation
    );
}

}  // namespace approximate::packing
