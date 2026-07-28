#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_UNION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_UNION_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace combinatorial_optimization {

struct MatroidUnionResult {
    std::vector<std::vector<std::size_t>> groups;
    std::vector<std::size_t> uncovered;
};

template<class IndependenceOracle>
MatroidUnionResult matroid_union(
    std::size_t ground_set_size,
    std::size_t copy_count,
    IndependenceOracle independent
) {
    MatroidUnionResult result;
    if(copy_count > result.groups.max_size())[[unlikely]]{
        throw std::length_error("matroid union copy count is too large");
    }
    result.groups.resize(copy_count);
    if(copy_count == 0){
        result.uncovered.resize(ground_set_size);
        for(std::size_t element = 0;
            element < ground_set_size;
            ++element){
            result.uncovered[element] = element;
        }
        return result;
    }
    const std::size_t no_element = ground_set_size;
    std::vector<std::size_t> owner(ground_set_size, copy_count);
    const auto rebuild_groups = [&]{
        for(auto& group: result.groups) group.clear();
        for(std::size_t element = 0;
            element < ground_set_size;
            ++element){
            if(owner[element] != copy_count){
                result.groups[owner[element]].push_back(element);
            }
        }
    };

    while(true){
        std::queue<std::size_t> queue;
        std::vector<std::size_t> predecessor(
            ground_set_size, no_element
        );
        std::vector<std::size_t> exchange_group(
            ground_set_size, copy_count
        );
        std::vector<unsigned char> reached(ground_set_size, 0);
        for(std::size_t element = 0;
            element < ground_set_size;
            ++element){
            if(owner[element] == copy_count){
                reached[element] = 1;
                queue.push(element);
            }
        }

        std::size_t sink = no_element;
        std::size_t sink_group = copy_count;
        while(!queue.empty() && sink == no_element){
            const std::size_t element = queue.front();
            queue.pop();
            for(std::size_t group = 0; group < copy_count; ++group){
                if(owner[element] == group) continue;
                std::vector<std::size_t> candidate =
                    result.groups[group];
                candidate.push_back(element);
                if(static_cast<bool>(std::invoke(independent, candidate))){
                    sink = element;
                    sink_group = group;
                    break;
                }
                for(const std::size_t displaced: result.groups[group]){
                    candidate.clear();
                    candidate.reserve(result.groups[group].size());
                    for(const std::size_t member: result.groups[group]){
                        if(member != displaced) candidate.push_back(member);
                    }
                    candidate.push_back(element);
                    if(!static_cast<bool>(
                        std::invoke(independent, candidate)
                    )) continue;
                    if(reached[displaced] == 0){
                        reached[displaced] = 1;
                        predecessor[displaced] = element;
                        exchange_group[displaced] = group;
                        queue.push(displaced);
                    }
                }
            }
        }
        if(sink == no_element) break;

        std::size_t element = sink;
        std::size_t group = sink_group;
        while(true){
            const std::size_t previous = predecessor[element];
            owner[element] = group;
            if(previous == no_element) break;
            group = exchange_group[element];
            element = previous;
        }
        rebuild_groups();
    }

    for(std::size_t element = 0;
        element < ground_set_size;
        ++element){
        if(owner[element] == copy_count){
            result.uncovered.push_back(element);
        }
    }
    return result;
}

}  // namespace combinatorial_optimization

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_UNION_HPP_INCLUDED
