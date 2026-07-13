#pragma once

#include <algorithm>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

struct ChordalGraphRecognitionResult{
    bool chordal;
    std::vector<int> perfect_elimination_order;
    std::vector<int> induced_cycle;
};

inline ChordalGraphRecognitionResult chordal_graph_recognition(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    constexpr const char* message =
        "library assertion fault: range violation "
        "(chordal_graph_recognition).";
    if(vertex_count < 0)[[unlikely]] throw std::runtime_error(message);
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& [left, right]: edges){
        if(left < 0 || vertex_count <= left ||
           right < 0 || vertex_count <= right || left == right)[[unlikely]]{
            throw std::runtime_error(message);
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    for(auto& adjacent: graph){
        std::sort(adjacent.begin(), adjacent.end());
        adjacent.erase(
            std::unique(adjacent.begin(), adjacent.end()), adjacent.end()
        );
    }

    std::vector<int> weight(static_cast<std::size_t>(vertex_count));
    std::vector<int> index(static_cast<std::size_t>(vertex_count), -1);
    std::vector<unsigned char> selected(
        static_cast<std::size_t>(vertex_count)
    );
    std::set<std::pair<int, int>> queue;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        queue.insert({0, vertex});
    }
    std::vector<int> selection_order;
    selection_order.reserve(static_cast<std::size_t>(vertex_count));
    while(!queue.empty()){
        const auto iterator = std::prev(queue.end());
        const int vertex = iterator->second;
        queue.erase(iterator);
        index[static_cast<std::size_t>(vertex)] =
            static_cast<int>(selection_order.size());
        selection_order.push_back(vertex);
        selected[static_cast<std::size_t>(vertex)] = 1;
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(selected[static_cast<std::size_t>(to)]) continue;
            queue.erase({weight[static_cast<std::size_t>(to)], to});
            ++weight[static_cast<std::size_t>(to)];
            queue.insert({weight[static_cast<std::size_t>(to)], to});
        }
    }

    const auto adjacent = [&](int left, int right){
        const auto& list = graph[static_cast<std::size_t>(left)];
        return std::binary_search(list.begin(), list.end(), right);
    };

    for(int vertex: selection_order){
        int predecessor_index = -1;
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            const int to_index = index[static_cast<std::size_t>(to)];
            if(to_index < index[static_cast<std::size_t>(vertex)]){
                predecessor_index = std::max(predecessor_index, to_index);
            }
        }
        if(predecessor_index == -1) continue;
        const int predecessor = selection_order[
            static_cast<std::size_t>(predecessor_index)
        ];
        int conflicting = -1;
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(index[static_cast<std::size_t>(to)] < predecessor_index &&
               !adjacent(predecessor, to)){
                conflicting = to;
                break;
            }
        }
        if(conflicting == -1) continue;

        std::vector<int> parent(static_cast<std::size_t>(vertex_count), -1);
        std::queue<int> bfs;
        parent[static_cast<std::size_t>(conflicting)] = conflicting;
        bfs.push(conflicting);
        while(!bfs.empty() &&
              parent[static_cast<std::size_t>(predecessor)] == -1){
            const int current = bfs.front();
            bfs.pop();
            for(int to: graph[static_cast<std::size_t>(current)]){
                if(to == vertex ||
                   (to != predecessor && adjacent(vertex, to)) ||
                   parent[static_cast<std::size_t>(to)] != -1){
                    continue;
                }
                parent[static_cast<std::size_t>(to)] = current;
                bfs.push(to);
            }
        }
        if(parent[static_cast<std::size_t>(predecessor)] == -1){
            throw std::logic_error(
                "library internal fault: chordal certificate was not found."
            );
        }

        std::vector<int> cycle;
        for(int current = predecessor; current != conflicting;
            current = parent[static_cast<std::size_t>(current)]){
            cycle.push_back(current);
        }
        cycle.push_back(conflicting);
        cycle.push_back(vertex);
        return {false, {}, std::move(cycle)};
    }

    std::reverse(selection_order.begin(), selection_order.end());
    return {true, std::move(selection_order), {}};
}
