#pragma once

#include <algorithm>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct UnicyclicMatchingEdge{
    int from;
    int to;
};

struct UnicyclicMatchingResult{
    int size;
    std::vector<int> match;
};

namespace unicyclic_matching_internal{

struct PathResult{
    int size = -1;
    std::vector<int> selected_edges;
};

class Solver{
    struct Edge{
        int from;
        int to;
    };

    int n;
    std::vector<Edge> edges;
    std::vector<std::vector<std::pair<int, int>>> graph;
    std::vector<int> match;
    std::vector<int> parent;
    std::vector<int> parent_edge;
    std::vector<std::vector<int>> children;
    std::vector<int> dp0;
    std::vector<int> dp1;
    std::vector<int> choice_child;
    std::vector<int> best_state;
    std::vector<char> visited;
    std::vector<char> used_edge;
    std::vector<char> cycle_edge;
    std::vector<char> removed;
    std::vector<int> degree;

    int best_tree_size(int v) const{
        return std::max(dp0[static_cast<std::size_t>(v)], dp1[static_cast<std::size_t>(v)]);
    }

    void collect_component(int root, std::vector<int>& vertices, std::vector<int>& component_edges){
        std::vector<int> stack = {root};
        visited[static_cast<std::size_t>(root)] = 1;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            vertices.push_back(v);
            for(auto [to, edge_id]: graph[static_cast<std::size_t>(v)]){
                if(!used_edge[static_cast<std::size_t>(edge_id)]){
                    used_edge[static_cast<std::size_t>(edge_id)] = 1;
                    component_edges.push_back(edge_id);
                }
                if(!visited[static_cast<std::size_t>(to)]){
                    visited[static_cast<std::size_t>(to)] = 1;
                    stack.push_back(to);
                }
            }
        }
    }

    std::vector<int> build_forest(const std::vector<int>& roots, const std::vector<int>& vertices){
        for(int v: vertices){
            children[static_cast<std::size_t>(v)].clear();
            parent[static_cast<std::size_t>(v)] = -2;
            parent_edge[static_cast<std::size_t>(v)] = -1;
        }
        std::vector<int> order;
        std::vector<int> stack;
        for(int root: roots){
            parent[static_cast<std::size_t>(root)] = -1;
            stack.push_back(root);
            while(!stack.empty()){
                int v = stack.back();
                stack.pop_back();
                order.push_back(v);
                for(auto [to, edge_id]: graph[static_cast<std::size_t>(v)]){
                    if(cycle_edge[static_cast<std::size_t>(edge_id)]) continue;
                    if(parent[static_cast<std::size_t>(to)] != -2) continue;
                    parent[static_cast<std::size_t>(to)] = v;
                    parent_edge[static_cast<std::size_t>(to)] = edge_id;
                    children[static_cast<std::size_t>(v)].push_back(to);
                    stack.push_back(to);
                }
            }
        }
        return order;
    }

    void compute_tree_dp(const std::vector<int>& order){
        for(auto it = order.rbegin(); it != order.rend(); ++it){
            int v = *it;
            int base = 0;
            for(int to: children[static_cast<std::size_t>(v)]){
                base += best_tree_size(to);
            }
            dp0[static_cast<std::size_t>(v)] = base;
            dp1[static_cast<std::size_t>(v)] = -1;
            choice_child[static_cast<std::size_t>(v)] = -1;
            for(int to: children[static_cast<std::size_t>(v)]){
                int candidate = base - best_tree_size(to) + dp0[static_cast<std::size_t>(to)] + 1;
                if(candidate > dp1[static_cast<std::size_t>(v)]){
                    dp1[static_cast<std::size_t>(v)] = candidate;
                    choice_child[static_cast<std::size_t>(v)] = to;
                }
            }
            best_state[static_cast<std::size_t>(v)] =
                dp1[static_cast<std::size_t>(v)] > dp0[static_cast<std::size_t>(v)] ? 1 : 0;
        }
    }

    void restore_tree(int root, int state){
        std::vector<std::pair<int, int>> stack = {{root, state}};
        while(!stack.empty()){
            auto [v, current_state] = stack.back();
            stack.pop_back();
            int chosen = current_state == 1 ? choice_child[static_cast<std::size_t>(v)] : -1;
            for(auto it = children[static_cast<std::size_t>(v)].rbegin();
                it != children[static_cast<std::size_t>(v)].rend();
                ++it){
                int to = *it;
                if(to == chosen){
                    match[static_cast<std::size_t>(v)] = to;
                    match[static_cast<std::size_t>(to)] = v;
                    stack.push_back({to, 0});
                }else{
                    stack.push_back({to, best_state[static_cast<std::size_t>(to)]});
                }
            }
        }
    }

