#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_DISCREPANCY_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_DISCREPANCY_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

enum class DiscrepancySearchStop{
    exhausted,
    goal_found,
    depth_limit,
    discrepancy_limit,
    expansion_limit,
    broadening_limit
};

template<class State>
struct DiscrepancySearchResult{
    std::optional<State> solution;
    std::vector<State> path;
    std::size_t expanded = 0;
    std::size_t generated = 1;
    std::size_t iterations = 0;
    DiscrepancySearchStop stop = DiscrepancySearchStop::exhausted;

    explicit operator bool() const noexcept{
        return solution.has_value();
    }
};

namespace discrepancy_search_internal{

template<class Range>
using range_value_t = std::decay_t<decltype(
    *std::begin(std::declval<std::remove_reference_t<Range>&>())
)>;

template<class State, class Goal>
bool accept_goal(
    const State& state,
    Goal& goal,
    const std::vector<State>& path,
    DiscrepancySearchResult<State>& result
){
    if(!static_cast<bool>(std::invoke(goal, state))) return false;
    result.solution = state;
    result.path = path;
    result.stop = DiscrepancySearchStop::goal_found;
    return true;
}

}  // namespace discrepancy_search_internal

template<class State, class Goal, class Children,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
DiscrepancySearchResult<State> limited_discrepancy_search(
    const State& root,
    Goal goal,
    Children children,
    std::size_t maximum_depth,
    std::size_t maximum_discrepancies,
    std::size_t maximum_expansions = std::numeric_limits<std::size_t>::max(),
    Hash hash = {},
    Equal equal = {}
){
    DiscrepancySearchResult<State> result;
    bool saw_depth_cutoff = false;
    bool saw_discrepancy_cutoff = false;
    bool expansion_cutoff = false;
    for(std::size_t limit = 0; limit <= maximum_discrepancies; ++limit){
        ++result.iterations;
        std::vector<State> path{root};
        std::unordered_set<State, Hash, Equal> on_path(0, hash, equal);
        on_path.insert(root);
        const auto visit = [&](auto&& self, const State& state,
                               std::size_t depth,
                               std::size_t used) -> bool {
            if(result.expanded == maximum_expansions){
                expansion_cutoff = true;
                return false;
            }
            ++result.expanded;
            if(discrepancy_search_internal::accept_goal(
                   state, goal, path, result
               )){
                return true;
            }
            if(depth == maximum_depth){
                saw_depth_cutoff = true;
                return false;
            }
            auto next = std::invoke(children, state);
            std::size_t rank = 0;
            for(const auto& child : next){
                const std::size_t additional = rank == 0 ? 0 : 1;
                ++rank;
                ++result.generated;
                if(used + additional > limit){
                    saw_discrepancy_cutoff = true;
                    continue;
                }
                if(on_path.find(child) != on_path.end()) continue;
                path.push_back(child);
                on_path.insert(child);
                if(self(self, child, depth + 1, used + additional)) return true;
                on_path.erase(child);
                path.pop_back();
                if(expansion_cutoff) return false;
            }
            return false;
        };
        if(visit(visit, root, 0, 0)) return result;
        if(expansion_cutoff){
            result.stop = DiscrepancySearchStop::expansion_limit;
            return result;
        }
    }
    if(saw_discrepancy_cutoff){
        result.stop = DiscrepancySearchStop::discrepancy_limit;
    }else if(saw_depth_cutoff){
        result.stop = DiscrepancySearchStop::depth_limit;
    }
    return result;
}

template<class State, class Goal, class Children,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
DiscrepancySearchResult<State> depth_bounded_discrepancy_search(
    const State& root,
    Goal goal,
    Children children,
    std::size_t maximum_depth,
    std::size_t maximum_expansions = std::numeric_limits<std::size_t>::max(),
    Hash hash = {},
    Equal equal = {}
){
    DiscrepancySearchResult<State> result;
    bool expansion_cutoff = false;
    bool saw_depth_cutoff = false;
    for(std::size_t discrepancy_depth = 0;
        discrepancy_depth <= maximum_depth;
        ++discrepancy_depth){
        ++result.iterations;
        std::vector<State> path{root};
        std::unordered_set<State, Hash, Equal> on_path(0, hash, equal);
        on_path.insert(root);
        const auto visit = [&](auto&& self, const State& state,
                               std::size_t depth) -> bool {
            if(result.expanded == maximum_expansions){
                expansion_cutoff = true;
                return false;
            }
            ++result.expanded;
            if(discrepancy_search_internal::accept_goal(
                   state, goal, path, result
               )){
                return true;
            }
            if(depth == maximum_depth){
                saw_depth_cutoff = true;
                return false;
            }
            auto next = std::invoke(children, state);
            std::size_t rank = 0;
            for(const auto& child : next){
                const bool preferred = rank++ == 0;
                ++result.generated;
                if(!preferred && depth >= discrepancy_depth) continue;
                if(on_path.find(child) != on_path.end()) continue;
                path.push_back(child);
                on_path.insert(child);
                if(self(self, child, depth + 1)) return true;
                on_path.erase(child);
                path.pop_back();
                if(expansion_cutoff) return false;
            }
            return false;
        };
        if(visit(visit, root, 0)) return result;
        if(expansion_cutoff){
            result.stop = DiscrepancySearchStop::expansion_limit;
            return result;
        }
    }
    if(saw_depth_cutoff) result.stop = DiscrepancySearchStop::depth_limit;
    return result;
}

