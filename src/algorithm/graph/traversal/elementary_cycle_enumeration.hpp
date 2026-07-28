#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_ELEMENTARY_CYCLE_ENUMERATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_ELEMENTARY_CYCLE_ENUMERATION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

inline std::vector<std::vector<int>> enumerate_elementary_cycles(
    std::vector<std::vector<int>> graph
){
    const int vertex_count = static_cast<int>(graph.size());
    std::vector<std::vector<int>> reverse_graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int from = 0; from < vertex_count; ++from){
        auto& next = graph[static_cast<std::size_t>(from)];
        for(int to: next){
            if(to < 0 || vertex_count <= to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation "
                    "(enumerate_elementary_cycles)."
                );
            }
        }
        std::sort(next.begin(), next.end());
        next.erase(std::unique(next.begin(), next.end()), next.end());
        for(int to: next){
            reverse_graph[static_cast<std::size_t>(to)].push_back(from);
        }
    }
    std::vector<std::vector<int>> result;
    int lower_bound = 0;
    while(lower_bound < vertex_count){
        std::vector<unsigned char> seen(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<int> finish_order;
        for(int start = lower_bound; start < vertex_count; ++start){
            if(seen[static_cast<std::size_t>(start)] != 0) continue;
            seen[static_cast<std::size_t>(start)] = 1;
            std::vector<std::pair<int, std::size_t>> stack{{start, 0}};
            while(!stack.empty()){
                int vertex = stack.back().first;
                std::size_t& cursor = stack.back().second;
                const auto& next = graph[static_cast<std::size_t>(vertex)];
                while(cursor < next.size()
                    && next[cursor] < lower_bound){
                    ++cursor;
                }
                if(cursor == next.size()){
                    finish_order.push_back(vertex);
                    stack.pop_back();
                    continue;
                }
                int to = next[cursor++];
                if(seen[static_cast<std::size_t>(to)] == 0){
                    seen[static_cast<std::size_t>(to)] = 1;
                    stack.emplace_back(to, 0);
                }
            }
        }
        std::vector<int> component(
            static_cast<std::size_t>(vertex_count), -1
        );
        int component_count = 0;
        for(std::size_t index = finish_order.size(); index-- > 0; ){
            int start = finish_order[index];
            if(component[static_cast<std::size_t>(start)] != -1) continue;
            component[static_cast<std::size_t>(start)] = component_count;
            std::vector<int> stack{start};
            while(!stack.empty()){
                int vertex = stack.back();
                stack.pop_back();
                for(int to: reverse_graph[static_cast<std::size_t>(vertex)]){
                    if(to < lower_bound
                        || component[static_cast<std::size_t>(to)] != -1){
                        continue;
                    }
                    component[static_cast<std::size_t>(to)] = component_count;
                    stack.push_back(to);
                }
            }
            ++component_count;
        }
        std::vector<int> component_size(
            static_cast<std::size_t>(component_count), 0
        );
        std::vector<int> component_minimum(
            static_cast<std::size_t>(component_count), vertex_count
        );
        std::vector<unsigned char> cyclic(
            static_cast<std::size_t>(component_count), 0
        );
        for(int vertex = lower_bound; vertex < vertex_count; ++vertex){
            int id = component[static_cast<std::size_t>(vertex)];
            ++component_size[static_cast<std::size_t>(id)];
            component_minimum[static_cast<std::size_t>(id)] = std::min(
                component_minimum[static_cast<std::size_t>(id)], vertex
            );
            if(std::binary_search(
                graph[static_cast<std::size_t>(vertex)].begin(),
                graph[static_cast<std::size_t>(vertex)].end(),
                vertex
            )){
                cyclic[static_cast<std::size_t>(id)] = 1;
            }
        }
        int selected_component = -1;
        int start = vertex_count;
        for(int id = 0; id < component_count; ++id){
            if(component_size[static_cast<std::size_t>(id)] > 1){
                cyclic[static_cast<std::size_t>(id)] = 1;
            }
            if(cyclic[static_cast<std::size_t>(id)] != 0
                && component_minimum[static_cast<std::size_t>(id)] < start){
                selected_component = id;
                start = component_minimum[static_cast<std::size_t>(id)];
            }
        }
        if(selected_component == -1) break;

        std::vector<unsigned char> blocked(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<std::unordered_set<int>> dependency(
            static_cast<std::size_t>(vertex_count)
        );
        std::vector<int> path;
        std::function<void(int)> unblock = [&](int vertex) -> void {
            blocked[static_cast<std::size_t>(vertex)] = 0;
            std::unordered_set<int> waiting =
                std::move(dependency[static_cast<std::size_t>(vertex)]);
            dependency[static_cast<std::size_t>(vertex)].clear();
            for(int to: waiting){
                if(blocked[static_cast<std::size_t>(to)] != 0) unblock(to);
            }
        };
        std::function<bool(int)> circuit = [&](int vertex) -> bool {
            bool found = false;
            path.push_back(vertex);
            blocked[static_cast<std::size_t>(vertex)] = 1;
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(component[static_cast<std::size_t>(to)]
                    != selected_component){
                    continue;
                }
                if(to == start){
                    result.push_back(path);
                    found = true;
                }else if(blocked[static_cast<std::size_t>(to)] == 0
                    && circuit(to)){
                    found = true;
                }
            }
            if(found){
                unblock(vertex);
            }else{
                for(int to: graph[static_cast<std::size_t>(vertex)]){
                    if(component[static_cast<std::size_t>(to)]
                        != selected_component){
                        continue;
                    }
                    dependency[static_cast<std::size_t>(to)].insert(vertex);
                }
            }
            path.pop_back();
            return found;
        };
        circuit(start);
        lower_bound = start + 1;
    }
    std::sort(result.begin(), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_ELEMENTARY_CYCLE_ENUMERATION_HPP_INCLUDED
