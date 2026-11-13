#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace combinatorial_optimization {

struct MatroidIntersectionResult {
    std::vector<std::size_t> elements;
};

namespace matroid_intersection_internal {

inline std::vector<std::size_t> selected_elements(
    const std::vector<unsigned char>& selected,
    std::size_t removed = static_cast<std::size_t>(-1),
    std::size_t added = static_cast<std::size_t>(-1)
) {
    std::vector<std::size_t> result;
    result.reserve(selected.size());
    for(std::size_t element = 0; element < selected.size(); ++element){
        if(element != removed && selected[element]){
            result.push_back(element);
        }
    }
    if(added != static_cast<std::size_t>(-1)){
        result.push_back(added);
        std::sort(result.begin(), result.end());
    }
    return result;
}

template<class Oracle>
bool exchange_independent(
    const Oracle& oracle,
    const std::vector<unsigned char>& selected,
    std::size_t removed,
    std::size_t added
) {
    return static_cast<bool>(std::invoke(
        oracle, selected_elements(selected, removed, added)
    ));
}

template<class Oracle>
bool addition_independent(
    const Oracle& oracle,
    const std::vector<unsigned char>& selected,
    std::size_t added
) {
    return static_cast<bool>(std::invoke(
        oracle, selected_elements(selected, static_cast<std::size_t>(-1), added)
    ));
}

inline std::vector<std::size_t> exchanged_elements(
    const std::vector<std::size_t>& base,
    std::size_t removed,
    std::size_t added
) {
    std::vector<std::size_t> result;
    result.reserve(base.size() + 1);
    const auto position = std::lower_bound(base.begin(), base.end(), added);
    for(auto it = base.begin(); it != position; ++it){
        if(*it != removed) result.push_back(*it);
    }
    result.push_back(added);
    for(auto it = position; it != base.end(); ++it){
        if(*it != removed) result.push_back(*it);
    }
    return result;
}

// Adapts a set oracle to the exchange interface used by augment().
template<class Oracle>
struct SortedSetExchangeOracle {
    const Oracle& oracle;
    std::vector<std::size_t> base;

    void reset(const std::vector<unsigned char>& selected) {
        base = selected_elements(selected);
    }

    // removed == size_t(-1) means a pure addition.
    bool operator()(std::size_t removed, std::size_t added) const {
        return static_cast<bool>(std::invoke(
            oracle, exchanged_elements(base, removed, added)
        ));
    }
};

// Shortest augmenting path matroid intersection. Each exchange oracle has
// reset(selected), called once per phase, and operator()(removed, added)
// telling whether selected - removed + added is independent.
template<class FirstExchangeOracle, class SecondExchangeOracle>
std::vector<std::size_t> augment(
    std::size_t ground_set_size,
    FirstExchangeOracle& first_exchange,
    SecondExchangeOracle& second_exchange
) {
    constexpr std::size_t none = static_cast<std::size_t>(-1);
    std::vector<unsigned char> selected(ground_set_size, 0);
    while(true){
        first_exchange.reset(selected);
        second_exchange.reset(selected);
        std::vector<int> parent(ground_set_size, -2);
        std::queue<std::size_t> queue;
        for(std::size_t element = 0;
            element < ground_set_size;
            ++element){
            if(selected[element]) continue;
            if(first_exchange(none, element)){
                parent[element] = -1;
                queue.push(element);
            }
        }
        std::size_t sink = ground_set_size;
        while(!queue.empty() && sink == ground_set_size){
            const std::size_t current = queue.front();
            queue.pop();
            if(!selected[current]){
                if(second_exchange(none, current)){
                    sink = current;
                    break;
                }
                for(std::size_t removed = 0;
                    removed < ground_set_size;
                    ++removed){
                    if(!selected[removed] || parent[removed] != -2) continue;
                    if(second_exchange(removed, current)){
                        parent[removed] = static_cast<int>(current);
                        queue.push(removed);
                    }
                }
            }else{
                for(std::size_t added = 0;
                    added < ground_set_size;
                    ++added){
                    if(selected[added] || parent[added] != -2) continue;
                    if(first_exchange(current, added)){
                        parent[added] = static_cast<int>(current);
                        queue.push(added);
                    }
                }
            }
        }
        if(sink == ground_set_size) break;
        for(int vertex = static_cast<int>(sink);
            vertex != -1;
            vertex = parent[static_cast<std::size_t>(vertex)]){
            selected[static_cast<std::size_t>(vertex)] ^= 1U;
        }
    }
    return selected_elements(selected);
}

}  // namespace matroid_intersection_internal

template<class FirstIndependenceOracle, class SecondIndependenceOracle>
MatroidIntersectionResult matroid_intersection(
    std::size_t ground_set_size,
    FirstIndependenceOracle first_independent,
    SecondIndependenceOracle second_independent
) {
    if(!static_cast<bool>(std::invoke(
        first_independent, std::vector<std::size_t>{}
    )) || !static_cast<bool>(std::invoke(
        second_independent, std::vector<std::size_t>{}
    ))){
        throw std::invalid_argument(
            "a matroid oracle must accept the empty set"
        );
    }

    matroid_intersection_internal::SortedSetExchangeOracle<
        FirstIndependenceOracle
    > first_exchange{first_independent, {}};
    matroid_intersection_internal::SortedSetExchangeOracle<
        SecondIndependenceOracle
    > second_exchange{second_independent, {}};
    MatroidIntersectionResult result;
    result.elements = matroid_intersection_internal::augment(
        ground_set_size, first_exchange, second_exchange
    );
    return result;
}

}  // namespace combinatorial_optimization

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_INTERSECTION_HPP_INCLUDED