template<class State, class Goal, class Children,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
DiscrepancySearchResult<State> improved_limited_discrepancy_search(
    const State& root,
    Goal goal,
    Children children,
    std::size_t maximum_depth,
    std::size_t maximum_discrepancies,
    std::size_t maximum_expansions = std::numeric_limits<std::size_t>::max(),
    Hash hash = {},
    Equal equal = {}
){
    DiscrepancySearchResult<State> result;
    bool expansion_cutoff = false;
    bool saw_depth_cutoff = false;
    for(std::size_t exact = 0; exact <= maximum_discrepancies; ++exact){
        ++result.iterations;
        std::vector<State> path{root};
        std::unordered_set<State, Hash, Equal> on_path(0, hash, equal);
        on_path.insert(root);
        const auto visit = [&](auto&& self, const State& state,
                               std::size_t depth,
                               std::size_t used) -> bool {
            if(result.expanded == maximum_expansions){
                expansion_cutoff = true;
                return false;
            }
            ++result.expanded;
            if(discrepancy_search_internal::accept_goal(
                   state, goal, path, result
               )){
                return true;
            }
            if(depth == maximum_depth){
                saw_depth_cutoff = true;
                return false;
            }
            if(used > exact || exact - used > maximum_depth - depth) return false;
            auto next = std::invoke(children, state);
            std::size_t rank = 0;
            for(const auto& child : next){
                const std::size_t additional = rank++ == 0 ? 0 : 1;
                ++result.generated;
                if(used + additional > exact) continue;
                if(on_path.find(child) != on_path.end()) continue;
                path.push_back(child);
                on_path.insert(child);
                if(self(self, child, depth + 1, used + additional)) return true;
                on_path.erase(child);
                path.pop_back();
                if(expansion_cutoff) return false;
            }
            return false;
        };
        if(visit(visit, root, 0, 0)) return result;
        if(expansion_cutoff){
            result.stop = DiscrepancySearchStop::expansion_limit;
            return result;
        }
    }
    result.stop = saw_depth_cutoff
        ? DiscrepancySearchStop::depth_limit
        : DiscrepancySearchStop::discrepancy_limit;
    return result;
}

template<class State, class Goal, class Children,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
DiscrepancySearchResult<State> iterative_broadening_search(
    const State& root,
    Goal goal,
    Children children,
    std::size_t maximum_depth,
    std::size_t maximum_width,
    std::size_t maximum_expansions = std::numeric_limits<std::size_t>::max(),
    Hash hash = {},
    Equal equal = {}
){
    if(maximum_width == 0)[[unlikely]]{
        throw std::invalid_argument(
            "iterative_broadening_search requires a positive width"
        );
    }
    DiscrepancySearchResult<State> result;
    bool expansion_cutoff = false;
    bool width_cutoff = false;
    bool depth_cutoff = false;
    for(std::size_t width = 1; width <= maximum_width; ++width){
        ++result.iterations;
        std::vector<State> path{root};
        std::unordered_set<State, Hash, Equal> on_path(0, hash, equal);
        on_path.insert(root);
        const auto visit = [&](auto&& self, const State& state,
                               std::size_t depth) -> bool {
            if(result.expanded == maximum_expansions){
                expansion_cutoff = true;
                return false;
            }
            ++result.expanded;
            if(discrepancy_search_internal::accept_goal(
                   state, goal, path, result
               )){
                return true;
            }
            if(depth == maximum_depth){
                depth_cutoff = true;
                return false;
            }
            auto next = std::invoke(children, state);
            std::size_t rank = 0;
            for(const auto& child : next){
                if(rank++ == width){
                    width_cutoff = true;
                    break;
                }
                ++result.generated;
                if(on_path.find(child) != on_path.end()) continue;
                path.push_back(child);
                on_path.insert(child);
                if(self(self, child, depth + 1)) return true;
                on_path.erase(child);
                path.pop_back();
                if(expansion_cutoff) return false;
            }
            return false;
        };
        if(visit(visit, root, 0)) return result;
        if(expansion_cutoff){
            result.stop = DiscrepancySearchStop::expansion_limit;
            return result;
        }
    }
    if(width_cutoff){
        result.stop = DiscrepancySearchStop::broadening_limit;
    }else if(depth_cutoff){
        result.stop = DiscrepancySearchStop::depth_limit;
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_DISCREPANCY_SEARCH_HPP_INCLUDED
