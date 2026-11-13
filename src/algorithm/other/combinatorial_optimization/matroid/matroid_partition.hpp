#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_PARTITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_PARTITION_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "matroid_intersection.hpp"

namespace combinatorial_optimization {

struct MatroidPartitionResult {
    std::vector<std::vector<std::size_t>> groups;
    std::vector<std::size_t> uncovered;
};

namespace matroid_partition_internal {

// Copy group * ground_set_size + element stands for (group, element).
// The current selection is a common independent set, so an exchange can
// only break the one-copy constraint at the added element and the direct
// sum at the group receiving the added copy (matroids are hereditary).
struct OneCopyPerElement {
    static constexpr std::size_t none = static_cast<std::size_t>(-1);
    std::size_t ground_set_size;
    std::vector<std::size_t> chosen_copy;

    void reset(const std::vector<unsigned char>& selected) {
        chosen_copy.assign(ground_set_size, none);
        for(std::size_t copy = 0; copy < selected.size(); ++copy){
            if(selected[copy]) chosen_copy[copy % ground_set_size] = copy;
        }
    }

    bool operator()(std::size_t removed, std::size_t added) const {
        const std::size_t chosen = chosen_copy[added % ground_set_size];
        return chosen == none || chosen == removed;
    }
};

template<class IndependenceOracle>
struct DirectSum {
    static constexpr std::size_t none = static_cast<std::size_t>(-1);
    std::size_t ground_set_size;
    const std::vector<IndependenceOracle>& independent;
    std::vector<std::vector<std::size_t>> groups;

    void reset(const std::vector<unsigned char>& selected) {
        for(std::vector<std::size_t>& group : groups) group.clear();
        for(std::size_t copy = 0; copy < selected.size(); ++copy){
            if(selected[copy]){
                groups[copy / ground_set_size].push_back(
                    copy % ground_set_size
                );
            }
        }
    }

    bool operator()(std::size_t removed, std::size_t added) const {
        const std::size_t group = added / ground_set_size;
        const std::size_t removed_element =
            removed != none && removed / ground_set_size == group
                ? removed % ground_set_size
                : none;
        return static_cast<bool>(std::invoke(
            independent[group],
            matroid_intersection_internal::exchanged_elements(
                groups[group], removed_element,
                added % ground_set_size
            )
        ));
    }
};

}  // namespace matroid_partition_internal

template<class IndependenceOracle>
MatroidPartitionResult matroid_partition(
    std::size_t ground_set_size,
    const std::vector<IndependenceOracle>& independent
) {
    if(independent.empty()){
        MatroidPartitionResult result;
        result.uncovered.resize(ground_set_size);
        for(std::size_t element = 0;
            element < ground_set_size;
            ++element){
            result.uncovered[element] = element;
        }
        return result;
    }
    const std::size_t matroid_count = independent.size();
    if(ground_set_size != 0
       && matroid_count
            > static_cast<std::size_t>(-1) / ground_set_size){
        throw std::length_error("matroid partition copy set is too large");
    }
    const std::size_t copy_count = ground_set_size * matroid_count;
    for(std::size_t index = 0; index < matroid_count; ++index){
        if(!static_cast<bool>(std::invoke(
            independent[index], std::vector<std::size_t>{}
        ))){
            throw std::invalid_argument(
                "a matroid oracle must accept the empty set"
            );
        }
    }
    matroid_partition_internal::OneCopyPerElement first_exchange{
        ground_set_size, {}
    };
    matroid_partition_internal::DirectSum<IndependenceOracle> second_exchange{
        ground_set_size, independent,
        std::vector<std::vector<std::size_t>>(matroid_count)
    };
    MatroidIntersectionResult intersection;
    intersection.elements = matroid_intersection_internal::augment(
        copy_count, first_exchange, second_exchange
    );
    MatroidPartitionResult result;
    result.groups.resize(matroid_count);
    std::vector<unsigned char> covered(ground_set_size, 0);
    for(const std::size_t copy : intersection.elements){
        const std::size_t group = copy / ground_set_size;
        const std::size_t element = copy % ground_set_size;
        result.groups[group].push_back(element);
        covered[element] = 1;
    }
    for(std::size_t element = 0;
        element < ground_set_size;
        ++element){
        if(!covered[element]) result.uncovered.push_back(element);
    }
    return result;
}

}  // namespace combinatorial_optimization

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_PARTITION_HPP_INCLUDED
