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
    const auto one_copy_per_element = [ground_set_size](
        const std::vector<std::size_t>& copies
    ){
        std::vector<unsigned char> used(ground_set_size, 0);
        for(const std::size_t copy : copies){
            const std::size_t element = copy % ground_set_size;
            if(used[element]) return false;
            used[element] = 1;
        }
        return true;
    };
    const auto direct_sum = [
        ground_set_size,
        matroid_count,
        &independent
    ](const std::vector<std::size_t>& copies){
        std::vector<std::vector<std::size_t>> groups(matroid_count);
        for(const std::size_t copy : copies){
            groups[copy / ground_set_size].push_back(
                copy % ground_set_size
            );
        }
        for(std::size_t index = 0; index < matroid_count; ++index){
            if(!static_cast<bool>(std::invoke(
                independent[index], groups[index]
            ))){
                return false;
            }
        }
        return true;
    };
    const auto intersection = matroid_intersection(
        copy_count, one_copy_per_element, direct_sum
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
