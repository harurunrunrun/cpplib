#pragma once

#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct OrderedQuotaEuclideanActivator{
    long double x;
    long double y;
    int quota;
    std::vector<int> priority_order;
};

inline std::vector<long double>
ordered_quota_euclidean_activation_times(
    const std::vector<OrderedQuotaEuclideanActivator>& activators,
    int initial_vertex = 0
){
    if(activators.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "ordered_quota_euclidean_activation_times "
            "has too many vertices"
        );
    }
    const int vertex_count = static_cast<int>(activators.size());
    if(vertex_count == 0) return {};
    if(initial_vertex < 0 || vertex_count <= initial_vertex)[[unlikely]]{
        throw std::out_of_range(
            "ordered_quota_euclidean_activation_times "
            "initial vertex is out of range"
        );
    }

    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const auto& activator =
            activators[static_cast<std::size_t>(vertex)];
        if(!std::isfinite(activator.x) ||
           !std::isfinite(activator.y))[[unlikely]]{
            throw std::invalid_argument(
                "ordered_quota_euclidean_activation_times "
                "requires finite coordinates"
            );
        }
        if(activator.quota < 0)[[unlikely]]{
            throw std::invalid_argument(
                "ordered_quota_euclidean_activation_times negative quota"
            );
        }
        if(activator.priority_order.size() !=
           static_cast<std::size_t>(vertex_count - 1))[[unlikely]]{
            throw std::invalid_argument(
                "ordered_quota_euclidean_activation_times "
                "priority order size mismatch"
            );
        }
        std::vector<unsigned char> appeared(
            static_cast<std::size_t>(vertex_count), 0
        );
        appeared[static_cast<std::size_t>(vertex)] = 1;
        for(const int target: activator.priority_order){
            if(target < 0 || vertex_count <= target ||
               appeared[static_cast<std::size_t>(target)])[[unlikely]]{
                throw std::invalid_argument(
                    "ordered_quota_euclidean_activation_times "
                    "priority order is not a permutation"
                );
            }
            appeared[static_cast<std::size_t>(target)] = 1;
        }
    }

    using Event = std::pair<long double, int>;
    std::priority_queue<
        Event,
        std::vector<Event>,
        std::greater<Event>
    > events;
    std::vector<long double> activation_time(
        static_cast<std::size_t>(vertex_count),
        std::numeric_limits<long double>::infinity()
    );
    std::vector<unsigned char> active(
        static_cast<std::size_t>(vertex_count), 0
    );

    const auto schedule = [&](int source){
        int remaining =
            activators[static_cast<std::size_t>(source)].quota;
        for(const int target:
            activators[static_cast<std::size_t>(source)].priority_order){
            if(remaining == 0) break;
            if(active[static_cast<std::size_t>(target)]) continue;

            const long double delta_x =
                activators[static_cast<std::size_t>(source)].x -
                activators[static_cast<std::size_t>(target)].x;
            const long double delta_y =
                activators[static_cast<std::size_t>(source)].y -
                activators[static_cast<std::size_t>(target)].y;
            events.emplace(
                activation_time[static_cast<std::size_t>(source)] +
                    std::hypot(delta_x, delta_y),
                target
            );
            --remaining;
        }
    };

    activation_time[static_cast<std::size_t>(initial_vertex)] = 0;
    active[static_cast<std::size_t>(initial_vertex)] = 1;
    schedule(initial_vertex);
    while(!events.empty()){
        const auto [time, target] = events.top();
        events.pop();
        if(active[static_cast<std::size_t>(target)]) continue;
        active[static_cast<std::size_t>(target)] = 1;
        activation_time[static_cast<std::size_t>(target)] = time;
        schedule(target);
    }
    return activation_time;
}
