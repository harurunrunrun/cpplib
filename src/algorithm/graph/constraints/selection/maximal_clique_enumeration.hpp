#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_SELECTION_MAXIMAL_CLIQUE_ENUMERATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_SELECTION_MAXIMAL_CLIQUE_ENUMERATION_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<std::vector<int>> enumerate_maximal_cliques(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(enumerate_maximal_cliques)."
        );
    }
    const std::size_t word_count =
        (static_cast<std::size_t>(vertex_count) + 63U) / 64U;
    std::vector<std::vector<std::uint64_t>> adjacent(
        static_cast<std::size_t>(vertex_count),
        std::vector<std::uint64_t>(word_count, 0)
    );
    for(auto [left, right]: edges){
        if(left < 0 || vertex_count <= left
            || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(enumerate_maximal_cliques)."
            );
        }
        if(left == right) continue;
        adjacent[static_cast<std::size_t>(left)]
            [static_cast<std::size_t>(right) >> 6U] |=
                std::uint64_t{1}
                    << (static_cast<unsigned>(right) & 63U);
        adjacent[static_cast<std::size_t>(right)]
            [static_cast<std::size_t>(left) >> 6U] |=
                std::uint64_t{1}
                    << (static_cast<unsigned>(left) & 63U);
    }
    std::vector<std::vector<int>> result;
    std::vector<int> current;
    std::vector<std::uint64_t> initial(word_count, ~std::uint64_t{0});
    if(vertex_count % 64 != 0 && !initial.empty()){
        initial.back() =
            (std::uint64_t{1}
                << static_cast<unsigned>(vertex_count % 64)) - 1;
    }
    std::function<void(
        std::vector<std::uint64_t>,
        std::vector<std::uint64_t>
    )> search;
    search = [&](std::vector<std::uint64_t> possible,
                 std::vector<std::uint64_t> excluded) -> void {
        bool possible_empty = true;
        bool excluded_empty = true;
        for(std::size_t word = 0; word < word_count; ++word){
            possible_empty &= possible[word] == 0;
            excluded_empty &= excluded[word] == 0;
        }
        if(possible_empty){
            if(excluded_empty){
                std::vector<int> clique = current;
                std::sort(clique.begin(), clique.end());
                result.push_back(std::move(clique));
            }
            return;
        }
        int pivot = -1;
        int pivot_neighbors = -1;
        for(std::size_t word = 0; word < word_count; ++word){
            std::uint64_t union_word = possible[word] | excluded[word];
            while(union_word != 0){
                unsigned bit = std::countr_zero(union_word);
                int vertex = static_cast<int>(word * 64U + bit);
                int count = 0;
                for(std::size_t index = 0; index < word_count; ++index){
                    count += std::popcount(
                        possible[index]
                            & adjacent[static_cast<std::size_t>(vertex)][index]
                    );
                }
                if(pivot_neighbors < count){
                    pivot_neighbors = count;
                    pivot = vertex;
                }
                union_word &= union_word - 1;
            }
        }
        std::vector<std::uint64_t> candidates = possible;
        if(pivot != -1){
            for(std::size_t word = 0; word < word_count; ++word){
                candidates[word] &=
                    ~adjacent[static_cast<std::size_t>(pivot)][word];
            }
        }
        for(std::size_t word = 0; word < word_count; ++word){
            while(candidates[word] != 0){
                unsigned bit = std::countr_zero(candidates[word]);
                int vertex = static_cast<int>(word * 64U + bit);
                std::vector<std::uint64_t> next_possible(word_count);
                std::vector<std::uint64_t> next_excluded(word_count);
                for(std::size_t index = 0; index < word_count; ++index){
                    next_possible[index] = possible[index]
                        & adjacent[static_cast<std::size_t>(vertex)][index];
                    next_excluded[index] = excluded[index]
                        & adjacent[static_cast<std::size_t>(vertex)][index];
                }
                current.push_back(vertex);
                search(std::move(next_possible), std::move(next_excluded));
                current.pop_back();
                possible[word] &= ~(std::uint64_t{1} << bit);
                excluded[word] |= std::uint64_t{1} << bit;
                candidates[word] &= candidates[word] - 1;
            }
        }
    };
    search(initial, std::vector<std::uint64_t>(word_count, 0));
    std::sort(result.begin(), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_SELECTION_MAXIMAL_CLIQUE_ENUMERATION_HPP_INCLUDED
