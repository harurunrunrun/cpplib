#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_JOHNSON_ALL_PAIRS_SHORTEST_PATHS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_JOHNSON_ALL_PAIRS_SHORTEST_PATHS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<class T>
struct JohnsonEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct JohnsonAllPairsShortestPathsResult{
    std::vector<std::vector<T>> dist;
    std::vector<std::vector<int>> parent;
    std::vector<std::vector<char>> reachable;

    std::vector<int> path(int from, int to) const{
        const int vertex_count = static_cast<int>(dist.size());
        if(from < 0 || vertex_count <= from || to < 0 || vertex_count <= to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(JohnsonAllPairsShortestPathsResult::path)."
            );
        }
        if(!reachable[static_cast<std::size_t>(from)]
                     [static_cast<std::size_t>(to)]){
            return {};
        }

        std::vector<int> result;
        int current = to;
        for(int step = 0;; ++step){
            if(step > vertex_count)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid parent forest "
                    "(JohnsonAllPairsShortestPathsResult::path)."
                );
            }
            result.push_back(current);
            if(current == from) break;
            current = parent[static_cast<std::size_t>(from)]
                            [static_cast<std::size_t>(current)];
            if(current < 0 || vertex_count <= current)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid parent forest "
                    "(JohnsonAllPairsShortestPathsResult::path)."
                );
            }
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
};

namespace johnson_all_pairs_shortest_paths_internal{

using Wide = __int128_t;

struct ReweightedEdge{
    int to;
    Wide cost;
};

class IndexedMinHeap{
private:
    const std::vector<Wide>& distance_;
    std::vector<int> heap_;
    std::vector<int> position_;

    bool less_vertex(int lhs, int rhs) const{
        const Wide lhs_distance = distance_[static_cast<std::size_t>(lhs)];
        const Wide rhs_distance = distance_[static_cast<std::size_t>(rhs)];
        return lhs_distance < rhs_distance ||
            (lhs_distance == rhs_distance && lhs < rhs);
    }

    void swap_positions(int lhs, int rhs){
        std::swap(
            heap_[static_cast<std::size_t>(lhs)],
            heap_[static_cast<std::size_t>(rhs)]
        );
        position_[static_cast<std::size_t>(
            heap_[static_cast<std::size_t>(lhs)]
        )] = lhs;
        position_[static_cast<std::size_t>(
            heap_[static_cast<std::size_t>(rhs)]
        )] = rhs;
    }

    void sift_up(int index){
        while(index > 0){
            const int parent = (index - 1) / 2;
            if(!less_vertex(
                heap_[static_cast<std::size_t>(index)],
                heap_[static_cast<std::size_t>(parent)]
            )) break;
            swap_positions(index, parent);
            index = parent;
        }
    }

    void sift_down(int index){
        const int size = static_cast<int>(heap_.size());
        while(true){
            const int left = 2 * index + 1;
            if(left >= size) break;
            int child = left;
            const int right = left + 1;
            if(right < size && less_vertex(
                heap_[static_cast<std::size_t>(right)],
                heap_[static_cast<std::size_t>(left)]
            )){
                child = right;
            }
            if(!less_vertex(
                heap_[static_cast<std::size_t>(child)],
                heap_[static_cast<std::size_t>(index)]
            )) break;
            swap_positions(index, child);
            index = child;
        }
    }

public:
    IndexedMinHeap(int vertex_count, const std::vector<Wide>& distance)
        : distance_(distance),
          position_(static_cast<std::size_t>(vertex_count), -1){}

    bool empty() const noexcept{ return heap_.empty(); }

    void push_or_decrease(int vertex){
        int& position = position_[static_cast<std::size_t>(vertex)];
        if(position == -1){
            position = static_cast<int>(heap_.size());
            heap_.push_back(vertex);
        }else if(position == -2){
            return;
        }
        sift_up(position);
    }

    int pop(){
        const int result = heap_.front();
        position_[static_cast<std::size_t>(result)] = -2;
        if(heap_.size() == 1U){
            heap_.pop_back();
            return result;
        }
        heap_.front() = heap_.back();
        heap_.pop_back();
        position_[static_cast<std::size_t>(heap_.front())] = 0;
        sift_down(0);
        return result;
    }
};

template<class T>
T narrow_distance(Wide value){
    const Wide lower = static_cast<Wide>(std::numeric_limits<T>::lowest());
    const Wide upper = static_cast<Wide>(std::numeric_limits<T>::max());
    if(value < lower || upper < value)[[unlikely]]{
        throw std::overflow_error(
            "shortest-path distance does not fit the result type "
            "(johnson_all_pairs_shortest_paths)."
        );
    }
    return static_cast<T>(value);
}

} // namespace johnson_all_pairs_shortest_paths_internal

