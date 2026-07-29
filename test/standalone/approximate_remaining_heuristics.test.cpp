// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "../../src/approximate/clustering/advanced_clustering.hpp"
#include "../../src/approximate/clustering/optics.hpp"
#include "../../src/approximate/facility/primal_dual_facility_location.hpp"
#include "../../src/approximate/packing/advanced_bin_packing.hpp"
#include "../../src/approximate/packing/advanced_rectangle_packing.hpp"
#include "../../src/approximate/scheduling/advanced_scheduling.hpp"
#include "../../src/approximate/scheduling/dag_scheduling.hpp"
#include "../../src/approximate/scheduling/single_machine_dispatching.hpp"
#include "../../src/approximate/set_cover/advanced_set_cover.hpp"

using approximate::clustering::DensePoints;
using approximate::packing::RectanglePackingResult;
using approximate::packing::RectangleSize;

template<class Size>
void verify_bin_packing(
    const std::vector<Size>& items,
    const Size& capacity,
    const approximate::packing::BinPackingResult<Size>& result
){
    assert(result.bin_of_item.size() == items.size());
    std::vector<Size> load(result.bin_count(), Size{});
    for(std::size_t item = 0; item < items.size(); ++item){
        assert(result.bin_of_item[item] < result.bin_count());
        load[result.bin_of_item[item]] += items[item];
    }
    for(std::size_t bin = 0; bin < load.size(); ++bin){
        assert(load[bin] <= capacity);
        assert(result.remaining_capacity[bin] == capacity - load[bin]);
    }
}

template<class Size>
void verify_rectangle_packing(
    const std::vector<RectangleSize<Size>>& rectangles,
    const RectanglePackingResult<Size>& result
){
    assert(result.placements.size() == rectangles.size());
    for(std::size_t item = 0; item < rectangles.size(); ++item){
        const auto& placement = result.placements[item];
        assert(placement.bin < result.bin_count());
        assert(placement.x + placement.width <= result.bin_width);
        assert(placement.y + placement.height <= result.bin_height);
        assert(
            (placement.width == rectangles[item].width
             && placement.height == rectangles[item].height)
            || (placement.width == rectangles[item].height
                && placement.height == rectangles[item].width)
        );
        for(std::size_t other = 0; other < item; ++other){
            const auto& previous = result.placements[other];
            if(previous.bin != placement.bin) continue;
            const bool overlap =
                previous.x < placement.x + placement.width
                && placement.x < previous.x + previous.width
                && previous.y < placement.y + placement.height
                && placement.y < previous.y + previous.height;
            assert(!overlap);
        }
    }
}

