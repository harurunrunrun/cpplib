#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_RECOGNITION_CHORDAL_GRAPH_RECOGNITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_RECOGNITION_CHORDAL_GRAPH_RECOGNITION_HPP_INCLUDED

#include <algorithm>
#include <queue>
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
    // Indexed binary max-heap of the unselected vertices keyed by
    // (weight, vertex). Keys are distinct, so the top is the same vertex an
    // ordered set of these pairs would give; a weight increment is a sift-up.
    std::vector<int> heap(static_cast<std::size_t>(vertex_count));
    std::vector<int> position(static_cast<std::size_t>(vertex_count));
    const auto higher = [&](int left, int right){
        const int left_weight = weight[static_cast<std::size_t>(left)];
        const int right_weight = weight[static_cast<std::size_t>(right)];
        return left_weight != right_weight
            ? left_weight > right_weight
            : left > right;
    };
    const auto place = [&](int slot, int vertex){
        heap[static_cast<std::size_t>(slot)] = vertex;
        position[static_cast<std::size_t>(vertex)] = slot;
    };
    const auto sift_up = [&](int slot){
        const int vertex = heap[static_cast<std::size_t>(slot)];
        while(slot > 0){
            const int parent_slot = (slot - 1) / 2;
            const int parent = heap[static_cast<std::size_t>(parent_slot)];
            if(!higher(vertex, parent)) break;
            place(slot, parent);
            slot = parent_slot;
        }
        place(slot, vertex);
    };
    int heap_size = vertex_count;
    const auto sift_down = [&](int slot){
        const int vertex = heap[static_cast<std::size_t>(slot)];
        while(true){
            int child_slot = 2 * slot + 1;
            if(child_slot >= heap_size) break;
            if(child_slot + 1 < heap_size &&
               higher(
                   heap[static_cast<std::size_t>(child_slot + 1)],
                   heap[static_cast<std::size_t>(child_slot)]
               )){
                ++child_slot;
            }
            const int child = heap[static_cast<std::size_t>(child_slot)];
            if(!higher(child, vertex)) break;
            place(slot, child);
            slot = child_slot;
        }
        place(slot, vertex);
    };
    // All weights start at 0, so the vertices in decreasing order form a heap.
    for(int slot = 0; slot < vertex_count; ++slot){
        place(slot, vertex_count - 1 - slot);
    }
    std::vector<int> selection_order;
    selection_order.reserve(static_cast<std::size_t>(vertex_count));
    while(heap_size > 0){
        const int vertex = heap[0];
        --heap_size;
        if(heap_size > 0){
            place(0, heap[static_cast<std::size_t>(heap_size)]);
            sift_down(0);
        }
        index[static_cast<std::size_t>(vertex)] =
            static_cast<int>(selection_order.size());
        selection_order.push_back(vertex);
        selected[static_cast<std::size_t>(vertex)] = 1;
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(selected[static_cast<std::size_t>(to)]) continue;
            ++weight[static_cast<std::size_t>(to)];
            sift_up(position[static_cast<std::size_t>(to)]);
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

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_RECOGNITION_CHORDAL_GRAPH_RECOGNITION_HPP_INCLUDED
