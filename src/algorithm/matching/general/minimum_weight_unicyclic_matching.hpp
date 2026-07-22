#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_MINIMUM_WEIGHT_UNICYCLIC_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_MINIMUM_WEIGHT_UNICYCLIC_MATCHING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct MinimumWeightUnicyclicMatchingEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct MinimumWeightUnicyclicMatchingResult{
    int size;
    T cost;
    std::vector<int> match;
};

namespace minimum_weight_unicyclic_matching_internal{

template<class T>
struct Score{
    int size = -1;
    T cost = T(0);
};

template<class T>
bool valid(const Score<T>& score){
    return score.size >= 0;
}

template<class T>
bool better(const Score<T>& lhs, const Score<T>& rhs){
    if(!valid(lhs)) return false;
    if(!valid(rhs)) return true;
    if(lhs.size != rhs.size) return lhs.size > rhs.size;
    return lhs.cost < rhs.cost;
}

template<class T>
Score<T> add(const Score<T>& lhs, const Score<T>& rhs){
    if(!valid(lhs) || !valid(rhs)) return {};
    return {lhs.size + rhs.size, lhs.cost + rhs.cost};
}

template<class T>
Score<T> sub(const Score<T>& lhs, const Score<T>& rhs){
    if(!valid(lhs) || !valid(rhs)) return {};
    return {lhs.size - rhs.size, lhs.cost - rhs.cost};
}

template<class T>
struct PathResult{
    Score<T> score;
    std::vector<int> selected_edges;
};

template<class T>
class Solver{
    struct Edge{
        int from;
        int to;
        T cost;
    };

    int n;
    std::vector<Edge> edges;
    std::vector<std::vector<std::pair<int, int>>> graph;
    std::vector<int> match;
    std::vector<int> parent;
    std::vector<int> parent_edge;
    std::vector<std::vector<int>> children;
    std::vector<Score<T>> dp0;
    std::vector<Score<T>> dp1;
    std::vector<int> choice_child;
    std::vector<int> best_state;
    std::vector<char> visited;
    std::vector<char> used_edge;
    std::vector<char> cycle_edge;
    std::vector<char> removed;
    std::vector<int> degree;

    Score<T> zero() const{
        return {0, T(0)};
    }

    Score<T> edge_score(int edge_id) const{
        return {1, edges[static_cast<std::size_t>(edge_id)].cost};
    }

