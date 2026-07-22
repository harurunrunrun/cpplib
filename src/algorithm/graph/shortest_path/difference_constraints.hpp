#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_DIFFERENCE_CONSTRAINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_DIFFERENCE_CONSTRAINTS_HPP_INCLUDED

#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
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

namespace difference_constraints_detail{

template<class T>
inline constexpr bool is_integer_like =
    std::is_integral_v<std::remove_cv_t<T>>
#ifdef __SIZEOF_INT128__
    || std::is_same_v<std::remove_cv_t<T>, __int128>
    || std::is_same_v<std::remove_cv_t<T>, unsigned __int128>
#endif
    ;

template<class T>
inline constexpr bool is_signed_integer_like =
    std::is_signed_v<std::remove_cv_t<T>>
#ifdef __SIZEOF_INT128__
    || std::is_same_v<std::remove_cv_t<T>, __int128>
#endif
    ;

template<class T>
T checked_add(const T& left, const T& right){
    if constexpr(is_integer_like<T>){
        if constexpr(is_signed_integer_like<T>){
            if((T(0) < right &&
                std::numeric_limits<T>::max() - right < left) ||
               (right < T(0) &&
                left < std::numeric_limits<T>::lowest() - right)){
                throw std::overflow_error(
                    "library assertion fault: arithmetic overflow "
                    "(difference constraints)."
                );
            }
        }else if(std::numeric_limits<T>::max() - right < left){
            throw std::overflow_error(
                "library assertion fault: arithmetic overflow "
                "(difference constraints)."
            );
        }
    }
    return left + right;
}

template<class T>
T checked_negate(const T& value){
    if constexpr(is_integer_like<T>){
        if constexpr(is_signed_integer_like<T>){
            if(value == std::numeric_limits<T>::lowest()){
                throw std::overflow_error(
                    "library assertion fault: arithmetic overflow "
                    "(difference constraints)."
                );
            }
        }else if(value != T(0)){
            throw std::overflow_error(
                "library assertion fault: arithmetic overflow "
                "(difference constraints)."
            );
        }
    }
    return -value;
}

}

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
    constraints.push_back({
        to, from, difference_constraints_detail::checked_negate(bound)
    });
}

template<class T>
void add_difference_equal(
    std::vector<DifferenceConstraint<T>>& constraints,
    int from,
    int to,
    T bound
){
    const T negated =
        difference_constraints_detail::checked_negate(bound);
    constraints.push_back({from, to, bound});
    constraints.push_back({to, from, negated});
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
    bool all_nonnegative = true;
    for(const auto& e: constraints){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (solve_difference_constraints).");
        }
        if(e.bound < T(0)) all_nonnegative = false;
    }

    if(all_nonnegative){
        DifferenceConstraintsResult<T> result;
        result.feasible = true;
        result.maximum.assign(static_cast<std::size_t>(n), inf);
        result.bounded.assign(static_cast<std::size_t>(n), 0);
        result.maximum[static_cast<std::size_t>(reference)] = T(0);
        result.bounded[static_cast<std::size_t>(reference)] = 1;

        std::vector<std::vector<std::size_t>> adjacency(
            static_cast<std::size_t>(n)
        );
        for(std::size_t index = 0; index < constraints.size(); ++index){
            adjacency[static_cast<std::size_t>(constraints[index].from)]
                .push_back(index);
        }

        using State = std::pair<T, int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>>
            queue;
        queue.push({T(0), reference});
        while(!queue.empty()){
            const auto [distance, vertex] = queue.top();
            queue.pop();
            if(result.maximum[static_cast<std::size_t>(vertex)] < distance){
                continue;
            }
            for(const std::size_t index:
                adjacency[static_cast<std::size_t>(vertex)]){
                const auto& edge = constraints[index];
                const T candidate =
                    difference_constraints_detail::checked_add(
                        distance, edge.bound
                    );
                const std::size_t to = static_cast<std::size_t>(edge.to);
                if(!result.bounded[to] || candidate < result.maximum[to]){
                    result.maximum[to] = candidate;
                    result.bounded[to] = 1;
                    queue.push({candidate, edge.to});
                }
            }
        }
        return result;
    }

    std::vector<T> potential(static_cast<std::size_t>(n), T(0));
    for(int iter = 0; iter < n; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            const T nd =
                difference_constraints_detail::checked_add(
                    potential[static_cast<std::size_t>(e.from)], e.bound
                );
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
            const T nd =
                difference_constraints_detail::checked_add(
                    result.maximum[static_cast<std::size_t>(e.from)], e.bound
                );
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
        answer.minimum =
            difference_constraints_detail::checked_negate(lower_dual.value);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_DIFFERENCE_CONSTRAINTS_HPP_INCLUDED