    PathResult solve_path(int left, int right, const std::vector<int>& free_size, const std::vector<int>& best_size) const{
        int length = right - left;
        std::vector<int> dp(static_cast<std::size_t>(length + 1), -1);
        std::vector<int> previous(static_cast<std::size_t>(length + 1), -1);
        std::vector<int> action(static_cast<std::size_t>(length + 1), -2);
        dp[0] = 0;
        for(int i = 0; i < length; i++){
            if(dp[static_cast<std::size_t>(i)] < 0) continue;
            int vertex = left + i;
            int leave = dp[static_cast<std::size_t>(i)] + best_size[static_cast<std::size_t>(vertex)];
            if(leave > dp[static_cast<std::size_t>(i + 1)]){
                dp[static_cast<std::size_t>(i + 1)] = leave;
                previous[static_cast<std::size_t>(i + 1)] = i;
                action[static_cast<std::size_t>(i + 1)] = -1;
            }
            if(i + 1 < length){
                int take = dp[static_cast<std::size_t>(i)] +
                           free_size[static_cast<std::size_t>(vertex)] +
                           free_size[static_cast<std::size_t>(vertex + 1)] + 1;
                if(take > dp[static_cast<std::size_t>(i + 2)]){
                    dp[static_cast<std::size_t>(i + 2)] = take;
                    previous[static_cast<std::size_t>(i + 2)] = i;
                    action[static_cast<std::size_t>(i + 2)] = vertex;
                }
            }
        }
        PathResult result;
        result.size = dp[static_cast<std::size_t>(length)];
        for(int current = length; current > 0; current = previous[static_cast<std::size_t>(current)]){
            int chosen = action[static_cast<std::size_t>(current)];
            if(chosen >= 0) result.selected_edges.push_back(chosen);
        }
        return result;
    }

    PathResult solve_cycle(const std::vector<int>& free_size, const std::vector<int>& best_size) const{
        int k = static_cast<int>(free_size.size());
        PathResult best = solve_path(0, k, free_size, best_size);
        PathResult use_last = solve_path(1, k - 1, free_size, best_size);
        use_last.size += free_size[static_cast<std::size_t>(k - 1)] + free_size[0] + 1;
        use_last.selected_edges.push_back(k - 1);
        if(use_last.size > best.size) return use_last;
        return best;
    }

    int solve_tree_component(const std::vector<int>& vertices){
        std::vector<int> order = build_forest({vertices[0]}, vertices);
        compute_tree_dp(order);
        int state = best_state[static_cast<std::size_t>(vertices[0])];
        restore_tree(vertices[0], state);
        return best_tree_size(vertices[0]);
    }

    int solve_unicyclic_component(const std::vector<int>& vertices, const std::vector<int>& component_edges){
        for(int v: vertices){
            degree[static_cast<std::size_t>(v)] = 0;
            removed[static_cast<std::size_t>(v)] = 0;
        }
        for(int edge_id: component_edges){
            const Edge& edge = edges[static_cast<std::size_t>(edge_id)];
            degree[static_cast<std::size_t>(edge.from)]++;
            degree[static_cast<std::size_t>(edge.to)]++;
        }
        std::queue<int> que;
        for(int v: vertices){
            if(degree[static_cast<std::size_t>(v)] <= 1) que.push(v);
        }
        while(!que.empty()){
            int v = que.front();
            que.pop();
            if(removed[static_cast<std::size_t>(v)]) continue;
            removed[static_cast<std::size_t>(v)] = 1;
            for(auto [to, edge_id]: graph[static_cast<std::size_t>(v)]){
                (void)edge_id;
                if(removed[static_cast<std::size_t>(to)]) continue;
                degree[static_cast<std::size_t>(to)]--;
                if(degree[static_cast<std::size_t>(to)] == 1) que.push(to);
            }
        }

        std::vector<int> cycle_vertices;
        for(int v: vertices){
            if(!removed[static_cast<std::size_t>(v)]) cycle_vertices.push_back(v);
        }
        std::vector<int> cycle_edges;
        for(int edge_id: component_edges){
            const Edge& edge = edges[static_cast<std::size_t>(edge_id)];
            if(!removed[static_cast<std::size_t>(edge.from)] && !removed[static_cast<std::size_t>(edge.to)]){
                cycle_edge[static_cast<std::size_t>(edge_id)] = 1;
                cycle_edges.push_back(edge_id);
            }
        }
        int k = static_cast<int>(cycle_vertices.size());
        if(k < 2 || static_cast<int>(cycle_edges.size()) != k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not unicyclic (unicyclic_matching).");
        }

        std::vector<int> cycle;
        if(k == 2){
            cycle = cycle_vertices;
        }else{
            int start = cycle_vertices[0];
            int current = start;
            int previous_edge = -1;
            do{
                cycle.push_back(current);
                int chosen_edge = -1;
                for(auto [to, edge_id]: graph[static_cast<std::size_t>(current)]){
                    (void)to;
                    if(!cycle_edge[static_cast<std::size_t>(edge_id)] || edge_id == previous_edge) continue;
                    chosen_edge = edge_id;
                    break;
                }
                if(chosen_edge == -1)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: graph is not unicyclic (unicyclic_matching).");
                }
                const Edge& edge = edges[static_cast<std::size_t>(chosen_edge)];
                int next = edge.from ^ edge.to ^ current;
                previous_edge = chosen_edge;
                current = next;
            }while(current != start);
            if(static_cast<int>(cycle.size()) != k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph is not unicyclic (unicyclic_matching).");
            }
        }

