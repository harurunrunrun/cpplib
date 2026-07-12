#pragma once

#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
struct CowGameConstraint{
    int from;
    int to;
    T cost;
};

template<class T>
struct CowGameResult{
    bool feasible;
    std::vector<T> maximum;
    std::vector<char> bounded;
};

template<class T>
struct CowGameMaximumResult{
    bool feasible = false;
    bool bounded = false;
    T value{};
};

template<class T>
struct CowGameRangeResult{
    bool feasible = false;
    bool has_minimum = false;
    bool has_maximum = false;
    T minimum{};
    T maximum{};
};

template<class T>
void cow_game_add_at_most(
    std::vector<CowGameConstraint<T>>& constraints,
    int from,
    int to,
    T cost
){
    constraints.push_back({from, to, cost});
}

template<class T>
void cow_game_add_at_least(
    std::vector<CowGameConstraint<T>>& constraints,
    int from,
    int to,
    T cost
){
    constraints.push_back({to, from, -cost});
}

template<class T>
void cow_game_add_equal(
    std::vector<CowGameConstraint<T>>& constraints,
    int from,
    int to,
    T cost
){
    cow_game_add_at_most(constraints, from, to, cost);
    cow_game_add_at_least(constraints, from, to, cost);
}

template<class T>
CowGameResult<T> cow_game(
    int n,
    int source,
    const std::vector<CowGameConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(n < 0 || source < 0 || n <= source)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (cow_game).");
    }
    for(const auto& e: constraints){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (cow_game).");
        }
    }

    std::vector<T> potential(static_cast<std::size_t>(n), T(0));
    for(int iter = 0; iter < n; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            T nd = potential[static_cast<std::size_t>(e.from)] + e.cost;
            if(nd < potential[static_cast<std::size_t>(e.to)]){
                potential[static_cast<std::size_t>(e.to)] = nd;
                updated = true;
            }
        }
        if(!updated) break;
        if(iter == n - 1){
            CowGameResult<T> result;
            result.feasible = false;
            return result;
        }
    }

    CowGameResult<T> result;
    result.feasible = true;
    result.maximum.assign(static_cast<std::size_t>(n), inf);
    result.bounded.assign(static_cast<std::size_t>(n), 0);
    result.maximum[static_cast<std::size_t>(source)] = T(0);
    result.bounded[static_cast<std::size_t>(source)] = 1;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            if(!result.bounded[static_cast<std::size_t>(e.from)]) continue;
            T nd = result.maximum[static_cast<std::size_t>(e.from)] + e.cost;
            if(!result.bounded[static_cast<std::size_t>(e.to)] ||
               nd < result.maximum[static_cast<std::size_t>(e.to)]){
                result.maximum[static_cast<std::size_t>(e.to)] = nd;
                result.bounded[static_cast<std::size_t>(e.to)] = 1;
                updated = true;
            }
        }
        if(!updated) break;
    }
    return result;
}

template<class T>
bool cow_game_has_maximum(const CowGameResult<T>& result, int target){
    if(!result.feasible) return false;
    if(target < 0 || static_cast<int>(result.bounded.size()) <= target)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (cow_game_has_maximum).");
    }
    return result.bounded[static_cast<std::size_t>(target)];
}

template<class T>
CowGameMaximumResult<T> cow_game_max_difference(
    int n,
    int source,
    int target,
    const std::vector<CowGameConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(target < 0 || n <= target)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (cow_game_max_difference).");
    }
    auto result = cow_game(n, source, constraints, inf);
    if(!result.feasible){
        return {};
    }

    CowGameMaximumResult<T> answer;
    answer.feasible = true;
    answer.bounded = result.bounded[static_cast<std::size_t>(target)];
    if(answer.bounded){
        answer.value = result.maximum[static_cast<std::size_t>(target)];
    }
    return answer;
}

template<class T>
CowGameRangeResult<T> cow_game_difference_range(
    int n,
    int source,
    int target,
    const std::vector<CowGameConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
){
    auto upper = cow_game_max_difference(n, source, target, constraints, inf);
    if(!upper.feasible){
        return {};
    }
    auto lower_dual = cow_game_max_difference(n, target, source, constraints, inf);
    if(!lower_dual.feasible){
        return {};
    }

    CowGameRangeResult<T> answer;
    answer.feasible = true;
    answer.has_maximum = upper.bounded;
    if(answer.has_maximum){
        answer.maximum = upper.value;
    }
    answer.has_minimum = lower_dual.bounded;
    if(answer.has_minimum){
        answer.minimum = -lower_dual.value;
    }
    return answer;
}
