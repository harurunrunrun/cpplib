#pragma once

#include <functional>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct KthShortestWalkEdge{
    int to;
    T cost;
};

namespace kth_shortest_walk_internal{

template<class T>
void validate(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target
){
    const int n = static_cast<int>(graph.size());
    if(source < 0 || n <= source || target < 0 || n <= target)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (kth_shortest_walk)."
        );
    }
    for(const auto& edges: graph){
        for(const auto& edge: edges){
            if(edge.to < 0 || n <= edge.to || edge.cost < T(0))[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (kth_shortest_walk)."
                );
            }
        }
    }
}

template<class T>
struct ShortestPathData{
    std::vector<std::optional<T>> distance;
    std::vector<int> parent_edge;
    std::vector<int> order;
};

template<class T>
ShortestPathData<T> shortest_path_tree_to_target(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int target
){
    struct ReverseEdge{
        int from;
        int index;
        T cost;
    };

    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<ReverseEdge>> reverse(static_cast<std::size_t>(n));
    for(int from = 0; from < n; from++){
        const auto& edges = graph[static_cast<std::size_t>(from)];
        for(int index = 0; index < static_cast<int>(edges.size()); index++){
            const auto& edge = edges[static_cast<std::size_t>(index)];
            reverse[static_cast<std::size_t>(edge.to)].push_back(
                {from, index, edge.cost}
            );
        }
    }

    ShortestPathData<T> result{
        std::vector<std::optional<T>>(static_cast<std::size_t>(n)),
        std::vector<int>(static_cast<std::size_t>(n), -1),
        {},
    };
    result.order.reserve(static_cast<std::size_t>(n));

    using State = std::pair<T, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    result.distance[static_cast<std::size_t>(target)] = T();
    queue.push({T(), target});
    while(!queue.empty()){
        auto [current_distance, vertex] = queue.top();
        queue.pop();
        const auto& best = result.distance[static_cast<std::size_t>(vertex)];
        if(*best < current_distance) continue;

        // A parent is selected only when a distance strictly improves. Thus a
        // zero-cost tie never changes the tree, and the parent is settled first.
        result.order.push_back(vertex);
        for(const auto& edge: reverse[static_cast<std::size_t>(vertex)]){
            T next_distance = current_distance + edge.cost;
            auto& next_best = result.distance[static_cast<std::size_t>(edge.from)];
            if(!next_best || next_distance < *next_best){
                next_best = next_distance;
                result.parent_edge[static_cast<std::size_t>(edge.from)] = edge.index;
                queue.push({next_distance, edge.from});
            }
        }
    }
    return result;
}

template<class T>
class PersistentLeftistHeap{
public:
    struct Node{
        T delta;
        int to;
        int left;
        int right;
        int rank;
    };

    int singleton(const T& delta, int to){
        nodes_.push_back({delta, to, -1, -1, 1});
        return static_cast<int>(nodes_.size()) - 1;
    }

    // Used only while constructing one vertex's local heap. Its nodes are
    // fresh and are not shared with another root yet.
    int meld_local(int first, int second){
        if(first == -1) return second;
        if(second == -1) return first;
        if(nodes_[static_cast<std::size_t>(second)].delta
            < nodes_[static_cast<std::size_t>(first)].delta){
            std::swap(first, second);
        }
        const int new_right = meld_local(
            nodes_[static_cast<std::size_t>(first)].right,
            second
        );
        nodes_[static_cast<std::size_t>(first)].right = new_right;
        normalize(first);
        return first;
    }

    // Both inputs may already be shared by roots of other vertices.
    int meld_persistent(int first, int second){
        if(first == -1) return second;
        if(second == -1) return first;
        if(nodes_[static_cast<std::size_t>(second)].delta
            < nodes_[static_cast<std::size_t>(first)].delta){
            std::swap(first, second);
        }
        const int copied = clone(first);
        const int new_right = meld_persistent(
            nodes_[static_cast<std::size_t>(copied)].right,
            second
        );
        nodes_[static_cast<std::size_t>(copied)].right = new_right;
        normalize(copied);
        return copied;
    }

    const Node& operator[](int index) const{
        return nodes_[static_cast<std::size_t>(index)];
    }

private:
    std::vector<Node> nodes_;

    int rank(int index) const{
        return index == -1 ? 0 : nodes_[static_cast<std::size_t>(index)].rank;
    }

    int clone(int index){
        nodes_.push_back(nodes_[static_cast<std::size_t>(index)]);
        return static_cast<int>(nodes_.size()) - 1;
    }

