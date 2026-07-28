#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MPM_MAX_FLOW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MPM_MAX_FLOW_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<class Capacity>
struct MPMMaxFlowEdge{
    int from;
    int to;
    Capacity capacity;
    Capacity flow;
};

template<class Capacity>
struct MPMMaxFlow{
    static_assert(std::is_integral_v<Capacity>);
    static_assert(!std::is_same_v<std::remove_cv_t<Capacity>, bool>);
    static_assert(sizeof(Capacity) <= sizeof(long long));

    using Wide = __int128_t;

    int vertex_count;
    std::vector<MPMMaxFlowEdge<Capacity>> edges;
    std::vector<std::vector<int>> graph;

    explicit MPMMaxFlow(int vertex_count_)
        : vertex_count(vertex_count_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (MPMMaxFlow)."
            )
            : vertex_count_),
          graph(static_cast<std::size_t>(vertex_count)){}

    int add_edge(int from, int to, Capacity capacity){
        if(from < 0 || vertex_count <= from
            || to < 0 || vertex_count <= to
            || capacity < Capacity(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(MPMMaxFlow::add_edge)."
            );
        }
        const int id = static_cast<int>(edges.size());
        graph[static_cast<std::size_t>(from)].push_back(id);
        edges.push_back({from, to, capacity, Capacity(0)});
        graph[static_cast<std::size_t>(to)].push_back(id ^ 1);
        edges.push_back({to, from, Capacity(0), Capacity(0)});
        return id;
    }

private:
    Capacity residual(int edge_id) const{
        const auto& edge = edges[static_cast<std::size_t>(edge_id)];
        return edge.capacity - edge.flow;
    }

    void augment(int edge_id, Capacity amount){
        edges[static_cast<std::size_t>(edge_id)].flow += amount;
        edges[static_cast<std::size_t>(edge_id ^ 1)].flow -= amount;
    }

