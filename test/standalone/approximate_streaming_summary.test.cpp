// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "../../src/approximate/streaming/ams_sketch.hpp"
#include "../../src/approximate/streaming/kll_sketch.hpp"
#include "../../src/approximate/streaming/misra_gries.hpp"
#include "../../src/approximate/streaming/tdigest.hpp"
#include "../../src/approximate/streaming/space_saving.hpp"

int main(){
    int count;
    if(!(std::cin >> count)) return 0;
    MisraGries<int, 15> misra_gries;
    SpaceSaving<int, 16> space_saving;
    TDigest<128> tdigest;
    AMSSketch<9, 32, int> ams(123456);
    KLLSketch<int, 128, 24> kll(654321);
    std::unordered_map<int, std::uint64_t> exact;
    std::vector<int> values(static_cast<std::size_t>(count));
    assert((TDigest<128>::maximum_centroids() == 128U));
    assert(tdigest.centroid_count() == 0U);
    assert((AMSSketch<9, 32, int>::groups() == 9U));
    assert((AMSSketch<9, 32, int>::samples_per_group() == 32U));
    assert((KLLSketch<int, 128, 24>::capacity_per_level() == 128U));
    assert((KLLSketch<int, 128, 24>::maximum_levels() == 24U));
    assert((KLLSketch<int, 128, 24>::typical_rank_error_rate() > 0.0L));
    assert((MisraGries<int, 15>::capacity() == 15U));
    assert((SpaceSaving<int, 16>::capacity() == 16U));
    assert(misra_gries.total() == 0U);
    assert(space_saving.total() == 0U);
    assert(space_saving.size() == 0U);
    assert(tdigest.total_weight() == 0.0L);
    assert(kll.size() == 0U);
    assert(kll.retained_items() == 0U);
    for(int& value: values){
        std::cin >> value;
        ++exact[value];
        misra_gries.add(value);
        space_saving.add(value);
        ams.add(value);
        tdigest.add(static_cast<long double>(value));
        kll.add(value);
    }
    for(const auto& candidate: misra_gries.candidates()){
        assert(candidate.second <= exact[candidate.first]);
        assert(exact[candidate.first] - candidate.second <= misra_gries.maximum_underestimate());
        assert(misra_gries.estimate_lower_bound(candidate.first) == candidate.second);
    }
    for(const auto& candidate: space_saving.candidates()){
        assert(candidate.estimate >= exact[candidate.key]);
        assert(candidate.estimate - candidate.error <= exact[candidate.key]);
        auto found = space_saving.estimate(candidate.key);
        assert(found.key == candidate.key);
        assert(found.estimate == candidate.estimate);
        assert(found.error == candidate.error);
    }
    assert(misra_gries.total() == static_cast<std::uint64_t>(count));
    assert(space_saving.total() == static_cast<std::uint64_t>(count));
    assert((space_saving.size() <= SpaceSaving<int, 16>::capacity()));
    assert(kll.size() == static_cast<std::uint64_t>(count));
    assert(tdigest.total_weight() == static_cast<long double>(count));
    std::uint64_t exact_second_moment = 0;
    for(const auto& entry: exact) exact_second_moment += entry.second * entry.second;
    const long double estimated_second_moment = ams.estimate_second_moment();
    assert(std::fabs(estimated_second_moment - static_cast<long double>(exact_second_moment))
           <= static_cast<long double>(exact_second_moment) * 0.55L + 2.0L);
    assert(tdigest.centroid_count() <= TDigest<128>::maximum_centroids());
    assert((tdigest.centroid_count() == 0U) == values.empty());
    assert(kll.retained_items() <= static_cast<std::size_t>(kll.size()));
    assert((kll.retained_items() == 0U) == values.empty());

    std::sort(values.begin(), values.end());
    for(const long double probability: {0.1L, 0.5L, 0.9L}){
        const int estimated = kll.quantile(probability);
        const auto lower = std::lower_bound(values.begin(), values.end(), estimated);
        const auto upper = std::upper_bound(values.begin(), values.end(), estimated);
        const std::size_t target = static_cast<std::size_t>(
            probability * static_cast<long double>(values.size() - 1U));
        const std::size_t minimum_rank = static_cast<std::size_t>(lower - values.begin());
        const std::size_t maximum_rank = static_cast<std::size_t>(upper - values.begin());
        const std::size_t tolerance = values.size() / 6U + 2U;
        assert(minimum_rank <= target + tolerance);
        assert(maximum_rank + tolerance > target);
        assert(std::fabs(tdigest.quantile(probability) - static_cast<long double>(values[target]))
               <= 35.0L);
    }
    TDigest<8> self_merge;
    self_merge.add(1.0L);
    self_merge.add(3.0L, 2.0L);
    const long double weight_before_merge = self_merge.total_weight();
    self_merge.merge(self_merge);
    assert(self_merge.total_weight() == 2.0L * weight_before_merge);

    bool thrown = false;
    try{
        (void)self_merge.cdf(std::numeric_limits<long double>::quiet_NaN());
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    MisraGries<int, 3, std::uint8_t> small_misra_gries;
    for(int i = 0; i < 255; ++i) small_misra_gries.add(1);
    thrown = false;
    try{
        small_misra_gries.add(1);
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);

    SpaceSaving<int, 3, std::uint8_t> small_space_saving;
    small_space_saving.add(1, std::numeric_limits<std::uint8_t>::max());
    thrown = false;
    try{
        small_space_saving.add(1);
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);

    TDigest<8> digest_left, digest_right;
    digest_left.add(1.0L);
    digest_right.add(3.0L, 2.0L);
    digest_left.merge(digest_right);
    assert(digest_left.total_weight() == 3.0L);
    assert(digest_left.centroid_count() == 2U);
    assert(digest_left.quantile(0.0L) == 1.0L);
    assert(digest_left.quantile(1.0L) == 3.0L);
    digest_left.merge(digest_left);
    assert(digest_left.total_weight() == 6.0L);
    assert(digest_left.centroid_count() <= TDigest<8>::maximum_centroids());

    misra_gries.clear();
    assert(misra_gries.total() == 0U);
    assert(misra_gries.maximum_underestimate() == 0U);
    assert(misra_gries.estimate_lower_bound(0) == 0U);
    assert(misra_gries.candidates().empty());

    space_saving.clear();
    assert(space_saving.total() == 0U);
    assert(space_saving.size() == 0U);
    assert(space_saving.candidates().empty());
    assert(space_saving.estimate(0).estimate == 0U);

    ams.clear();
    assert(ams.estimate_second_moment() == 0.0L);
    tdigest.clear();
    assert(tdigest.centroid_count() == 0U);
    assert(tdigest.total_weight() == 0.0L);
    assert(tdigest.cdf(0.0L) == 0.0L);
    kll.clear(777);
    assert(kll.size() == 0U);
    assert(kll.retained_items() == 0U);
    thrown = false;
    try{
        (void)kll.quantile(0.5L);
    }catch(const std::logic_error&){
        thrown = true;
    }
    assert(thrown);
    std::cout << "OK\n";
}
