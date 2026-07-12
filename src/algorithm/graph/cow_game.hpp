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
            if(result.maximum[static_cast<std::size_t>(e.from)] == inf) continue;
            T nd = result.maximum[static_cast<std::size_t>(e.from)] + e.cost;
            if(nd < result.maximum[static_cast<std::size_t>(e.to)]){
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
