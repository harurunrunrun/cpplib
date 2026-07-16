#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "dsu.hpp"

template<int MAX_SIZE>
struct OfflineTreeEdgeDeletionDisconnectedPairs{
    static_assert(MAX_SIZE > 0);

private:
    enum class OperationType{
        delete_edge,
        query,
    };
    struct Operation{
        OperationType type;
        int edge;
    };

    int vertex_count;
    std::vector<std::pair<int, int>> edges;
    std::vector<Operation> operations;
    std::vector<bool> deleted;

    int expected_edge_count() const{
        return vertex_count == 0 ? 0 : vertex_count - 1;
    }

public:
    explicit OfflineTreeEdgeDeletionDisconnectedPairs(int n):
        vertex_count(n),
        deleted(static_cast<std::size_t>(n <= 1 ? 0 : n - 1), false){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        edges.reserve(static_cast<std::size_t>(expected_edge_count()));
    }

    int add_edge(int left, int right){
        if(static_cast<int>(edges.size()) >= expected_edge_count())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: too many edges (add_edge)."
            );
        }
        if(left < 0 || vertex_count <= left ||
           right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (add_edge)."
            );
        }
        edges.emplace_back(left, right);
        return static_cast<int>(edges.size()) - 1;
    }

    void delete_edge(int edge){
        if(edge < 0 || static_cast<int>(edges.size()) <= edge)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (delete_edge)."
            );
        }
        if(deleted[static_cast<std::size_t>(edge)])[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge deleted twice (delete_edge)."
            );
        }
        deleted[static_cast<std::size_t>(edge)] = true;
        operations.push_back({OperationType::delete_edge, edge});
    }

    void add_query(){
        operations.push_back({OperationType::query, -1});
    }

    std::vector<long long> solve() const{
        if(static_cast<int>(edges.size()) != expected_edge_count())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge count mismatch (solve)."
            );
        }

        DSU<MAX_SIZE> validator(vertex_count);
        for(const auto& [left, right]: edges){
            if(!validator.merge(left, right))[[unlikely]]{
                throw std::invalid_argument(
                    "OfflineTreeEdgeDeletionDisconnectedPairs requires a tree"
                );
            }
        }

        DSU<MAX_SIZE> dsu(vertex_count);
        long long disconnected =
            static_cast<long long>(vertex_count) * (vertex_count - 1LL) / 2LL;
        const auto restore = [&](int edge){
            const auto [left, right] = edges[static_cast<std::size_t>(edge)];
            disconnected -= static_cast<long long>(dsu.component_size(left)) *
                dsu.component_size(right);
            dsu.merge(left, right);
        };
        for(int edge = 0; edge < static_cast<int>(edges.size()); ++edge){
            if(!deleted[static_cast<std::size_t>(edge)]) restore(edge);
        }

        std::vector<long long> answers;
        for(auto iterator = operations.rbegin(); iterator != operations.rend();
            ++iterator){
            if(iterator->type == OperationType::query){
                answers.push_back(disconnected);
            }else{
                restore(iterator->edge);
            }
        }
        std::reverse(answers.begin(), answers.end());
        return answers;
    }
};