template<class T>
JohnsonAllPairsShortestPathsResult<T> johnson_all_pairs_shortest_paths(
    int vertex_count,
    const std::vector<JohnsonEdge<T>>& edges,
    T inf = std::numeric_limits<T>::max() / T(4)
){
    using CleanT = std::remove_cv_t<T>;
    static_assert(
        std::numeric_limits<CleanT>::is_integer &&
        std::numeric_limits<CleanT>::is_signed &&
        !std::is_same_v<CleanT, bool> && sizeof(CleanT) <= 8U,
        "Johnson's algorithm requires a signed integral type of at most 64 bits"
    );
    using johnson_all_pairs_shortest_paths_internal::IndexedMinHeap;
    using johnson_all_pairs_shortest_paths_internal::ReweightedEdge;
    using johnson_all_pairs_shortest_paths_internal::Wide;

    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative vertex count "
            "(johnson_all_pairs_shortest_paths)."
        );
    }
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge range violation "
                "(johnson_all_pairs_shortest_paths)."
            );
        }
    }

    const std::size_t size = static_cast<std::size_t>(vertex_count);
    JohnsonAllPairsShortestPathsResult<T> result;
    result.dist.assign(size, std::vector<T>(size, inf));
    result.parent.assign(size, std::vector<int>(size, -1));
    result.reachable.assign(size, std::vector<char>(size, 0));
    if(vertex_count == 0) return result;

    // This is Bellman--Ford after adding a super-source with zero-cost edges
    // to every vertex.  Initializing every potential to zero performs the
    // first (super-source) relaxation implicitly.
    std::vector<Wide> potential(size, Wide(0));
    for(int iteration = 0; iteration < vertex_count; ++iteration){
        bool changed = false;
        for(const auto& edge: edges){
            const Wide candidate =
                potential[static_cast<std::size_t>(edge.from)] +
                static_cast<Wide>(edge.cost);
            Wide& target = potential[static_cast<std::size_t>(edge.to)];
            if(candidate < target){
                target = candidate;
                changed = true;
            }
        }
        if(!changed) break;
        if(iteration + 1 == vertex_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative cycle "
                "(johnson_all_pairs_shortest_paths)."
            );
        }
    }

    std::vector<std::vector<ReweightedEdge>> graph(size);
    for(const auto& edge: edges){
        const Wide cost = static_cast<Wide>(edge.cost) +
            potential[static_cast<std::size_t>(edge.from)] -
            potential[static_cast<std::size_t>(edge.to)];
        if(cost < Wide(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid reduced cost "
                "(johnson_all_pairs_shortest_paths)."
            );
        }
        graph[static_cast<std::size_t>(edge.from)].push_back({edge.to, cost});
    }

    for(int source = 0; source < vertex_count; ++source){
        std::vector<Wide> reweighted_distance(size, Wide(0));
        std::vector<char> reached(size, 0);
        IndexedMinHeap heap(vertex_count, reweighted_distance);
        reached[static_cast<std::size_t>(source)] = 1;
        heap.push_or_decrease(source);

        while(!heap.empty()){
            const int vertex = heap.pop();
            const Wide current =
                reweighted_distance[static_cast<std::size_t>(vertex)];
            for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
                const Wide candidate = current + edge.cost;
                Wide& target =
                    reweighted_distance[static_cast<std::size_t>(edge.to)];
                if(!reached[static_cast<std::size_t>(edge.to)] ||
                   candidate < target){
                    reached[static_cast<std::size_t>(edge.to)] = 1;
                    target = candidate;
                    result.parent[static_cast<std::size_t>(source)]
                                 [static_cast<std::size_t>(edge.to)] = vertex;
                    heap.push_or_decrease(edge.to);
                }
            }
        }

        for(int target = 0; target < vertex_count; ++target){
            if(!reached[static_cast<std::size_t>(target)]) continue;
            const Wide original_distance =
                reweighted_distance[static_cast<std::size_t>(target)] -
                potential[static_cast<std::size_t>(source)] +
                potential[static_cast<std::size_t>(target)];
            result.dist[static_cast<std::size_t>(source)]
                       [static_cast<std::size_t>(target)] =
                johnson_all_pairs_shortest_paths_internal::
                    narrow_distance<T>(original_distance);
            result.reachable[static_cast<std::size_t>(source)]
                            [static_cast<std::size_t>(target)] = 1;
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_JOHNSON_ALL_PAIRS_SHORTEST_PATHS_HPP_INCLUDED
