#pragma once

#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
struct DifferenceConstraint{
    int from;
    int to;
    T bound;
};

template<class T>
struct DifferenceConstraintsResult{
    bool feasible;
    std::vector<T> maximum;
    std::vector<char> bounded;
};

template<class T>
struct DifferenceUpperBoundResult{
    bool feasible = false;
    bool bounded = false;
    T value{};
};

template<class T>
struct DifferenceRangeResult{
    bool feasible = false;
    bool has_minimum = false;
    bool has_maximum = false;
    T minimum{};
    T maximum{};
};

template<class T>
void add_difference_at_most(
    std::vector<DifferenceConstraint<T>>& constraints,
    int from,
    int to,
    T bound
){
    constraints.push_back({from, to, bound});
}

template<class T>
void add_difference_at_least(
    std::vector<DifferenceConstraint<T>>& constraints,
    int from,
    int to,
    T bound
){
    constraints.push_back({to, from, -bound});
}

template<class T>
void add_difference_equal(
    std::vector<DifferenceConstraint<T>>& constraints,
    int from,
    int to,
    T bound
){
    add_difference_at_most(constraints, from, to, bound);
    add_difference_at_least(constraints, from, to, bound);
}

template<class T>
DifferenceConstraintsResult<T> solve_difference_constraints(
    int n,
    int reference,
    const std::vector<DifferenceConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(n < 0 || reference < 0 || n <= reference)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (solve_difference_constraints).");
    }
    for(const auto& e: constraints){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (solve_difference_constraints).");
        }
    }

    std::vector<T> potential(static_cast<std::size_t>(n), T(0));
    for(int iter = 0; iter < n; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            T nd = potential[static_cast<std::size_t>(e.from)] + e.bound;
            if(nd < potential[static_cast<std::size_t>(e.to)]){
                potential[static_cast<std::size_t>(e.to)] = nd;
                updated = true;
            }
        }
        if(!updated) break;
        if(iter == n - 1){
            DifferenceConstraintsResult<T> result;
            result.feasible = false;
            return result;
        }
    }

    DifferenceConstraintsResult<T> result;
    result.feasible = true;
    result.maximum.assign(static_cast<std::size_t>(n), inf);
    result.bounded.assign(static_cast<std::size_t>(n), 0);
    result.maximum[static_cast<std::size_t>(reference)] = T(0);
    result.bounded[static_cast<std::size_t>(reference)] = 1;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            if(!result.bounded[static_cast<std::size_t>(e.from)]) continue;
            T nd = result.maximum[static_cast<std::size_t>(e.from)] + e.bound;
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
bool has_finite_difference_upper_bound(const DifferenceConstraintsResult<T>& result, int variable){
    if(!result.feasible) return false;
    if(variable < 0 || static_cast<int>(result.bounded.size()) <= variable)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (has_finite_difference_upper_bound).");
    }
    return result.bounded[static_cast<std::size_t>(variable)];
}

template<class T>
DifferenceUpperBoundResult<T> maximum_difference(
    int n,
    int reference,
    int variable,
    const std::vector<DifferenceConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(variable < 0 || n <= variable)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (maximum_difference).");
    }
    auto result = solve_difference_constraints(n, reference, constraints, inf);
    if(!result.feasible){
        return {};
    }

    DifferenceUpperBoundResult<T> answer;
    answer.feasible = true;
    answer.bounded = result.bounded[static_cast<std::size_t>(variable)];
    if(answer.bounded){
        answer.value = result.maximum[static_cast<std::size_t>(variable)];
    }
    return answer;
}

template<class T>
DifferenceRangeResult<T> difference_range(
    int n,
    int reference,
    int variable,
    const std::vector<DifferenceConstraint<T>>& constraints,
    T inf = std::numeric_limits<T>::max() / 4
){
    auto upper = maximum_difference(n, reference, variable, constraints, inf);
    if(!upper.feasible){
        return {};
    }
    auto lower_dual = maximum_difference(n, variable, reference, constraints, inf);
    if(!lower_dual.feasible){
        return {};
    }

    DifferenceRangeResult<T> answer;
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
