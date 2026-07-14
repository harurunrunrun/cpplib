#pragma once

#include <cstddef>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct PilotSalaries {
    long long captain;
    long long assistant;
};

inline long long minimum_pilot_salary(
    const std::vector<PilotSalaries>& pilots_by_increasing_age
){
    if(pilots_by_increasing_age.size() % 2 != 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: odd pilot count (minimum_pilot_salary)."
        );
    }
    std::priority_queue<long long, std::vector<long long>, std::greater<long long>> deltas;
    long long result = 0;
    int balance = 0;
    for(const auto& pilot: pilots_by_increasing_age){
        if(pilot.captain < 0 || pilot.assistant < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative salary (minimum_pilot_salary)."
            );
        }
        result += pilot.captain;
        deltas.push(pilot.assistant - pilot.captain);
        balance--;
        if(balance < 0){
            result += deltas.top();
            deltas.pop();
            balance += 2;
        }
    }
    return result;
}
