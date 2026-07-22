#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_INCREMENTAL_SCC_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_INCREMENTAL_SCC_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace incremental_scc_internal{

struct Edge{
    int index;
    int from;
    int to;
};

inline std::vector<int> strongly_connected_components(
    int vertex_count,
    const std::vector<Edge>& edges,
    int prefix_length
){
    std::vector<int> offset(static_cast<std::size_t>(vertex_count + 1));
    std::vector<int> reverse_offset(
        static_cast<std::size_t>(vertex_count + 1)
    );
    int edge_count = 0;
    for(const Edge& edge: edges){
        if(edge.index >= prefix_length) continue;
        ++offset[static_cast<std::size_t>(edge.from + 1)];
        ++reverse_offset[static_cast<std::size_t>(edge.to + 1)];
        ++edge_count;
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        offset[static_cast<std::size_t>(vertex + 1)] +=
            offset[static_cast<std::size_t>(vertex)];
        reverse_offset[static_cast<std::size_t>(vertex + 1)] +=
            reverse_offset[static_cast<std::size_t>(vertex)];
    }

    std::vector<int> graph(static_cast<std::size_t>(edge_count));
    std::vector<int> reverse_graph(static_cast<std::size_t>(edge_count));
    std::vector<int> cursor = offset;
    for(const Edge& edge: edges){
        if(edge.index >= prefix_length) continue;
        graph[static_cast<std::size_t>(
            cursor[static_cast<std::size_t>(edge.from)]++
        )] = edge.to;
    }
    cursor = reverse_offset;
    for(const Edge& edge: edges){
        if(edge.index >= prefix_length) continue;
        reverse_graph[static_cast<std::size_t>(
            cursor[static_cast<std::size_t>(edge.to)]++
        )] = edge.from;
    }
    std::vector<int>().swap(cursor);

    struct Frame{
        int vertex;
        int next_edge;
    };
    std::vector<unsigned char> visited(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<int> finish_order;
    finish_order.reserve(static_cast<std::size_t>(vertex_count));
    std::vector<Frame> dfs_stack;
    for(int start = 0; start < vertex_count; ++start){
        if(visited[static_cast<std::size_t>(start)]) continue;
        visited[static_cast<std::size_t>(start)] = 1;
        dfs_stack.push_back({
            start, offset[static_cast<std::size_t>(start)]
        });
        while(!dfs_stack.empty()){
            Frame& frame = dfs_stack.back();
            const int end = offset[
                static_cast<std::size_t>(frame.vertex + 1)
            ];
            if(frame.next_edge == end){
                finish_order.push_back(frame.vertex);
                dfs_stack.pop_back();
                continue;
            }
            const int to = graph[static_cast<std::size_t>(frame.next_edge++)];
            if(visited[static_cast<std::size_t>(to)]) continue;
            visited[static_cast<std::size_t>(to)] = 1;
            dfs_stack.push_back({
                to, offset[static_cast<std::size_t>(to)]
            });
        }
    }

    std::vector<int> component(
        static_cast<std::size_t>(vertex_count), -1
    );
    std::vector<int> stack;
    int component_count = 0;
    for(auto iterator = finish_order.rbegin();
        iterator != finish_order.rend(); ++iterator){
        const int start = *iterator;
        if(component[static_cast<std::size_t>(start)] != -1) continue;
        component[static_cast<std::size_t>(start)] = component_count;
        stack.push_back(start);
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            for(int position = reverse_offset[static_cast<std::size_t>(vertex)];
                position < reverse_offset[
                    static_cast<std::size_t>(vertex + 1)
                ]; ++position){
                const int to = reverse_graph[static_cast<std::size_t>(position)];
                if(component[static_cast<std::size_t>(to)] != -1) continue;
                component[static_cast<std::size_t>(to)] = component_count;
                stack.push_back(to);
            }
        }
        ++component_count;
    }
    return component;
}

} // namespace incremental_scc_internal