public:
    Capacity max_flow(
        int source,
        int sink,
        Capacity flow_limit =
            std::numeric_limits<Capacity>::max() / Capacity(4)
    ){
        if(source < 0 || vertex_count <= source
            || sink < 0 || vertex_count <= sink
            || flow_limit < Capacity(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(MPMMaxFlow::max_flow)."
            );
        }
        if(source == sink || flow_limit == Capacity(0)) return Capacity(0);
        Capacity total_flow = 0;
        std::vector<int> level(static_cast<std::size_t>(vertex_count));
        while(total_flow < flow_limit){
            std::fill(level.begin(), level.end(), -1);
            std::queue<int> bfs_queue;
            level[static_cast<std::size_t>(source)] = 0;
            bfs_queue.push(source);
            while(!bfs_queue.empty()){
                const int vertex = bfs_queue.front();
                bfs_queue.pop();
                for(int edge_id:
                    graph[static_cast<std::size_t>(vertex)]){
                    const auto& edge =
                        edges[static_cast<std::size_t>(edge_id)];
                    if(residual(edge_id) <= Capacity(0)
                        || level[static_cast<std::size_t>(edge.to)] != -1){
                        continue;
                    }
                    level[static_cast<std::size_t>(edge.to)] =
                        level[static_cast<std::size_t>(vertex)] + 1;
                    bfs_queue.push(edge.to);
                }
            }
            if(level[static_cast<std::size_t>(sink)] == -1) break;

            std::vector<unsigned char> alive(
                static_cast<std::size_t>(vertex_count), 0
            );
            std::vector<Wide> incoming(
                static_cast<std::size_t>(vertex_count), 0
            );
            std::vector<Wide> outgoing(
                static_cast<std::size_t>(vertex_count), 0
            );
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                alive[static_cast<std::size_t>(vertex)] =
                    level[static_cast<std::size_t>(vertex)] != -1
                    && level[static_cast<std::size_t>(vertex)]
                        <= level[static_cast<std::size_t>(sink)];
            }
            for(std::size_t edge_id = 0; edge_id < edges.size(); ++edge_id){
                const auto& edge = edges[edge_id];
                if(alive[static_cast<std::size_t>(edge.from)] == 0
                    || alive[static_cast<std::size_t>(edge.to)] == 0
                    || level[static_cast<std::size_t>(edge.to)]
                        != level[static_cast<std::size_t>(edge.from)] + 1){
                    continue;
                }
                const Capacity capacity =
                    residual(static_cast<int>(edge_id));
                if(capacity <= Capacity(0)) continue;
                outgoing[static_cast<std::size_t>(edge.from)] +=
                    static_cast<Wide>(capacity);
                incoming[static_cast<std::size_t>(edge.to)] +=
                    static_cast<Wide>(capacity);
            }
            const Wide remaining =
                static_cast<Wide>(flow_limit - total_flow);
            incoming[static_cast<std::size_t>(source)] = remaining;
            outgoing[static_cast<std::size_t>(sink)] = remaining;

            std::vector<std::size_t> forward_pointer(
                static_cast<std::size_t>(vertex_count), 0
            );
            std::vector<std::size_t> backward_pointer(
                static_cast<std::size_t>(vertex_count), 0
            );
            std::vector<Wide> excess(
                static_cast<std::size_t>(vertex_count), 0
            );

            auto send_forward = [&](int start, Capacity amount){
                std::queue<int> queue;
                excess[static_cast<std::size_t>(start)] += amount;
                queue.push(start);
                while(!queue.empty()){
                    const int vertex = queue.front();
                    queue.pop();
                    if(vertex == sink) continue;
                    auto& pointer =
                        forward_pointer[static_cast<std::size_t>(vertex)];
                    const auto& adjacent =
                        graph[static_cast<std::size_t>(vertex)];
                    while(excess[static_cast<std::size_t>(vertex)] > 0
                        && pointer < adjacent.size()){
                        const int edge_id = adjacent[pointer];
                        const auto& edge =
                            edges[static_cast<std::size_t>(edge_id)];
                        if(alive[static_cast<std::size_t>(edge.to)] == 0
                            || level[static_cast<std::size_t>(edge.to)]
                                != level[static_cast<std::size_t>(vertex)] + 1
                            || residual(edge_id) <= Capacity(0)){
                            ++pointer;
                            continue;
                        }
                        const Capacity pushed = static_cast<Capacity>(
                            std::min(
                                excess[static_cast<std::size_t>(vertex)],
                                static_cast<Wide>(residual(edge_id))
                            )
                        );
                        augment(edge_id, pushed);
                        excess[static_cast<std::size_t>(vertex)] -= pushed;
                        if(excess[static_cast<std::size_t>(edge.to)] == 0){
                            queue.push(edge.to);
                        }
                        excess[static_cast<std::size_t>(edge.to)] += pushed;
                        outgoing[static_cast<std::size_t>(vertex)] -= pushed;
                        incoming[static_cast<std::size_t>(edge.to)] -= pushed;
                        if(residual(edge_id) == Capacity(0)) ++pointer;
                    }
                }
            };

            auto send_backward = [&](int start, Capacity amount){
                std::queue<int> queue;
                excess[static_cast<std::size_t>(start)] += amount;
                queue.push(start);
                while(!queue.empty()){
                    const int vertex = queue.front();
                    queue.pop();
                    if(vertex == source) continue;
                    auto& pointer =
                        backward_pointer[static_cast<std::size_t>(vertex)];
                    const auto& adjacent =
                        graph[static_cast<std::size_t>(vertex)];
                    while(excess[static_cast<std::size_t>(vertex)] > 0
                        && pointer < adjacent.size()){
                        const int outgoing_id = adjacent[pointer];
                        const int incoming_id = outgoing_id ^ 1;
                        const auto& incoming_edge =
                            edges[static_cast<std::size_t>(incoming_id)];
                        const int previous = incoming_edge.from;
                        if(alive[static_cast<std::size_t>(previous)] == 0
                            || level[static_cast<std::size_t>(previous)] + 1
                                != level[static_cast<std::size_t>(vertex)]
                            || residual(incoming_id) <= Capacity(0)){
                            ++pointer;
                            continue;
                        }
                        const Capacity pushed = static_cast<Capacity>(
                            std::min(
                                excess[static_cast<std::size_t>(vertex)],
                                static_cast<Wide>(residual(incoming_id))
                            )
                        );
                        augment(incoming_id, pushed);
                        excess[static_cast<std::size_t>(vertex)] -= pushed;
                        if(excess[static_cast<std::size_t>(previous)] == 0){
                            queue.push(previous);
                        }
                        excess[static_cast<std::size_t>(previous)] += pushed;
                        incoming[static_cast<std::size_t>(vertex)] -= pushed;
                        outgoing[static_cast<std::size_t>(previous)] -= pushed;
                        if(residual(incoming_id) == Capacity(0)) ++pointer;
                    }
                }
            };
            auto remove_vertex = [&](int vertex){
                alive[static_cast<std::size_t>(vertex)] = 0;
                for(int outgoing_id:
                    graph[static_cast<std::size_t>(vertex)]){
                    const auto& edge =
                        edges[static_cast<std::size_t>(outgoing_id)];
                    if(alive[static_cast<std::size_t>(edge.to)] != 0
                        && level[static_cast<std::size_t>(edge.to)]
                            == level[static_cast<std::size_t>(vertex)] + 1){
                        incoming[static_cast<std::size_t>(edge.to)] -=
                            residual(outgoing_id);
                    }
                    const int incoming_id = outgoing_id ^ 1;
                    const int previous =
                        edges[static_cast<std::size_t>(incoming_id)].from;
                    if(alive[static_cast<std::size_t>(previous)] != 0
                        && level[static_cast<std::size_t>(previous)] + 1
                            == level[static_cast<std::size_t>(vertex)]){
                        outgoing[static_cast<std::size_t>(previous)] -=
                            residual(incoming_id);
                    }
                }
            };


            while(total_flow < flow_limit){
                int bottleneck_vertex = -1;
                Wide bottleneck = remaining;
                for(int vertex = 0; vertex < vertex_count; ++vertex){
                    if(alive[static_cast<std::size_t>(vertex)] == 0) continue;
                    const Wide potential = std::min(
                        incoming[static_cast<std::size_t>(vertex)],
                        outgoing[static_cast<std::size_t>(vertex)]
                    );
                    if(bottleneck_vertex == -1 || potential < bottleneck){
                        bottleneck_vertex = vertex;
                        bottleneck = potential;
                    }
                }
                if(bottleneck_vertex == -1) break;
                if(bottleneck <= 0){
                    remove_vertex(bottleneck_vertex);
                    continue;
                }
                bottleneck = std::min(
                    bottleneck,
                    static_cast<Wide>(flow_limit - total_flow)
                );
                const Capacity amount =
                    static_cast<Capacity>(bottleneck);
                std::fill(excess.begin(), excess.end(), Wide(0));
                send_forward(bottleneck_vertex, amount);
                std::fill(excess.begin(), excess.end(), Wide(0));
                send_backward(bottleneck_vertex, amount);
                total_flow += amount;

                remove_vertex(bottleneck_vertex);
            }
        }
        return total_flow;
    }

    std::vector<unsigned char> min_cut(int source) const{
        if(source < 0 || vertex_count <= source)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(MPMMaxFlow::min_cut)."
            );
        }
        std::vector<unsigned char> reached(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<int> stack{source};
        reached[static_cast<std::size_t>(source)] = 1;
        while(!stack.empty()){
            int vertex = stack.back();
            stack.pop_back();
            for(int edge_id: graph[static_cast<std::size_t>(vertex)]){
                int to = edges[static_cast<std::size_t>(edge_id)].to;
                if(residual(edge_id) <= Capacity(0)
                    || reached[static_cast<std::size_t>(to)] != 0) continue;
                reached[static_cast<std::size_t>(to)] = 1;
                stack.push_back(to);
            }
        }
        return reached;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MPM_MAX_FLOW_HPP_INCLUDED
