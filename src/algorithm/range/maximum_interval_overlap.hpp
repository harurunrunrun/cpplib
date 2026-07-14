#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

enum class IntervalEndpointPolicy{
    half_open,
    closed,
};

template<class T>
int maximum_interval_overlap(
    const std::vector<std::pair<T, T>>& intervals,
    IntervalEndpointPolicy endpoint_policy = IntervalEndpointPolicy::half_open
){
    if(
        endpoint_policy != IntervalEndpointPolicy::half_open &&
        endpoint_policy != IntervalEndpointPolicy::closed
    )[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid endpoint policy (maximum_interval_overlap)."
        );
    }

    struct Event{
        T coordinate;
        int delta;
    };
    std::vector<Event> events;
    events.reserve(intervals.size() * 2);
    for(const auto& [left, right]: intervals){
        if(right < left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (maximum_interval_overlap)."
            );
        }
        if(endpoint_policy == IntervalEndpointPolicy::half_open && !(left < right)){
            continue;
        }
        events.push_back({left, 1});
        events.push_back({right, -1});
    }

    std::sort(events.begin(), events.end(), [&](const Event& left, const Event& right){
        if(left.coordinate < right.coordinate) return true;
        if(right.coordinate < left.coordinate) return false;
        if(endpoint_policy == IntervalEndpointPolicy::half_open){
            return left.delta < right.delta;
        }
        return right.delta < left.delta;
    });

    int current = 0;
    int answer = 0;
    for(const auto& event: events){
        current += event.delta;
        answer = std::max(answer, current);
    }
    return answer;
}
