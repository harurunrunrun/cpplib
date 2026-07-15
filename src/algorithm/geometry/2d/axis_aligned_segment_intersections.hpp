#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "geometry_sign.hpp"
#include "types.hpp"

inline long long count_axis_aligned_segment_intersections(
    const std::vector<Segment>& segments
){
    struct Event{
        long double x;
        int type;
        long double low;
        long double high;
    };

    std::vector<Event> events;
    std::vector<long double> horizontal_y;
    events.reserve(segments.size() * 2);
    horizontal_y.reserve(segments.size());

    for(const Segment& segment: segments){
        const int dx = geometry_sign(segment.b.x - segment.a.x);
        const int dy = geometry_sign(segment.b.y - segment.a.y);
        if(dx == 0 && dy == 0)[[unlikely]]{
            throw std::invalid_argument("degenerate segment");
        }
        if(dx != 0 && dy != 0)[[unlikely]]{
            throw std::invalid_argument("segment is not axis-aligned");
        }
        if(dy == 0){
            const long double left = std::min(segment.a.x, segment.b.x);
            const long double right = std::max(segment.a.x, segment.b.x);
            const long double y = segment.a.y;
            events.push_back({left, 0, y, y});
            events.push_back({right, 2, y, y});
            horizontal_y.push_back(y);
        }else{
            const long double low = std::min(segment.a.y, segment.b.y);
            const long double high = std::max(segment.a.y, segment.b.y);
            events.push_back({segment.a.x, 1, low, high});
        }
    }

    std::sort(horizontal_y.begin(), horizontal_y.end());
    horizontal_y.erase(
        std::unique(horizontal_y.begin(), horizontal_y.end()),
        horizontal_y.end()
    );
    std::sort(events.begin(), events.end(), [](const Event& lhs, const Event& rhs){
        if(lhs.x != rhs.x) return lhs.x < rhs.x;
        return lhs.type < rhs.type;
    });

    std::vector<int> bit(horizontal_y.size() + 1, 0);
    auto add = [&](int index, int value){
        for(std::size_t i = static_cast<std::size_t>(index + 1); i < bit.size(); i += i & -i){
            bit[i] += value;
        }
    };
    auto prefix_sum = [&](int right){
        int result = 0;
        for(std::size_t i = static_cast<std::size_t>(right); i > 0; i -= i & -i){
            result += bit[i];
        }
        return result;
    };

    long long result = 0;
    for(const Event& event: events){
        if(event.type == 1){
            const int left = static_cast<int>(std::lower_bound(
                horizontal_y.begin(), horizontal_y.end(), event.low
            ) - horizontal_y.begin());
            const int right = static_cast<int>(std::upper_bound(
                horizontal_y.begin(), horizontal_y.end(), event.high
            ) - horizontal_y.begin());
            result += prefix_sum(right) - prefix_sum(left);
        }else{
            const int index = static_cast<int>(std::lower_bound(
                horizontal_y.begin(), horizontal_y.end(), event.low
            ) - horizontal_y.begin());
            add(index, event.type == 0 ? 1 : -1);
        }
    }
    return result;
}
