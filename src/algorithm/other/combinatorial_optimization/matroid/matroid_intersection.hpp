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

}  // namespace matroid_intersection_internal

template<class FirstIndependenceOracle, class SecondIndependenceOracle>
MatroidIntersectionResult matroid_intersection(
    std::size_t ground_set_size,
    FirstIndependenceOracle first_independent,
    SecondIndependenceOracle second_independent
) {
    std::vector<unsigned char> selected(ground_set_size, 0);
    if(!static_cast<bool>(std::invoke(
        first_independent, std::vector<std::size_t>{}
    )) || !static_cast<bool>(std::invoke(
        second_independent, std::vector<std::size_t>{}
    ))){
        throw std::invalid_argument(
            "a matroid oracle must accept the empty set"
        );
    }

    while(true){
        std::vector<int> parent(ground_set_size, -2);
        std::queue<std::size_t> queue;
        for(std::size_t element = 0;
            element < ground_set_size;
            ++element){
            if(selected[element]) continue;
            if(matroid_intersection_internal::addition_independent(
                first_independent, selected, element
            )){
                parent[element] = -1;
                queue.push(element);
            }
        }
        std::size_t sink = ground_set_size;
        while(!queue.empty() && sink == ground_set_size){
            const std::size_t current = queue.front();
            queue.pop();
            if(!selected[current]){
                if(matroid_intersection_internal::addition_independent(
                    second_independent, selected, current
                )){
                    sink = current;
                    break;
                }
                for(std::size_t removed = 0;
                    removed < ground_set_size;
                    ++removed){
                    if(!selected[removed] || parent[removed] != -2) continue;
                    if(matroid_intersection_internal::exchange_independent(
                        second_independent, selected, removed, current
                    )){
                        parent[removed] = static_cast<int>(current);
                        queue.push(removed);
                    }
                }
            }else{
                for(std::size_t added = 0;
                    added < ground_set_size;
                    ++added){
                    if(selected[added] || parent[added] != -2) continue;
                    if(matroid_intersection_internal::exchange_independent(
                        first_independent, selected, current, added
                    )){
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

    MatroidIntersectionResult result;
    result.elements =
        matroid_intersection_internal::selected_elements(selected);
    return result;
}

}  // namespace combinatorial_optimization

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_INTERSECTION_HPP_INCLUDED