    void normalize(int index){
        auto& node = nodes_[static_cast<std::size_t>(index)];
        if(rank(node.left) < rank(node.right)) std::swap(node.left, node.right);
        node.rank = rank(node.right) + 1;
    }
};

template<class T>
std::optional<T> enumerate(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    std::size_t required,
    std::vector<T>* output
){
    validate(graph, source, target);
    if(required == 0) return std::nullopt;

    auto shortest = shortest_path_tree_to_target(graph, target);
    const auto& source_distance = shortest.distance[static_cast<std::size_t>(source)];
    if(!source_distance) return std::nullopt;

    if(output != nullptr) output->push_back(*source_distance);
    if(required == 1) return *source_distance;

    const int n = static_cast<int>(graph.size());
    PersistentLeftistHeap<T> heap;
    std::vector<int> roots(static_cast<std::size_t>(n), -1);

    // order starts at target and follows Dijkstra's settled order. Therefore
    // the heap inherited from the shortest-path-tree parent is already built.
    for(int vertex: shortest.order){
        int local_root = -1;
        const auto& edges = graph[static_cast<std::size_t>(vertex)];
        for(int index = 0; index < static_cast<int>(edges.size()); index++){
            const auto& edge = edges[static_cast<std::size_t>(index)];
            const auto& to_distance = shortest.distance[static_cast<std::size_t>(edge.to)];
            if(!to_distance) continue;
            if(index == shortest.parent_edge[static_cast<std::size_t>(vertex)]){
                continue;
            }
            const T delta = edge.cost + *to_distance
                - *shortest.distance[static_cast<std::size_t>(vertex)];
            local_root = heap.meld_local(
                local_root,
                heap.singleton(delta, edge.to)
            );
        }

        int inherited_root = -1;
        const int parent_index = shortest.parent_edge[static_cast<std::size_t>(vertex)];
        if(parent_index != -1){
            const int parent = edges[static_cast<std::size_t>(parent_index)].to;
            inherited_root = roots[static_cast<std::size_t>(parent)];
        }
        roots[static_cast<std::size_t>(vertex)] = heap.meld_persistent(
            local_root,
            inherited_root
        );
    }

    struct Candidate{
        T distance;
        T prefix_delta;
        int node;
    };
    struct GreaterCandidate{
        bool operator()(const Candidate& lhs, const Candidate& rhs) const{
            return rhs.distance < lhs.distance;
        }
    };
    std::priority_queue<
        Candidate,
        std::vector<Candidate>,
        GreaterCandidate
    > queue;

    const T base = *source_distance;
    const int source_root = roots[static_cast<std::size_t>(source)];
    if(source_root != -1){
        queue.push({base + heap[source_root].delta, T(), source_root});
    }

    std::size_t found = 1;
    while(!queue.empty()){
        const Candidate current = queue.top();
        queue.pop();
        const auto& node = heap[current.node];

        if(output != nullptr) output->push_back(current.distance);
        if(++found == required) return current.distance;

        if(node.left != -1){
            queue.push({
                base + current.prefix_delta + heap[node.left].delta,
                current.prefix_delta,
                node.left,
            });
        }
        if(node.right != -1){
            queue.push({
                base + current.prefix_delta + heap[node.right].delta,
                current.prefix_delta,
                node.right,
            });
        }

        const T next_prefix = current.prefix_delta + node.delta;
        const int next_root = roots[static_cast<std::size_t>(node.to)];
        if(next_root != -1){
            queue.push({
                base + next_prefix + heap[next_root].delta,
                next_prefix,
                next_root,
            });
        }
    }
    return std::nullopt;
}

} // namespace kth_shortest_walk_internal

template<class T>
std::vector<T> kth_shortest_walks(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    int k
){
    if(k < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (kth_shortest_walks).");
    }
    std::vector<T> result;
    result.reserve(static_cast<std::size_t>(k));
    (void)kth_shortest_walk_internal::enumerate(
        graph, source, target, static_cast<std::size_t>(k), &result
    );
    return result;
}

template<class T>
std::optional<T> kth_shortest_walk(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    int k
){
    if(k < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (kth_shortest_walk).");
    }
    return kth_shortest_walk_internal::enumerate<T>(
        graph, source, target, static_cast<std::size_t>(k) + 1, nullptr
    );
}

template<class T>
std::optional<T> kth_shortest_walk_1indexed(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    int k
){
    if(k <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (kth_shortest_walk_1indexed)."
        );
    }
    return kth_shortest_walk_internal::enumerate<T>(
        graph, source, target, static_cast<std::size_t>(k), nullptr
    );
}
