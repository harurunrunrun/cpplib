// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/packing/rectangle_packing.hpp"

namespace {

using approximate::packing::RectanglePackingResult;
using approximate::packing::RectangleSize;

template<class Size>
bool same_result(
    const RectanglePackingResult<Size>& first,
    const RectanglePackingResult<Size>& second
){
    if(first.bin_count() != second.bin_count()
        || first.placements.size() != second.placements.size()) return false;
    for(std::size_t index = 0; index < first.placements.size(); ++index){
        const auto& a = first.placements[index];
        const auto& b = second.placements[index];
        if(a.bin != b.bin || a.x != b.x || a.y != b.y
            || a.width != b.width || a.height != b.height
            || a.rotated != b.rotated) return false;
    }
    return true;
}

template<class Size>
bool valid_result(
    const RectanglePackingResult<Size>& result,
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation
){
    if(result.placements.size() != rectangles.size()) return false;
    if(result.bin_width != bin_width || result.bin_height != bin_height) return false;
    if(rectangles.empty()) return result.bin_count() == 0;
    if(result.bin_count() == 0 || result.bin_count() > rectangles.size()) return false;
    std::vector<bool> used(result.bin_count());
    for(std::size_t item = 0; item < rectangles.size(); ++item){
        const auto& placement = result.placements[item];
        if(placement.bin >= result.bin_count()) return false;
        used[placement.bin] = true;
        if(placement.rotated){
            if(!allow_rotation || placement.width != rectangles[item].height
                || placement.height != rectangles[item].width) return false;
        }else if(placement.width != rectangles[item].width
            || placement.height != rectangles[item].height){
            return false;
        }
        if(!approximate::packing::rectangle_packing_internal::fits_at(
            placement.x,
            placement.y,
            placement.width,
            placement.height,
            bin_width,
            bin_height
        )) return false;
    }
    for(const bool value: used){
        if(!value) return false;
    }
    for(std::size_t first = 0; first < rectangles.size(); ++first){
        const auto& a = result.placements[first];
        for(std::size_t second = first + 1; second < rectangles.size(); ++second){
            const auto& b = result.placements[second];
            if(a.bin != b.bin) continue;
            if(a.x < b.x + b.width && b.x < a.x + a.width
                && a.y < b.y + b.height && b.y < a.y + a.height) return false;
        }
    }
    return true;
}

template<class Size>
std::vector<RectanglePackingResult<Size>> run_all(
    const std::vector<RectangleSize<Size>>& rectangles,
    const Size& bin_width,
    const Size& bin_height,
    bool allow_rotation
){
    using namespace approximate::packing;
    const auto snf = shelf_next_fit(rectangles, bin_width, bin_height, allow_rotation);
    const auto sff = shelf_first_fit(rectangles, bin_width, bin_height, allow_rotation);
    const auto sbw = shelf_best_width_fit(rectangles, bin_width, bin_height, allow_rotation);
    const auto sbh = shelf_best_height_fit(rectangles, bin_width, bin_height, allow_rotation);
    if(!same_result(snf, shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::next_fit, allow_rotation, false
    ))) throw std::logic_error("Shelf Next Fit wrapper mismatch");
    if(!same_result(sff, shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::first_fit, allow_rotation, false
    ))) throw std::logic_error("Shelf First Fit wrapper mismatch");
    if(!same_result(sbw, shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::best_width_fit, allow_rotation, false
    ))) throw std::logic_error("Shelf Best Width Fit wrapper mismatch");
    if(!same_result(sbh, shelf_rectangle_packing(
        rectangles, bin_width, bin_height, ShelfFit::best_height_fit, allow_rotation, false
    ))) throw std::logic_error("Shelf Best Height Fit wrapper mismatch");

    const auto bottom_left = bottom_left_rectangle_packing(
        rectangles, bin_width, bin_height, allow_rotation
    );
    const auto bottom_left_fill = bottom_left_fill_rectangle_packing(
        rectangles, bin_width, bin_height, allow_rotation
    );
    if(!same_result(bottom_left_fill, bottom_left_rectangle_packing(
        rectangles, bin_width, bin_height, allow_rotation, true
    ))) throw std::logic_error("Bottom-Left-Fill wrapper mismatch");

    const auto skyline_bl = skyline_bottom_left_rectangle_packing(
        rectangles, bin_width, bin_height, allow_rotation
    );
    const auto skyline_bf = skyline_best_fit_rectangle_packing(
        rectangles, bin_width, bin_height, allow_rotation
    );
    if(!same_result(skyline_bl, skyline_rectangle_packing(
        rectangles, bin_width, bin_height, SkylineFit::bottom_left, allow_rotation
    ))) throw std::logic_error("Skyline Bottom-Left wrapper mismatch");
    if(!same_result(skyline_bf, skyline_rectangle_packing(
        rectangles, bin_width, bin_height, SkylineFit::best_fit, allow_rotation
    ))) throw std::logic_error("Skyline Best-Fit wrapper mismatch");

    const auto maxrects_ssf = maxrects_best_short_side_fit(
        rectangles, bin_width, bin_height, allow_rotation
    );
    const auto maxrects_lsf = maxrects_best_long_side_fit(
        rectangles, bin_width, bin_height, allow_rotation
    );
    const auto maxrects_af = maxrects_best_area_fit(
        rectangles, bin_width, bin_height, allow_rotation
    );
    if(!same_result(maxrects_ssf, maxrects_rectangle_packing(
        rectangles, bin_width, bin_height, MaxRectsFit::best_short_side_fit, allow_rotation
    ))) throw std::logic_error("MaxRects BSSF wrapper mismatch");
    if(!same_result(maxrects_lsf, maxrects_rectangle_packing(
        rectangles, bin_width, bin_height, MaxRectsFit::best_long_side_fit, allow_rotation
    ))) throw std::logic_error("MaxRects BLSF wrapper mismatch");
    if(!same_result(maxrects_af, maxrects_rectangle_packing(
        rectangles, bin_width, bin_height, MaxRectsFit::best_area_fit, allow_rotation
    ))) throw std::logic_error("MaxRects BAF wrapper mismatch");

    return {
        snf,
        sff,
        sbw,
        sbh,
        nfdh_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation),
        ffdh_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation),
        bfdh_rectangle_packing(rectangles, bin_width, bin_height, allow_rotation),
        bottom_left,
        bottom_left_fill,
        skyline_bl,
        skyline_bf,
        maxrects_ssf,
        maxrects_lsf,
        maxrects_af,
        guillotine_best_area_fit_rectangle_packing(
            rectangles, bin_width, bin_height, allow_rotation,
            GuillotineSplit::shorter_leftover_axis
        ),
        guillotine_best_area_fit_rectangle_packing(
            rectangles, bin_width, bin_height, allow_rotation,
            GuillotineSplit::longer_leftover_axis
        ),
        guillotine_best_area_fit_rectangle_packing(
            rectangles, bin_width, bin_height, allow_rotation,
            GuillotineSplit::shorter_axis
        ),
        guillotine_best_area_fit_rectangle_packing(
            rectangles, bin_width, bin_height, allow_rotation,
            GuillotineSplit::longer_axis
        ),
    };
}