inline std::vector<int> incremental_scc(
    int vertex_count,
    const std::vector<std::pair<int, int>>& added_edges
){
    constexpr const char* message =
        "library assertion fault: range violation (incremental_scc).";
    if(vertex_count < 0)[[unlikely]] throw std::runtime_error(message);
    if(vertex_count == std::numeric_limits<int>::max())[[unlikely]]{
        throw std::length_error("incremental_scc has too many vertices");
    }
    if(added_edges.size() >=
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error("incremental_scc has too many edges");
    }
    std::vector<incremental_scc_internal::Edge> records;
    const int edge_count = static_cast<int>(added_edges.size());
    records.reserve(added_edges.size());
    for(int index = 0; index < edge_count; ++index){
        const auto [from, to] = added_edges[static_cast<std::size_t>(index)];
        if(from < 0 || vertex_count <= from ||
           to < 0 || vertex_count <= to)[[unlikely]]{
            throw std::runtime_error(message);
        }
        records.push_back({index, from, to});
    }

    std::vector<int> merge_time(
        static_cast<std::size_t>(edge_count), edge_count + 1
    );
    std::vector<int> new_index(static_cast<std::size_t>(vertex_count), -1);
    const auto divide = [&](auto&& self,
        std::vector<incremental_scc_internal::Edge> data,
        int left_time,
        int right_time
    ) -> void{
        if(data.empty() || right_time == left_time + 1) return;
        const int middle_time = (left_time + right_time) / 2;

        int local_vertex_count = 0;
        std::vector<int> touched;
        touched.reserve(std::min(
            static_cast<std::size_t>(vertex_count), 2 * data.size()
        ));
        for(auto& edge: data){
            const int old_from = edge.from;
            const int old_to = edge.to;
            if(new_index[static_cast<std::size_t>(old_from)] == -1){
                new_index[static_cast<std::size_t>(old_from)] =
                    local_vertex_count++;
                touched.push_back(old_from);
            }
            if(new_index[static_cast<std::size_t>(old_to)] == -1){
                new_index[static_cast<std::size_t>(old_to)] =
                    local_vertex_count++;
                touched.push_back(old_to);
            }
            edge.from = new_index[static_cast<std::size_t>(old_from)];
            edge.to = new_index[static_cast<std::size_t>(old_to)];
        }
        for(int vertex: touched){
            new_index[static_cast<std::size_t>(vertex)] = -1;
        }
        std::vector<int>().swap(touched);

        std::vector<int> component =
            incremental_scc_internal::strongly_connected_components(
                local_vertex_count, data, middle_time
            );
        std::size_t left_size = 0;
        for(const auto& edge: data){
            if(edge.index < middle_time &&
               component[static_cast<std::size_t>(edge.from)] ==
               component[static_cast<std::size_t>(edge.to)]){
                ++left_size;
            }
        }
        std::vector<incremental_scc_internal::Edge> left;
        std::vector<incremental_scc_internal::Edge> right;
        left.reserve(left_size);
        right.reserve(data.size() - left_size);
        for(const auto& edge: data){
            const int from_component = component[
                static_cast<std::size_t>(edge.from)
            ];
            const int to_component = component[
                static_cast<std::size_t>(edge.to)
            ];
            if(edge.index < middle_time &&
               from_component == to_component){
                merge_time[static_cast<std::size_t>(edge.index)] =
                    std::min(
                        merge_time[static_cast<std::size_t>(edge.index)],
                        middle_time
                    );
                left.push_back(edge);
            }else{
                right.push_back({
                    edge.index, from_component, to_component
                });
            }
        }
        std::vector<int>().swap(component);
        std::vector<incremental_scc_internal::Edge>().swap(data);
        self(self, std::move(left), left_time, middle_time);
        self(self, std::move(right), middle_time, right_time);
    };
    divide(divide, std::move(records), 0, edge_count + 1);
    return merge_time;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_INCREMENTAL_SCC_HPP_INCLUDED