    Score<T> best_tree_score(int v) const{
        return better(dp1[static_cast<std::size_t>(v)], dp0[static_cast<std::size_t>(v)])
                   ? dp1[static_cast<std::size_t>(v)]
                   : dp0[static_cast<std::size_t>(v)];
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
            Score<T> base = zero();
            for(int to: children[static_cast<std::size_t>(v)]){
                base = add(base, best_tree_score(to));
            }
            dp0[static_cast<std::size_t>(v)] = base;
            dp1[static_cast<std::size_t>(v)] = {};
            choice_child[static_cast<std::size_t>(v)] = -1;
            for(int to: children[static_cast<std::size_t>(v)]){
                Score<T> candidate = sub(base, best_tree_score(to));
                candidate = add(candidate, dp0[static_cast<std::size_t>(to)]);
                candidate = add(candidate, edge_score(parent_edge[static_cast<std::size_t>(to)]));
                if(better(candidate, dp1[static_cast<std::size_t>(v)])){
                    dp1[static_cast<std::size_t>(v)] = candidate;
                    choice_child[static_cast<std::size_t>(v)] = to;
                }
            }
            best_state[static_cast<std::size_t>(v)] =
                better(dp1[static_cast<std::size_t>(v)], dp0[static_cast<std::size_t>(v)]) ? 1 : 0;
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

    PathResult<T> solve_path(
        int left,
        int right,
        const std::vector<Score<T>>& free_score,
        const std::vector<Score<T>>& best_score,
        const std::vector<Score<T>>& cycle_edge_score
    ) const{
        int length = right - left;
        std::vector<Score<T>> dp(static_cast<std::size_t>(length + 1));
        std::vector<int> previous(static_cast<std::size_t>(length + 1), -1);
        std::vector<int> action(static_cast<std::size_t>(length + 1), -2);
        dp[0] = zero();
        for(int i = 0; i < length; i++){
            if(!valid(dp[static_cast<std::size_t>(i)])) continue;
            int vertex = left + i;
            Score<T> leave = add(dp[static_cast<std::size_t>(i)], best_score[static_cast<std::size_t>(vertex)]);
            if(better(leave, dp[static_cast<std::size_t>(i + 1)])){
                dp[static_cast<std::size_t>(i + 1)] = leave;
                previous[static_cast<std::size_t>(i + 1)] = i;
                action[static_cast<std::size_t>(i + 1)] = -1;
            }
            if(i + 1 < length){
                Score<T> take = add(dp[static_cast<std::size_t>(i)], free_score[static_cast<std::size_t>(vertex)]);
                take = add(take, free_score[static_cast<std::size_t>(vertex + 1)]);
                take = add(take, cycle_edge_score[static_cast<std::size_t>(vertex)]);
                if(better(take, dp[static_cast<std::size_t>(i + 2)])){
                    dp[static_cast<std::size_t>(i + 2)] = take;
                    previous[static_cast<std::size_t>(i + 2)] = i;
                    action[static_cast<std::size_t>(i + 2)] = vertex;
                }
            }
        }
        PathResult<T> result;
        result.score = dp[static_cast<std::size_t>(length)];
        for(int current = length; current > 0; current = previous[static_cast<std::size_t>(current)]){
            int chosen = action[static_cast<std::size_t>(current)];
            if(chosen >= 0) result.selected_edges.push_back(chosen);
        }
        return result;
    }

    PathResult<T> solve_cycle(
        const std::vector<Score<T>>& free_score,
        const std::vector<Score<T>>& best_score,
        const std::vector<Score<T>>& cycle_edge_score
    ) const{
        int k = static_cast<int>(free_score.size());
        PathResult<T> best = solve_path(0, k, free_score, best_score, cycle_edge_score);
        PathResult<T> use_last = solve_path(1, k - 1, free_score, best_score, cycle_edge_score);
        Score<T> last = add(free_score[static_cast<std::size_t>(k - 1)], free_score[0]);
        last = add(last, cycle_edge_score[static_cast<std::size_t>(k - 1)]);
        use_last.score = add(last, use_last.score);
        use_last.selected_edges.push_back(k - 1);
        if(better(use_last.score, best.score)) return use_last;
        return best;
    }

    Score<T> solve_tree_component(const std::vector<int>& vertices){
        std::vector<int> order = build_forest({vertices[0]}, vertices);
        compute_tree_dp(order);
        int state = best_state[static_cast<std::size_t>(vertices[0])];
        restore_tree(vertices[0], state);
        return best_tree_score(vertices[0]);
    }

    Score<T> solve_unicyclic_component(
        const std::vector<int>& vertices,
        const std::vector<int>& component_edges
    ){
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
            throw std::runtime_error("library assertion fault: graph is not unicyclic (minimum_weight_unicyclic_matching).");
        }

        std::vector<int> cycle;
        std::vector<int> ordered_cycle_edges;
        if(k == 2){
            cycle = cycle_vertices;
            ordered_cycle_edges = cycle_edges;
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
                    throw std::runtime_error("library assertion fault: graph is not unicyclic (minimum_weight_unicyclic_matching).");
                }
                ordered_cycle_edges.push_back(chosen_edge);
                const Edge& edge = edges[static_cast<std::size_t>(chosen_edge)];
                int next = edge.from ^ edge.to ^ current;
                previous_edge = chosen_edge;
                current = next;
            }while(current != start);
            if(static_cast<int>(cycle.size()) != k || static_cast<int>(ordered_cycle_edges.size()) != k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph is not unicyclic (minimum_weight_unicyclic_matching).");
            }
        }