        std::vector<int> order = build_forest(cycle, vertices);
        compute_tree_dp(order);

        std::vector<int> free_size(static_cast<std::size_t>(k));
        std::vector<int> best_size(static_cast<std::size_t>(k));
        for(int i = 0; i < k; i++){
            int v = cycle[static_cast<std::size_t>(i)];
            free_size[static_cast<std::size_t>(i)] = dp0[static_cast<std::size_t>(v)];
            best_size[static_cast<std::size_t>(i)] = best_tree_size(v);
        }
        PathResult cycle_result = solve_cycle(free_size, best_size);
        std::vector<char> matched_on_cycle(static_cast<std::size_t>(k), 0);
        for(int edge_index: cycle_result.selected_edges){
            int left = edge_index;
            int right = (edge_index + 1) % k;
            int u = cycle[static_cast<std::size_t>(left)];
            int v = cycle[static_cast<std::size_t>(right)];
            match[static_cast<std::size_t>(u)] = v;
            match[static_cast<std::size_t>(v)] = u;
            matched_on_cycle[static_cast<std::size_t>(left)] = 1;
            matched_on_cycle[static_cast<std::size_t>(right)] = 1;
        }
        for(int i = 0; i < k; i++){
            int v = cycle[static_cast<std::size_t>(i)];
            int state = matched_on_cycle[static_cast<std::size_t>(i)] ? 0 : best_state[static_cast<std::size_t>(v)];
            restore_tree(v, state);
        }
        return cycle_result.size;
    }

public:
    Solver(int n_, const std::vector<UnicyclicMatchingEdge>& input_edges)
        : n(n_),
          graph(static_cast<std::size_t>(n_)),
          match(static_cast<std::size_t>(n_), -1),
          parent(static_cast<std::size_t>(n_)),
          parent_edge(static_cast<std::size_t>(n_)),
          children(static_cast<std::size_t>(n_)),
          dp0(static_cast<std::size_t>(n_)),
          dp1(static_cast<std::size_t>(n_)),
          choice_child(static_cast<std::size_t>(n_)),
          best_state(static_cast<std::size_t>(n_)),
          visited(static_cast<std::size_t>(n_), 0),
          used_edge(input_edges.size(), 0),
          cycle_edge(input_edges.size(), 0),
          removed(static_cast<std::size_t>(n_), 0),
          degree(static_cast<std::size_t>(n_), 0){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (unicyclic_matching).");
        }
        edges.reserve(input_edges.size());
        for(const auto& edge: input_edges){
            if(edge.from < 0 || n <= edge.from || edge.to < 0 || n <= edge.to || edge.from == edge.to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (unicyclic_matching).");
            }
            int edge_id = static_cast<int>(edges.size());
            edges.push_back({edge.from, edge.to});
            graph[static_cast<std::size_t>(edge.from)].push_back({edge.to, edge_id});
            graph[static_cast<std::size_t>(edge.to)].push_back({edge.from, edge_id});
        }
    }

    UnicyclicMatchingResult run(){
        int total = 0;
        for(int root = 0; root < n; root++){
            if(visited[static_cast<std::size_t>(root)]) continue;
            std::vector<int> vertices;
            std::vector<int> component_edges;
            collect_component(root, vertices, component_edges);
            if(component_edges.size() + 1 == vertices.size()){
                total += solve_tree_component(vertices);
            }else if(component_edges.size() == vertices.size()){
                total += solve_unicyclic_component(vertices, component_edges);
            }else{
                throw std::runtime_error("library assertion fault: graph is not a forest or unicyclic graph (unicyclic_matching).");
            }
        }
        return {total, match};
    }
};

} // namespace unicyclic_matching_internal

inline UnicyclicMatchingResult unicyclic_matching(int n, const std::vector<UnicyclicMatchingEdge>& edges){
    unicyclic_matching_internal::Solver solver(n, edges);
    return solver.run();
}