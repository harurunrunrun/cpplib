#pragma once

#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct GondorSpark{
    long long x;
    long long y;
    int arrow_count;
    std::vector<int> instruction_order;
};

inline std::vector<double> gondor_spark_lighting_times(
    const std::vector<GondorSpark>& sparks
){
    const int spark_count = static_cast<int>(sparks.size());
    for(int spark = 0; spark < spark_count; spark++){
        const auto& data = sparks[static_cast<std::size_t>(spark)];
        if(data.arrow_count < 0 ||
           data.instruction_order.size() != static_cast<std::size_t>(spark_count - 1))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch "
                "(gondor_spark_lighting_times)."
            );
        }
        std::vector<unsigned char> appeared(static_cast<std::size_t>(spark_count), 0);
        appeared[static_cast<std::size_t>(spark)] = 1;
        for(int target: data.instruction_order){
            if(target < 0 || spark_count <= target ||
               appeared[static_cast<std::size_t>(target)])[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid instruction "
                    "(gondor_spark_lighting_times)."
                );
            }
            appeared[static_cast<std::size_t>(target)] = 1;
        }
    }
    if(spark_count == 0) return {};

    using Event = std::pair<double, int>;
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> events;
    std::vector<double> lighting_time(
        static_cast<std::size_t>(spark_count),
        std::numeric_limits<double>::infinity()
    );
    std::vector<unsigned char> lit(static_cast<std::size_t>(spark_count), 0);

    auto activate = [&](int source){
        int remaining = sparks[static_cast<std::size_t>(source)].arrow_count;
        for(int target: sparks[static_cast<std::size_t>(source)].instruction_order){
            if(remaining == 0) break;
            if(lit[static_cast<std::size_t>(target)]) continue;
            const long double delta_x =
                static_cast<long double>(
                    sparks[static_cast<std::size_t>(source)].x
                ) - static_cast<long double>(
                    sparks[static_cast<std::size_t>(target)].x
                );
            const long double delta_y =
                static_cast<long double>(sparks[static_cast<std::size_t>(source)].y) -
                static_cast<long double>(sparks[static_cast<std::size_t>(target)].y);
            const double arrival = lighting_time[static_cast<std::size_t>(source)] +
                static_cast<double>(std::sqrt(delta_x * delta_x + delta_y * delta_y));
            events.emplace(arrival, target);
            remaining--;
        }
    };

    lighting_time[0] = 0.0;
    lit[0] = 1;
    activate(0);
    int lit_count = 1;
    while(!events.empty() && lit_count < spark_count){
        const auto [time, target] = events.top();
        events.pop();
        if(lit[static_cast<std::size_t>(target)]) continue;
        lit[static_cast<std::size_t>(target)] = 1;
        lighting_time[static_cast<std::size_t>(target)] = time;
        lit_count++;
        activate(target);
    }
    if(lit_count != spark_count)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: unreachable spark "
            "(gondor_spark_lighting_times)."
        );
    }
    return lighting_time;
}
