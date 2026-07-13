#pragma once

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include "rectangle_packing_common.hpp"

namespace approximate::packing {

enum class SkylineFit{
    bottom_left,
    best_fit,
};

namespace rectangle_packing_internal {

template<class Size>
struct SkylineNode{
    Size x{};
    Size width{};
    Size height{};
};

template<class Size>
struct SkylineCandidate{
    bool valid{};
    std::size_t bin{};
    std::size_t node{};
    Size x{};
    Size y{};
    Size width{};
    Size height{};
    bool rotated{};
    long double primary{};
    long double secondary{};
};

template<class Size>
[[nodiscard]] std::pair<Size, long double> skyline_level_and_waste(
    const std::vector<SkylineNode<Size>>& skyline,
    std::size_t start,
    const Size& width,
    const Size& bin_width,
    const Size& bin_height
){
    const Size x = skyline[start].x;
    const Size end = x + width;
    Size level{};
    for(std::size_t index = start; index < skyline.size() && skyline[index].x < end; ++index){
        if(level < skyline[index].height) level = skyline[index].height;
    }
    long double waste = 0;
    for(std::size_t index = start; index < skyline.size() && skyline[index].x < end; ++index){
        const Size node_end = skyline[index].x + skyline[index].width;
        const Size covered_end = node_end < end ? node_end : end;
        const Size covered_width = covered_end - skyline[index].x;
        waste += normalized_length(covered_width, bin_width)
            * normalized_length(level - skyline[index].height, bin_height);
    }
    return {level, waste};
}

template<class Size>
void skyline_append(
    std::vector<SkylineNode<Size>>& skyline,
    const Size& x,
    const Size& width,
    const Size& height
){
    if(!(Size{} < width)) return;
    if(!skyline.empty() && skyline.back().height == height
        && skyline.back().x + skyline.back().width == x){
        skyline.back().width = extent_between(skyline.back().x, x + width);
    }else{
        skyline.push_back(SkylineNode<Size>{x, width, height});
    }
}

template<class Size>
void update_skyline(
    std::vector<SkylineNode<Size>>& skyline,
    const Size& x,
    const Size& width,
    const Size& new_height
){
    const Size end = x + width;
    std::vector<SkylineNode<Size>> updated;
    updated.reserve(skyline.size() + 2);
    for(const auto& node: skyline){
        const Size node_end = node.x + node.width;
        if(node_end <= x || end <= node.x){
            skyline_append(updated, node.x, node.width, node.height);
            continue;
        }
        if(node.x < x){
            skyline_append(updated, node.x, extent_between(node.x, x), node.height);
        }
        const Size middle_left = node.x < x ? x : node.x;
        const Size middle_right = node_end < end ? node_end : end;
        skyline_append(
            updated, middle_left, extent_between(middle_left, middle_right), new_height
        );
        if(end < node_end){
            skyline_append(updated, end, extent_between(end, node_end), node.height);
        }
    }
    skyline.swap(updated);
}

template<class Size>
[[nodiscard]] bool better_skyline_candidate(
    const SkylineCandidate<Size>& candidate,
    const SkylineCandidate<Size>& current
){
    if(!current.valid) return true;
    return std::tie(
        candidate.primary,
        candidate.secondary,
        candidate.bin,
        candidate.node,
        candidate.rotated
    ) < std::tie(
        current.primary,
        current.secondary,
        current.bin,
        current.node,
        current.rotated
    );
}

}  // namespace rectangle_packing_internal

template<class Size>
RectanglePackingResult<Size> skyline_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    SkylineFit fit,
    bool allow_rotation = false
){
    using namespace rectangle_packing_internal;
    validate_rectangles(rectangles, bin_width, bin_height, allow_rotation);
    auto result = make_result<Size>(rectangles.size(), bin_width, bin_height);
    std::vector<std::vector<SkylineNode<Size>>> bins;

    for(std::size_t item = 0; item < rectangles.size(); ++item){
        SkylineCandidate<Size> chosen;
        for(std::size_t bin = 0; bin < bins.size(); ++bin){
            for(std::size_t node = 0; node < bins[bin].size(); ++node){
                const Size x = bins[bin][node].x;
                for_each_orientation(rectangles[item], allow_rotation,
                    [&](const Size& width, const Size& height, bool rotated){
                        if(!fits_extent(x, width, bin_width)) return;
                        const auto [y, waste] = skyline_level_and_waste(
                            bins[bin], node, width, bin_width, bin_height
                        );
                        if(!fits_extent(y, height, bin_height)) return;
                        SkylineCandidate<Size> candidate;
                        candidate.valid = true;
                        candidate.bin = bin;
                        candidate.node = node;
                        candidate.x = x;
                        candidate.y = y;
                        candidate.width = width;
                        candidate.height = height;
                        candidate.rotated = rotated;
                        if(fit == SkylineFit::bottom_left){
                            candidate.primary = normalized_length(y + height, bin_height);
                            candidate.secondary = normalized_length(x, bin_width);
                        }else{
                            candidate.primary = waste;
                            candidate.secondary = normalized_length(y + height, bin_height);
                        }
                        if(better_skyline_candidate(candidate, chosen)) chosen = candidate;
                    }
                );
            }
        }
        if(!chosen.valid){
            const std::size_t new_bin = bins.size();
            for_each_orientation(rectangles[item], allow_rotation,
                [&](const Size& width, const Size& height, bool rotated){
                    if(chosen.valid || width > bin_width || height > bin_height) return;
                    chosen = SkylineCandidate<Size>{
                        true,
                        new_bin,
                        0,
                        Size{},
                        Size{},
                        width,
                        height,
                        rotated,
                        normalized_length(height, bin_height),
                        0,
                    };
                }
            );
            bins.push_back({SkylineNode<Size>{Size{}, bin_width, Size{}}});
        }
        update_skyline(
            bins[chosen.bin], chosen.x, chosen.width, chosen.y + chosen.height
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
RectanglePackingResult<Size> skyline_bottom_left_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return skyline_rectangle_packing(
        rectangles, bin_width, bin_height, SkylineFit::bottom_left, allow_rotation
    );
}

template<class Size>
RectanglePackingResult<Size> skyline_best_fit_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return skyline_rectangle_packing(
        rectangles, bin_width, bin_height, SkylineFit::best_fit, allow_rotation
    );
}

}  // namespace approximate::packing
