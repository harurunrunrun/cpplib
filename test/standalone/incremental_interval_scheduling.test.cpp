// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/structure/other/incremental_interval_scheduling.hpp"

using Scheduler = IncrementalIntervalScheduling<long long>;

static std::size_t naive_maximum(
    const std::vector<std::pair<long long, long long>>& intervals
){
    auto sorted = intervals;
    std::sort(sorted.begin(), sorted.end(), [](const auto& left, const auto& right){
        if(left.second != right.second) return left.second < right.second;
        return left.first > right.first;
    });
    bool has_previous = false;
    long long previous_finish = 0;
    std::size_t answer = 0;
    for(const auto [start, finish]: sorted){
        if(!has_previous || previous_finish <= start){
            has_previous = true;
            previous_finish = finish;
            ++answer;
        }
    }
    return answer;
}

static void validate_schedule(const Scheduler& scheduler){
    const auto ids = scheduler.optimal_schedule_ids();
    assert(ids.size() == scheduler.maximum_count());
    bool has_previous = false;
    long long previous_finish = 0;
    for(const std::size_t id: ids){
        assert(scheduler.is_active(id));
        const auto& interval = scheduler.interval(id);
        if(has_previous) assert(previous_finish <= interval.start);
        has_previous = true;
        previous_finish = interval.finish;
    }
}

static void test_random_arbitrary_intervals(){
    std::mt19937_64 random(2026071611ULL);
    for(int iteration = 0; iteration < 80; ++iteration){
        Scheduler scheduler(-40, 41);
        std::vector<std::pair<long long, long long>> intervals;
        for(int step = 0; step < 450; ++step){
            long long start = static_cast<long long>(random() % 80) - 40;
            long long finish = static_cast<long long>(random() % 80) - 39;
            if(finish <= start) std::swap(start, finish);
            if(finish <= start) finish = start + 1;

            const auto result = scheduler.insert(start, finish);
            intervals.emplace_back(start, finish);
            assert(result.id + 1 == intervals.size());
            assert(result.active == scheduler.is_active(result.id));
            assert(result.maximum_count == naive_maximum(intervals));
            assert(scheduler.maximum_count() == result.maximum_count);
            if(step % 17 == 0) validate_schedule(scheduler);
        }
        validate_schedule(scheduler);
    }
}

static void test_containment_and_boundaries(){
    Scheduler nested(0, 20);
    const auto first = nested.insert(0, 10);
    const auto second = nested.insert(1, 11);
    const auto third = nested.insert(2, 12);
    assert(first.active && second.active && third.active);
    assert(nested.active_interval_count() == 3);
    assert(nested.maximum_count() == 1);

    const auto inner = nested.insert(3, 4);
    assert(inner.active);
    assert(inner.deactivated_count == 3);
    assert(!nested.is_active(first.id));
    assert(!nested.is_active(second.id));
    assert(!nested.is_active(third.id));
    assert(nested.active_interval_count() == 1);
    assert(nested.maximum_count() == 1);

    const auto touching = nested.insert(4, 5);
    assert(touching.active);
    assert(nested.maximum_count() == 2);
    const auto outer = nested.insert(0, 20);
    assert(!outer.active);
    assert(outer.deactivated_count == 0);
    assert(nested.maximum_count() == 2);
    validate_schedule(nested);

    Scheduler same_start(-10, 10);
    const auto longer = same_start.insert(-4, 7);
    const auto shorter = same_start.insert(-4, 3);
    assert(longer.active);
    assert(shorter.active && shorter.deactivated_count == 1);
    assert(!same_start.is_active(longer.id));
    assert(same_start.is_active(shorter.id));
    assert(!same_start.insert(-4, 3).active);

    bool thrown = false;
    try{ (void)same_start.insert(2, 2); }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)same_start.insert(-11, 0); }
    catch(const std::out_of_range&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)same_start.interval(99); }
    catch(const std::out_of_range&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ Scheduler invalid(5, 5); (void)invalid; }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);

    IncrementalIntervalScheduling<std::int8_t> narrow(
        std::numeric_limits<std::int8_t>::min(),
        std::numeric_limits<std::int8_t>::max()
    );
    assert(narrow.insert(-1, 1).maximum_count == 1);
}

static void test_large(){
    constexpr std::size_t count = 200000;
    Scheduler scheduler(0, static_cast<long long>(count * 2));
    scheduler.reserve_intervals(count);
    for(std::size_t index = 0; index < count; ++index){
        const long long start = static_cast<long long>(index * 2);
        const auto result = scheduler.insert(start, start + 1);
        assert(result.active);
        assert(result.maximum_count == index + 1);
    }
    assert(scheduler.maximum_count() == count);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long minimum_coordinate;
    long long maximum_coordinate;
    std::size_t interval_count;
    if(!(std::cin >> minimum_coordinate >> maximum_coordinate >> interval_count)){
        test_random_arbitrary_intervals();
        test_containment_and_boundaries();
        test_large();
        return 0;
    }

    Scheduler scheduler(minimum_coordinate, maximum_coordinate);
    scheduler.reserve_intervals(interval_count);
    while(interval_count-- > 0){
        long long start;
        long long finish;
        std::cin >> start >> finish;
        std::cout << scheduler.insert(start, finish).maximum_count << '\n';
    }
}
