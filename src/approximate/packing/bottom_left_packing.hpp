#ifndef CPPLIB_SRC_APPROXIMATE_PACKING_BOTTOM_LEFT_PACKING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_PACKING_BOTTOM_LEFT_PACKING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include "rectangle_packing_common.hpp"

namespace approximate::packing {

namespace rectangle_packing_internal {

template<class Size>
struct BottomLeftCandidate{
    bool valid{};
    std::size_t bin{};
    Size x{};
    Size y{};
    Size width{};
    Size height{};
    bool rotated{};
};

template<class Size>
[[nodiscard]] bool bottom_left_placeable(
    const std::vector<RectanglePlacement<Size>>& placed,
    const Size& x,
    const Size& y,
    const Size& width,
    const Size& height,
    const Size& bin_width,
    const Size& bin_height
){
    if(!fits_at(x, y, width, height, bin_width, bin_height)) return false;
    for(const auto& rectangle: placed){
        if(overlaps(rectangle, x, y, width, height)) return false;
    }
    return true;
}

template<class Size>
[[nodiscard]] std::vector<std::pair<Size, Size>> bottom_left_points(
    const std::vector<RectanglePlacement<Size>>& placed,
    bool fill_holes
){
    std::vector<std::pair<Size, Size>> points;
    if(fill_holes){
        std::vector<Size> x_coordinates{Size{}};
        std::vector<Size> y_coordinates{Size{}};
        x_coordinates.reserve(placed.size() + 1);
        y_coordinates.reserve(placed.size() + 1);
        for(const auto& rectangle: placed){
            x_coordinates.push_back(rectangle.x + rectangle.width);
            y_coordinates.push_back(rectangle.y + rectangle.height);
        }
        std::sort(x_coordinates.begin(), x_coordinates.end());
        x_coordinates.erase(
            std::unique(x_coordinates.begin(), x_coordinates.end()),
            x_coordinates.end()
        );
        std::sort(y_coordinates.begin(), y_coordinates.end());
        y_coordinates.erase(
            std::unique(y_coordinates.begin(), y_coordinates.end()),
            y_coordinates.end()
        );
        points.reserve(x_coordinates.size() * y_coordinates.size());
        for(const Size& y: y_coordinates){
            for(const Size& x: x_coordinates) points.emplace_back(x, y);
        }
    }else{
        points.emplace_back(Size{}, Size{});
        points.reserve(placed.size() * 4 + 1);
        for(const auto& rectangle: placed){
            const Size right = rectangle.x + rectangle.width;
            const Size top = rectangle.y + rectangle.height;
            points.emplace_back(right, rectangle.y);
            points.emplace_back(rectangle.x, top);
            points.emplace_back(right, Size{});
            points.emplace_back(Size{}, top);
        }
    }
    std::sort(points.begin(), points.end(), [](const auto& first, const auto& second){
        return std::tie(first.second, first.first) < std::tie(second.second, second.first);
    });
    points.erase(std::unique(points.begin(), points.end()), points.end());
    return points;
}

}  // namespace rectangle_packing_internal

template<class Size>
RectanglePackingResult<Size> bottom_left_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false,
    bool fill_holes = false
){
    using namespace rectangle_packing_internal;
    validate_rectangles(rectangles, bin_width, bin_height, allow_rotation);
    auto result = make_result<Size>(rectangles.size(), bin_width, bin_height);
    std::vector<std::vector<RectanglePlacement<Size>>> bins;

    for(std::size_t item = 0; item < rectangles.size(); ++item){
        BottomLeftCandidate<Size> chosen;
        for(std::size_t bin = 0; bin < bins.size() && !chosen.valid; ++bin){
            const auto points = bottom_left_points(bins[bin], fill_holes);
            for(const auto& [x, y]: points){
                for_each_orientation(rectangles[item], allow_rotation,
                    [&](const Size& width, const Size& height, bool rotated){
                        if(!bottom_left_placeable(
                            bins[bin], x, y, width, height, bin_width, bin_height
                        )) return;
                        BottomLeftCandidate<Size> candidate{
                            true, bin, x, y, width, height, rotated,
                        };
                        if(!chosen.valid || std::tie(candidate.y, candidate.x, candidate.rotated)
                            < std::tie(chosen.y, chosen.x, chosen.rotated)){
                            chosen = candidate;
                        }
                    }
                );
            }
        }
        if(!chosen.valid){
            const std::size_t new_bin = bins.size();
            for_each_orientation(rectangles[item], allow_rotation,
                [&](const Size& width, const Size& height, bool rotated){
                    if(chosen.valid || width > bin_width || height > bin_height) return;
                    chosen = BottomLeftCandidate<Size>{
                        true, new_bin, Size{}, Size{}, width, height, rotated,
                    };
                }
            );
            bins.emplace_back();
        }
        const RectanglePlacement<Size> placement{
            chosen.bin,
            chosen.x,
            chosen.y,
            chosen.width,
            chosen.height,
            chosen.rotated,
        };
        bins[chosen.bin].push_back(placement);
        result.placements[item] = placement;
    }
    result.used_bin_count = bins.size();
    return result;
}

template<class Size>
RectanglePackingResult<Size> bottom_left_fill_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation = false
){
    return bottom_left_rectangle_packing(
        rectangles, bin_width, bin_height, allow_rotation, true
    );
}

}  // namespace approximate::packing

#endif  // CPPLIB_SRC_APPROXIMATE_PACKING_BOTTOM_LEFT_PACKING_HPP_INCLUDED