template<class Function>
bool throws_invalid_argument(Function&& function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

bool boundary_tests(){
    using namespace approximate::packing;
    const std::vector<RectangleSize<long long>> one{{1, 1}};
    if(!throws_invalid_argument([&]{ (void)shelf_next_fit(one, 0LL, 1LL); })) return false;
    if(!throws_invalid_argument([&]{
        (void)bottom_left_rectangle_packing(
            std::vector<RectangleSize<long long>>{{0, 1}}, 2LL, 2LL
        );
    })) return false;
    if(!throws_invalid_argument([&]{
        (void)skyline_best_fit_rectangle_packing(
            std::vector<RectangleSize<long long>>{{3, 1}}, 2LL, 2LL
        );
    })) return false;
    if(!throws_invalid_argument([&]{
        (void)maxrects_best_area_fit(
            std::vector<RectangleSize<long long>>{{-1, 1}}, 2LL, 2LL
        );
    })) return false;
    const double infinity = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    if(!throws_invalid_argument([&]{
        (void)guillotine_best_area_fit_rectangle_packing(
            std::vector<RectangleSize<double>>{{1, nan}}, 2.0, 2.0
        );
    })) return false;
    if(!throws_invalid_argument([&]{
        (void)shelf_first_fit(std::vector<RectangleSize<double>>{{1, 1}}, infinity, 2.0);
    })) return false;

    const std::vector<RectangleSize<long long>> rotation_only{{5, 3}};
    if(!throws_invalid_argument([&]{
        (void)maxrects_best_short_side_fit(rotation_only, 3LL, 5LL, false);
    })) return false;
    for(const auto& result: run_all(rotation_only, 3LL, 5LL, true)){
        if(!valid_result(result, rotation_only, 3LL, 5LL, true)
            || !result.placements[0].rotated) return false;
    }

    const long long signed_max = std::numeric_limits<long long>::max();
    const std::vector<RectangleSize<long long>> signed_extreme{
        {signed_max - 1, signed_max}, {1, signed_max},
    };
    for(const auto& result: run_all(signed_extreme, signed_max, signed_max, false)){
        if(!valid_result(result, signed_extreme, signed_max, signed_max, false)) return false;
    }

    const auto unsigned_max = std::numeric_limits<unsigned long long>::max();
    const std::vector<RectangleSize<unsigned long long>> unsigned_extreme{
        {unsigned_max - 1, unsigned_max}, {1, unsigned_max},
    };
    for(const auto& result: run_all(unsigned_extreme, unsigned_max, unsigned_max, false)){
        if(!valid_result(result, unsigned_extreme, unsigned_max, unsigned_max, false)) return false;
    }

    const long double real_max = std::numeric_limits<long double>::max();
    if(approximate::packing::rectangle_packing_internal::fits_extent(
        1.0L, real_max, real_max
    )) return false;

    const std::vector<RectangleSize<long double>> real_extreme{
        {real_max / 2, real_max}, {real_max / 2, real_max},
    };
    for(const auto& result: run_all(real_extreme, real_max, real_max, false)){
        if(!valid_result(result, real_extreme, real_max, real_max, false)) return false;
    }
    const std::vector<RectangleSize<long double>> real_offset_extreme{
        {1.0L, real_max / 2}, {real_max, real_max / 2},
    };
    for(const auto& result: run_all(real_offset_extreme, real_max, real_max, false)){
        if(!valid_result(
            result, real_offset_extreme, real_max, real_max, false
        )) return false;
    }
    return true;
}

}  // namespace

int main(){
    if(!boundary_tests()) return 2;
    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t rectangle_count = 0;
        long long bin_width = 0;
        long long bin_height = 0;
        int expected_bins = -1;
        std::cin >> rectangle_count >> bin_width >> bin_height >> expected_bins;
        std::vector<RectangleSize<long long>> rectangles(rectangle_count);
        for(auto& rectangle: rectangles) std::cin >> rectangle.width >> rectangle.height;

        for(const bool allow_rotation: {false, true}){
            for(const auto& result: run_all(
                rectangles, bin_width, bin_height, allow_rotation
            )){
                if(!valid_result(
                    result, rectangles, bin_width, bin_height, allow_rotation
                )) return 3;
                if(expected_bins >= 0
                    && result.bin_count() != static_cast<std::size_t>(expected_bins)) return 4;
            }
        }
        std::cout << "OK " << test << ' ' << rectangle_count << ' '
            << bin_width << ' ' << bin_height << '\n';
    }
}
