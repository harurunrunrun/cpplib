#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

#include "rectangle_packing_common.hpp"

namespace approximate::packing {

enum class ShelfFit{
    next_fit,
    first_fit,
    best_width_fit,
    best_height_fit,
};

namespace rectangle_packing_internal {

template<class Size>
struct ShelfState{
    Size y{};
    Size height{};
    Size used_width{};
};

template<class Size>
struct ShelfCandidate{
    bool valid{};
    std::size_t bin{};
    std::size_t shelf{};
    Size width{};
    Size height{};
    bool rotated{};
    long double primary{};
    long double secondary{};
};

template<class Size>
[[nodiscard]] bool better_shelf_candidate(
    const ShelfCandidate<Size>& candidate,
    const ShelfCandidate<Size>& current
){
    if(!current.valid) return true;
    return std::tie(
        candidate.primary,
        candidate.secondary,
        candidate.bin,
        candidate.shelf,
        candidate.rotated
    ) < std::tie(
        current.primary,
        current.secondary,
        current.bin,
        current.shelf,
        current.rotated
    );
}

}  // namespace rectangle_packing_internal

template<class Size>
RectanglePackingResult<Size> shelf_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    ShelfFit fit,
    bool allow_rotation = false,
    bool decreasing_height = false
){
    using namespace rectangle_packing_internal;
    validate_rectangles(rectangles, bin_width, bin_height, allow_rotation);
    auto result = make_result<Size>(rectangles.size(), bin_width, bin_height);
    std::vector<std::vector<ShelfState<Size>>> bins;

    for(const std::size_t item: rectangle_order(rectangles, decreasing_height)){
        ShelfCandidate<Size> chosen;
        const auto inspect_shelf = [&](std::size_t bin, std::size_t shelf){
            const auto& state = bins[bin][shelf];
            for_each_orientation(rectangles[item], allow_rotation,
                [&](const Size& width, const Size& height, bool rotated){
                    if(height > state.height
                        || !fits_extent(state.used_width, width, bin_width)) return;
                    ShelfCandidate<Size> candidate;
                    candidate.valid = true;
                    candidate.bin = bin;
                    candidate.shelf = shelf;
                    candidate.width = width;
                    candidate.height = height;
                    candidate.rotated = rotated;
                    if(fit == ShelfFit::best_width_fit){
                        candidate.primary = normalized_length(
                            bin_width - state.used_width - width,
                            bin_width
                        );
                        candidate.secondary = normalized_length(state.height - height, bin_height);
                    }else if(fit == ShelfFit::best_height_fit){
                        candidate.primary = normalized_length(state.height - height, bin_height);
                        candidate.secondary = normalized_length(
                            bin_width - state.used_width - width,
                            bin_width
                        );
                    }
                    if(better_shelf_candidate(candidate, chosen)) chosen = candidate;
                }
            );
        };

        if(fit == ShelfFit::next_fit){
            if(!bins.empty() && !bins.back().empty()){
                inspect_shelf(bins.size() - 1, bins.back().size() - 1);
            }
        }else if(fit == ShelfFit::first_fit){
            for(std::size_t bin = 0; bin < bins.size() && !chosen.valid; ++bin){
                for(std::size_t shelf = 0; shelf < bins[bin].size() && !chosen.valid; ++shelf){
                    inspect_shelf(bin, shelf);
                }
            }
        }else{
            for(std::size_t bin = 0; bin < bins.size(); ++bin){
                for(std::size_t shelf = 0; shelf < bins[bin].size(); ++shelf){
                    inspect_shelf(bin, shelf);
                }
            }
        }

        if(!chosen.valid){
            const auto try_open_shelf = [&](std::size_t bin, auto&& accept){
                const Size y = bins[bin].empty()
                    ? Size{}
                    : bins[bin].back().y + bins[bin].back().height;
                for_each_orientation(rectangles[item], allow_rotation,
                    [&](const Size& width, const Size& height, bool rotated){
                        if(chosen.valid || width > bin_width
                            || !fits_extent(y, height, bin_height)) return;
                        chosen.valid = true;
                        chosen.bin = bin;
                        chosen.shelf = bins[bin].size();
                        chosen.width = width;
                        chosen.height = height;
                        chosen.rotated = rotated;
                        accept();
                    }
                );
            };

            if(fit == ShelfFit::next_fit && !bins.empty()){
                try_open_shelf(bins.size() - 1, []{});
            }else if(fit != ShelfFit::next_fit){
                for(std::size_t bin = 0; bin < bins.size() && !chosen.valid; ++bin){
                    try_open_shelf(bin, []{});
                }
            }
            if(!chosen.valid){
                bins.emplace_back();
                try_open_shelf(bins.size() - 1, []{});
            }
            bins[chosen.bin].push_back(ShelfState<Size>{
                bins[chosen.bin].empty()
                    ? Size{}
                    : bins[chosen.bin].back().y + bins[chosen.bin].back().height,
                chosen.height,
                Size{},
            });
        }

        auto& shelf = bins[chosen.bin][chosen.shelf];
        result.placements[item] = RectanglePlacement<Size>{
            chosen.bin,
            shelf.used_width,
            shelf.y,
            chosen.width,
            chosen.height,
            chosen.rotated,
        };
        shelf.used_width += chosen.width;
    }
    result.used_bin_count = bins.size();
    return result;
}

template<class Size>
RectanglePackingResult<Size> shelf_next_fit(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::next_fit, allow_rotation, false
    );
}

template<class Size>
RectanglePackingResult<Size> shelf_first_fit(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::first_fit, allow_rotation, false
    );
}

template<class Size>
RectanglePackingResult<Size> shelf_best_width_fit(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::best_width_fit, allow_rotation, false
    );
}

template<class Size>
RectanglePackingResult<Size> shelf_best_height_fit(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::best_height_fit, allow_rotation, false
    );
}

template<class Size>
RectanglePackingResult<Size> nfdh_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::next_fit, allow_rotation, true
    );
}

template<class Size>
RectanglePackingResult<Size> ffdh_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::first_fit, allow_rotation, true
    );
}

template<class Size>
RectanglePackingResult<Size> bfdh_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::best_width_fit, allow_rotation, true
    );
}

}  // namespace approximate::packing
