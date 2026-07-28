#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_GENERAL_B_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_GENERAL_B_MATCHING_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "micali_vazirani_matching.hpp"

struct GeneralBMatchingResult{
    int size;
    std::vector<int> selected_edge_ids;
    std::vector<int> degree;
};

struct GeneralBMatching{
    int vertex_count;
    std::vector<int> capacity;
    std::vector<std::pair<int, int>> edges;

    explicit GeneralBMatching(std::vector<int> capacity_)
        : vertex_count(static_cast<int>(capacity_.size())),
          capacity(std::move(capacity_)){
        for(int value: capacity){
            if(value < 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: negative capacity "
                    "(GeneralBMatching)."
                );
            }
        }
    }

    int add_edge(int first, int second){
        if(first < 0 || vertex_count <= first
            || second < 0 || vertex_count <= second)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(GeneralBMatching::add_edge)."
            );
        }
        edges.emplace_back(first, second);
        return static_cast<int>(edges.size()) - 1;
    }

    GeneralBMatchingResult solve() const{
        long long copy_count_wide = 0;
        for(int value: capacity) copy_count_wide += value;
        const long long auxiliary_count_wide =
            copy_count_wide + static_cast<long long>(edges.size()) * 2;
        if(auxiliary_count_wide > std::numeric_limits<int>::max())
            [[unlikely]]{
            throw std::length_error(
                "auxiliary graph is too large (GeneralBMatching::solve)"
            );
        }
        const int copy_count = static_cast<int>(copy_count_wide);
        const int auxiliary_count =
            static_cast<int>(auxiliary_count_wide);
        std::vector<std::vector<int>> copies(
            static_cast<std::size_t>(vertex_count)
        );
        int next_copy = 0;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            auto& row = copies[static_cast<std::size_t>(vertex)];
            row.reserve(static_cast<std::size_t>(
                capacity[static_cast<std::size_t>(vertex)]
            ));
            for(int index = 0;
                index < capacity[static_cast<std::size_t>(vertex)];
                ++index){
                row.push_back(next_copy++);
            }
        }
        MicaliVaziraniMatching matching(auxiliary_count);
        for(std::size_t edge_id = 0; edge_id < edges.size(); ++edge_id){
            const int first_endpoint =
                copy_count + static_cast<int>(edge_id) * 2;
            const int second_endpoint = first_endpoint + 1;
            matching.add_edge(first_endpoint, second_endpoint);
            const auto [first, second] = edges[edge_id];
            for(int copy: copies[static_cast<std::size_t>(first)]){
                matching.add_edge(first_endpoint, copy);
            }
            for(int copy: copies[static_cast<std::size_t>(second)]){
                matching.add_edge(second_endpoint, copy);
            }
        }
        const MicaliVaziraniMatchingResult auxiliary = matching.solve();
        GeneralBMatchingResult result;
        result.size = 0;
        result.degree.assign(
            static_cast<std::size_t>(vertex_count), 0
        );
        for(std::size_t edge_id = 0; edge_id < edges.size(); ++edge_id){
            const int first_endpoint =
                copy_count + static_cast<int>(edge_id) * 2;
            const int second_endpoint = first_endpoint + 1;
            const int first_mate =
                auxiliary.mate[static_cast<std::size_t>(first_endpoint)];
            const int second_mate =
                auxiliary.mate[static_cast<std::size_t>(second_endpoint)];
            if(first_mate < 0 || copy_count <= first_mate
                || second_mate < 0 || copy_count <= second_mate){
                continue;
            }
            const auto [first, second] = edges[edge_id];
            result.selected_edge_ids.push_back(
                static_cast<int>(edge_id)
            );
            ++result.size;
            ++result.degree[static_cast<std::size_t>(first)];
            ++result.degree[static_cast<std::size_t>(second)];
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_GENERAL_B_MATCHING_HPP_INCLUDED