void fixed_tests(){
    std::mt19937 random(20260729U);
    const DensePoints<double> points{
        {0.0, 0.0}, {0.1, 0.0}, {10.0, 10.0}, {10.1, 10.0}
    };
    const auto optics = approximate::clustering::optics(points, 1.0L, 2);
    assert(optics.ordering.size() == points.size());
    const auto spectral = approximate::clustering::spectral_clustering(
        points, 2, 1.0L, random, 100, 100
    );
    assert(spectral.labels[0] == spectral.labels[1]);
    assert(spectral.labels[2] == spectral.labels[3]);
    assert(spectral.labels[0] != spectral.labels[2]);
    const auto isodata = approximate::clustering::isodata_clustering(
        points, 2, 1, 3, 1, 100.0L, 0.1L, random, 20
    );
    assert(isodata.centers.size() == 2);
    assert(isodata.labels[0] == isodata.labels[1]);
    assert(isodata.labels[2] == isodata.labels[3]);

    const std::vector<std::vector<int>> distance{
        {0, 1, 10, 11},
        {11, 10, 1, 0}
    };
    const auto facility =
        approximate::facility::primal_dual_facility_location(
            distance, std::vector<int>{1, 1}
        );
    assert(facility.solution.labels.size() == 4);
    assert(facility.solution.facilities.size() == 2);
    assert(facility.client_dual.size() == 4);
    const auto zero_opening_cost =
        approximate::facility::primal_dual_facility_location(
            std::vector<std::vector<int>>{{2, 3}},
            std::vector<int>{0}
        );
    assert(zero_opening_cost.solution.labels.size() == 2);
    assert(zero_opening_cost.solution.facilities
           == std::vector<std::size_t>{0});

    using Operation =
        approximate::scheduling::JobShopOperation<int>;
    const std::vector<std::vector<Operation>> jobs{
        {{0, 2}, {1, 1}},
        {{1, 2}, {0, 1}}
    };
    const auto job_shop =
        approximate::scheduling::shifting_bottleneck_schedule(jobs, 2);
    assert(job_shop.start_time[0][1] >= job_shop.completion_time[0][0]);
    assert(job_shop.start_time[1][1] >= job_shop.completion_time[1][0]);
    for(const auto& machine : job_shop.machine_order){
        for(std::size_t index = 1; index < machine.size(); ++index){
            const auto [previous_job, previous_operation] = machine[index - 1];
            const auto [job, operation] = machine[index];
            assert(job_shop.start_time[job][operation]
                   >= job_shop.completion_time[previous_job][previous_operation]);
        }
    }
    const std::vector<int> durations{9, 8, 7, 6, 5, 4, 3};
    const auto randomized =
        approximate::scheduling::randomized_list_schedule(
            durations, 3, random
        );
    assert(randomized.machine_of_job.size() == durations.size());
    const auto multifit =
        approximate::scheduling::multifit_schedule(durations, 3, 12);
    const auto lpt = approximate::scheduling::lpt_schedule(durations, 3);
    assert(multifit.schedule.makespan <= lpt.makespan);

    const std::vector<int> items{51, 49, 40, 34, 33, 20, 18, 10, 5};
    constexpr int capacity = 100;
    const auto nfd =
        approximate::packing::next_fit_decreasing_bin_packing(items, capacity);
    const auto ffd =
        approximate::packing::first_fit_decreasing_bin_packing(items, capacity);
    const auto bfd =
        approximate::packing::best_fit_decreasing_bin_packing(items, capacity);
    const auto wfd =
        approximate::packing::worst_fit_decreasing_bin_packing(items, capacity);
    const auto awf =
        approximate::packing::almost_worst_fit_bin_packing(items, capacity);
    const auto harmonic =
        approximate::packing::harmonic_bin_packing(items, capacity, 8);
    const auto harmonic_fit =
        approximate::packing::harmonic_fit_bin_packing(items, capacity, 8);
    const auto harmonic_ff =
        approximate::packing::harmonic_first_fit_bin_packing(
            items, capacity, 8
        );
    const auto refined =
        approximate::packing::refined_first_fit_bin_packing(items, capacity);
    const auto modified =
        approximate::packing::modified_first_fit_decreasing_bin_packing(
            items, capacity
        );
    const auto mffd =
        approximate::packing::mffd_bin_packing(items, capacity);
    for(const auto* result : {
        &nfd, &ffd, &bfd, &wfd, &awf, &harmonic,
        &harmonic_fit, &harmonic_ff, &refined, &modified, &mffd
    }){
        verify_bin_packing(items, capacity, *result);
    }

    const std::vector<RectangleSize<int>> rectangles{
        {6, 4}, {4, 6}, {5, 3}, {3, 3}, {2, 7}, {2, 2}
    };
    const auto split = approximate::packing::split_fit_rectangle_packing(
        rectangles, 10, 10, true
    );
    const auto floor_ceiling =
        approximate::packing::floor_ceiling_rectangle_packing(
            rectangles, 10, 10, true
        );
    const auto steinberg =
        approximate::packing::steinberg_rectangle_packing(
            rectangles, 10, 10, true
        );
    verify_rectangle_packing(rectangles, split);
    verify_rectangle_packing(rectangles, floor_ceiling);
    verify_rectangle_packing(rectangles, steinberg);

    const std::vector<std::vector<std::size_t>> sets{
        {0, 1}, {1, 2}, {2, 3}, {0, 3}
    };
    const std::vector<int> weights{2, 2, 2, 2};
    const auto primal_dual =
        approximate::set_cover::primal_dual_set_cover(4, sets, weights);
    const auto local_ratio =
        approximate::set_cover::local_ratio_set_cover(4, sets, weights);
    assert(primal_dual.cover.covers_universe());
    assert(local_ratio.covers_universe());
    const std::vector<long double> fractional(4, 0.5L);
    const auto deterministic =
        approximate::set_cover::deterministic_rounding_set_cover(
            4, sets, fractional
        );
    const auto iterative =
        approximate::set_cover::iterative_rounding_set_cover(
            4, sets, fractional
        );
    const auto lp = approximate::set_cover::lp_rounding_set_cover(
        4, sets, fractional
    );
    assert(deterministic.covers_universe());
    assert(iterative.covers_universe());
    assert(lp.covers_universe());
    const auto rounded =
        approximate::set_cover::randomized_rounding_set_cover(
            4, sets, fractional, random, 3
        );
    for(const std::size_t set_index : rounded.selected_sets){
        assert(set_index < sets.size());
    }
    const auto dependent = approximate::set_cover::dependent_rounding(
        std::vector<long double>{0.2L, 0.8L, 0.4L, 0.6L}, random
    );
    assert(std::accumulate(
        dependent.begin(), dependent.end(), std::size_t{0}
    ) == 2);
    const auto pipage = approximate::set_cover::pipage_rounding(
        std::vector<long double>{0.2L, 0.8L, 0.4L, 0.6L},
        [](const std::vector<long double>& values){
            long double result = 0.0L;
            for(const long double value : values) result += value * value;
            return result;
        }
    );
    assert(std::accumulate(
        pipage.begin(), pipage.end(), std::size_t{0}
    ) == 2);
}

int main(){
    fixed_tests();
    int count;
    int capacity;
    if(!(std::cin >> count >> capacity)) return 0;
    std::vector<int> items(static_cast<std::size_t>(count));
    for(int& item : items) std::cin >> item;
    const auto result =
        approximate::packing::first_fit_decreasing_bin_packing(
            items, capacity
        );
    verify_bin_packing(items, capacity, result);
    std::cout << result.bin_count() << '\n';
}