        std::vector<int> order = build_forest(cycle, vertices);
        compute_tree_dp(order);

        std::vector<Score<T>> free_score(static_cast<std::size_t>(k));
        std::vector<Score<T>> best_score(static_cast<std::size_t>(k));
        std::vector<Score<T>> cycle_edge_score(static_cast<std::size_t>(k));
        for(int i = 0; i < k; i++){
            int v = cycle[static_cast<std::size_t>(i)];
            free_score[static_cast<std::size_t>(i)] = dp0[static_cast<std::size_t>(v)];
            best_score[static_cast<std::size_t>(i)] = best_tree_score(v);
            cycle_edge_score[static_cast<std::size_t>(i)] =
                edge_score(ordered_cycle_edges[static_cast<std::size_t>(i)]);
        }
        PathResult<T> cycle_result = solve_cycle(free_score, best_score, cycle_edge_score);
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
        return cycle_result.score;
    }

public:
    Solver(int n_, const std::vector<MinimumWeightUnicyclicMatchingEdge<T>>& input_edges)
        : n(n_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (minimum_weight_unicyclic_matching)."
            )
            : n_),
          graph(static_cast<std::size_t>(n)),
          match(static_cast<std::size_t>(n), -1),
          parent(static_cast<std::size_t>(n)),
          parent_edge(static_cast<std::size_t>(n)),
          children(static_cast<std::size_t>(n)),
          dp0(static_cast<std::size_t>(n)),
          dp1(static_cast<std::size_t>(n)),
          choice_child(static_cast<std::size_t>(n)),
          best_state(static_cast<std::size_t>(n)),
          visited(static_cast<std::size_t>(n), 0),
          used_edge(input_edges.size(), 0),
          cycle_edge(input_edges.size(), 0),
          removed(static_cast<std::size_t>(n), 0),
          degree(static_cast<std::size_t>(n), 0){
        edges.reserve(input_edges.size());
        for(const auto& edge: input_edges){
            if(edge.from < 0 || n <= edge.from || edge.to < 0 || n <= edge.to || edge.from == edge.to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (minimum_weight_unicyclic_matching).");
            }
            int edge_id = static_cast<int>(edges.size());
            edges.push_back({edge.from, edge.to, edge.cost});
            graph[static_cast<std::size_t>(edge.from)].push_back({edge.to, edge_id});
            graph[static_cast<std::size_t>(edge.to)].push_back({edge.from, edge_id});
        }
    }

    MinimumWeightUnicyclicMatchingResult<T> run(){
        Score<T> total = zero();
        for(int root = 0; root < n; root++){
            if(visited[static_cast<std::size_t>(root)]) continue;
            std::vector<int> vertices;
            std::vector<int> component_edges;
            collect_component(root, vertices, component_edges);
            Score<T> current;
            if(component_edges.size() + 1 == vertices.size()){
                current = solve_tree_component(vertices);
            }else if(component_edges.size() == vertices.size()){
                current = solve_unicyclic_component(vertices, component_edges);
            }else{
                throw std::runtime_error("library assertion fault: graph is not a forest or unicyclic graph (minimum_weight_unicyclic_matching).");
            }
            total = add(total, current);
        }
        return {total.size, total.cost, match};
    }
};

} // namespace minimum_weight_unicyclic_matching_internal

template<class T>
MinimumWeightUnicyclicMatchingResult<T> minimum_weight_unicyclic_matching(
    int n,
    const std::vector<MinimumWeightUnicyclicMatchingEdge<T>>& edges
){
    minimum_weight_unicyclic_matching_internal::Solver<T> solver(n, edges);
    return solver.run();
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_MINIMUM_WEIGHT_UNICYCLIC_MATCHING_HPP_INCLUDED
