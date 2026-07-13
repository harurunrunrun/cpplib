// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>
#include "../../src/algorithm/other/time_segment_tree.hpp"

struct RollbackMultiset{
    struct Change{
        int value;
        int previous_count;
    };

    std::map<int, int> count;
    std::vector<Change> history;
    long long sum = 0;
    int elements = 0;

    int snapshot() const{ return static_cast<int>(history.size()); }

    void insert(int value){
        auto it = count.find(value);
        int previous = it == count.end() ? 0 : it->second;
        history.push_back({value, previous});
        count[value] = previous + 1;
        sum += value;
        ++elements;
    }

    void rollback(int snapshot){
        while(static_cast<int>(history.size()) > snapshot){
            Change change = history.back();
            history.pop_back();
            auto it = count.find(change.value);
            assert(it != count.end());
            --elements;
            sum -= change.value;
            if(change.previous_count == 0) count.erase(it);
            else it->second = change.previous_count;
        }
    }
};

struct Answer{
    int elements;
    int distinct;
    long long sum;

    friend bool operator==(const Answer&, const Answer&) = default;
};

template<int MAX_TIME, int MAX_EVENTS>
std::vector<Answer> solve(
    int time_count,
    const std::vector<std::tuple<int, int, int>>& intervals
){
    TimeSegmentTree<int, MAX_TIME, MAX_EVENTS> events(time_count);
    for(auto [left, right, value]: intervals){
        events.emplace_interval(left, right, value);
    }
    RollbackMultiset state;
    std::vector<Answer> answer(static_cast<std::size_t>(time_count));
    events.run(
        [&](const int& value){ state.insert(value); },
        [&](){ return state.snapshot(); },
        [&](int snapshot){ state.rollback(snapshot); },
        [&](int time){
            answer[static_cast<std::size_t>(time)] = {
                state.elements,
                static_cast<int>(state.count.size()),
                state.sum
            };
        }
    );
    assert(state.elements == 0);
    assert(state.count.empty());
    assert(state.history.empty());
    return answer;
}

std::vector<Answer> naive(
    int time_count,
    const std::vector<std::tuple<int, int, int>>& intervals
){
    std::vector<Answer> result(static_cast<std::size_t>(time_count));
    for(int time = 0; time < time_count; ++time){
        std::map<int, int> count;
        long long sum = 0;
        int elements = 0;
        for(auto [left, right, value]: intervals){
            if(left <= time && time < right){
                ++count[value];
                sum += value;
                ++elements;
            }
        }
        result[static_cast<std::size_t>(time)] = {
            elements,
            static_cast<int>(count.size()),
            sum
        };
    }
    return result;
}

void self_test(){
    {
        std::vector<std::tuple<int, int, int>> intervals = {
            {0, 5, 3}, {1, 4, -2}, {2, 2, 100}, {2, 5, 3}, {4, 5, 7}
        };
        assert((solve<8, 16>(5, intervals) == naive(5, intervals)));
    }
    {
        TimeSegmentTree<std::unique_ptr<int>, 4, 2> events(4);
        events.add_interval(1, 3, std::make_unique<int>(7));
        int sum = 0;
        std::vector<int> history;
        std::vector<int> answer;
        events.run(
            [&](const std::unique_ptr<int>& value){
                history.push_back(sum);
                sum += *value;
            },
            [&](){ return history.size(); },
            [&](std::size_t snapshot){
                while(history.size() > snapshot){
                    sum = history.back();
                    history.pop_back();
                }
            },
            [&](int){ answer.push_back(sum); }
        );
        assert((answer == std::vector<int>{0, 7, 7, 0}));
    }
    {
        OfflineIntervalEvents<int, 4, 1> events(0);
        int calls = 0;
        events.run(
            [&](const int&){ assert(false); },
            [&](){ return 0; },
            [&](int){},
            [&](int){ ++calls; }
        );
        assert(calls == 0);
        events.add_interval(0, 0, 1);
        assert(events.event_count() == 0);
    }
    {
        TimeSegmentTree<int, 8, 1> events(5);
        int snapshots = 0;
        int rollbacks = 0;
        int answers = 0;
        events.run(
            [&](const int&){ assert(false); },
            [&](){
                ++snapshots;
                return 0;
            },
            [&](int){ ++rollbacks; },
            [&](int){ ++answers; }
        );
        assert(snapshots == 0);
        assert(rollbacks == 0);
        assert(answers == 5);

        events.add_interval(0, 5, 1);
        events.run(
            [&](const int&){},
            [&](){
                ++snapshots;
                return 0;
            },
            [&](int){ ++rollbacks; },
            [&](int){ ++answers; }
        );
        assert(snapshots == events.reference_count());
        assert(rollbacks == events.reference_count());
        assert(answers == 10);
    }
    {
        bool negative_error = false;
        try{
            [[maybe_unused]] TimeSegmentTree<int, 4, 1> events(-1);
        }catch(const std::runtime_error&){
            negative_error = true;
        }
        assert(negative_error);

        bool capacity_error = false;
        try{
            [[maybe_unused]] TimeSegmentTree<int, 4, 1> events(5);
        }catch(const std::runtime_error&){
            capacity_error = true;
        }
        assert(capacity_error);
    }
    {
        TimeSegmentTree<int, 5, 1> events(5);
        events.add_interval(0, 5, 1);
        bool capacity_error = false;
        try{
            events.add_interval(1, 2, 2);
        }catch(const std::runtime_error&){
            capacity_error = true;
        }
        assert(capacity_error);
        events.clear();
        assert(events.event_count() == 0);
        assert(events.reference_count() == 0);
        events.add_interval(2, 3, 4);
        bool range_error = false;
        try{
            events.add_interval(-1, 2, 0);
        }catch(const std::runtime_error&){
            range_error = true;
        }
        assert(range_error);
    }

    std::mt19937 rng(20260713);
    for(int iteration = 0; iteration < 300; ++iteration){
        int time_count = static_cast<int>(rng() % 64);
        int interval_count = static_cast<int>(rng() % 150);
        std::vector<std::tuple<int, int, int>> intervals;
        intervals.reserve(static_cast<std::size_t>(interval_count));
        for(int i = 0; i < interval_count; ++i){
            int left = static_cast<int>(rng() % static_cast<unsigned>(time_count + 1));
            int right = static_cast<int>(rng() % static_cast<unsigned>(time_count + 1));
            if(right < left) std::swap(left, right);
            int value = static_cast<int>(rng() % 31) - 15;
            intervals.emplace_back(left, right, value);
        }
        assert((solve<64, 160>(time_count, intervals) == naive(time_count, intervals)));
    }
}

int main(){
    int time_count, interval_count;
    if(!(std::cin >> time_count >> interval_count)){
        self_test();
        return 0;
    }
    std::vector<std::tuple<int, int, int>> intervals;
    intervals.reserve(static_cast<std::size_t>(interval_count));
    for(int i = 0; i < interval_count; ++i){
        int left, right, value;
        std::cin >> left >> right >> value;
        intervals.emplace_back(left, right, value);
    }
    for(const Answer& answer: solve<1024, 4096>(time_count, intervals)){
        std::cout << answer.elements << ' ' << answer.distinct << ' ' << answer.sum << '\n';
    }
}
