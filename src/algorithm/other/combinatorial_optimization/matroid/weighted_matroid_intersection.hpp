#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_WEIGHTED_MATROID_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_WEIGHTED_MATROID_INTERSECTION_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "matroid_intersection.hpp"

namespace combinatorial_optimization {

template<class Weight>
struct WeightedMatroidIntersectionResult {
    std::vector<std::size_t> elements;
    Weight weight{};
};

template<class Weight, class FirstIndependenceOracle,
         class SecondIndependenceOracle>
WeightedMatroidIntersectionResult<Weight> weighted_matroid_intersection(
    const std::vector<Weight>& weights,
    FirstIndependenceOracle first_independent,
    SecondIndependenceOracle second_independent
) {
    const std::size_t size = weights.size();
    std::vector<unsigned char> selected(size, 0);
    if(!static_cast<bool>(std::invoke(
        first_independent, std::vector<std::size_t>{}
    )) || !static_cast<bool>(std::invoke(
        second_independent, std::vector<std::size_t>{}
    ))){
        throw std::invalid_argument(
            "a matroid oracle must accept the empty set"
        );
    }

    WeightedMatroidIntersectionResult<Weight> best;
    Weight current_weight{};
    while(true){
        struct Arc {
            std::size_t to;
            Weight gain;
        };
        std::vector<std::vector<Arc>> graph(size);
        std::vector<unsigned char> source(size, 0);
        std::vector<unsigned char> sink(size, 0);
        for(std::size_t element = 0; element < size; ++element){
            if(selected[element]) continue;
            source[element] =
                matroid_intersection_internal::addition_independent(
                    first_independent, selected, element
                );
            sink[element] =
                matroid_intersection_internal::addition_independent(
                    second_independent, selected, element
                );
        }
        for(std::size_t added = 0; added < size; ++added){
            if(selected[added]) continue;
            for(std::size_t removed = 0; removed < size; ++removed){
                if(!selected[removed]) continue;
                if(matroid_intersection_internal::exchange_independent(
                    second_independent, selected, removed, added
                )){
                    graph[added].push_back(Arc{
                        removed, -weights[removed]
                    });
                }
                if(matroid_intersection_internal::exchange_independent(
                    first_independent, selected, removed, added
                )){
                    graph[removed].push_back(Arc{
                        added, weights[added]
                    });
                }
            }
        }

        std::vector<std::optional<Weight>> distance(size);
        std::vector<std::size_t> path_length(size, size + 1);
        std::vector<int> parent(size, -2);
        for(std::size_t element = 0; element < size; ++element){
            if(source[element]){
                distance[element] = weights[element];
                path_length[element] = 0;
                parent[element] = -1;
            }
        }
        for(std::size_t iteration = 1; iteration < size; ++iteration){
            bool changed = false;
            for(std::size_t from = 0; from < size; ++from){
                if(!distance[from]) continue;
                for(const Arc& arc : graph[from]){
                    const Weight candidate = *distance[from] + arc.gain;
                    const std::size_t candidate_length =
                        path_length[from] + 1;
                    if(distance[arc.to]){
                        const bool larger =
                            *distance[arc.to] < candidate;
                        const bool equal =
                            !larger && !(candidate < *distance[arc.to]);
                        if(!larger
                           && !(equal
                                && candidate_length
                                    < path_length[arc.to])){
                            continue;
                        }
                    }
                    distance[arc.to] = candidate;
                    path_length[arc.to] = candidate_length;
                    parent[arc.to] = static_cast<int>(from);
                    changed = true;
                }
            }
            if(!changed) break;
        }

        std::size_t terminal = size;
        for(std::size_t element = 0; element < size; ++element){
            if(!sink[element] || !distance[element]) continue;
            if(terminal == size
               || *distance[terminal] < *distance[element]
               || (!(*distance[element] < *distance[terminal])
                   && path_length[element] < path_length[terminal])){
                terminal = element;
            }
        }
        if(terminal == size) break;
        const Weight gain = *distance[terminal];
        for(int vertex = static_cast<int>(terminal);
            vertex != -1;
            vertex = parent[static_cast<std::size_t>(vertex)]){
            selected[static_cast<std::size_t>(vertex)] ^= 1U;
        }
        current_weight += gain;
        if(best.elements.empty() && !(Weight{} < current_weight)){
            continue;
        }
        if(best.weight < current_weight){
            best.weight = current_weight;
            best.elements =
                matroid_intersection_internal::selected_elements(selected);
        }
    }
    return best;
}

}  // namespace combinatorial_optimization

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_WEIGHTED_MATROID_INTERSECTION_HPP_INCLUDED
