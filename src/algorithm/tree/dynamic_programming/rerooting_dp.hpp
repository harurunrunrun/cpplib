#ifndef CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_REROOTING_DP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_REROOTING_DP_HPP_INCLUDED

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid, class AddVertex, class AddEdge>
struct RerootingDP{
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    struct Edge{
        int to;
        int id;
    };

    int _n;
    int edge_count = 0;
    std::vector<std::vector<Edge>> graph;
    AddVertex add_vertex;
    AddEdge add_edge_func;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    RerootingDP(int n, AddVertex add_vertex_, AddEdge add_edge_):
        _n(n),
        graph(n),
        add_vertex(std::move(add_vertex_)),
        add_edge_func(std::move(add_edge_)){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
    }

    int size() const{
        return _n;
    }

    int add_edge(int u, int v){
        check_vertex(u, "library assertion fault: range violation (add_edge).");
        check_vertex(v, "library assertion fault: range violation (add_edge).");
        const int id = edge_count++;
        graph[u].push_back({v, id});
        graph[v].push_back({u, id});
        return id;
    }

    std::vector<S> solve(int root = 0) const{
        if(_n == 0){
            return {};
        }
        check_vertex(root, "library assertion fault: range violation (solve).");
        if(edge_count != _n - 1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not a tree (solve).");
        }

        std::vector<int> parent(_n, -1), order;
        order.reserve(_n);
        std::vector<int> stack = {root};
        parent[root] = -2;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            order.push_back(v);
            for(const Edge& edge: graph[v]){
                int to = edge.to;
                if(to == parent[v]){
                    continue;
                }
                if(parent[to] != -1)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: graph is not a tree (solve).");
                }
                parent[to] = v;
                stack.push_back(to);
            }
        }
        if((int)order.size() != _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not connected (solve).");
        }
        parent[root] = -1;

        std::vector<S> down(_n, Monoid.e());
        for(auto it = order.rbegin(); it != order.rend(); ++it){
            int v = *it;
            S aggregate = Monoid.e();
            for(const Edge& edge: graph[v]){
                int to = edge.to;
                if(parent[to] == v){
                    aggregate = Monoid.op(
                        aggregate,
                        add_edge_func(down[to], to, v, edge.id)
                    );
                }
            }
            down[v] = add_vertex(aggregate, v);
        }

        std::vector<S> answer(_n, Monoid.e());
        std::vector<S> parent_contribution(_n, Monoid.e());

        for(int v: order){
            const int degree = (int)graph[v].size();
            std::vector<S> contribution(degree, Monoid.e());
            for(int i = 0; i < degree; i++){
                const Edge& edge = graph[v][i];
                int to = edge.to;
                if(to == parent[v]){
                    contribution[i] = parent_contribution[v];
                }else{
                    contribution[i] = add_edge_func(down[to], to, v, edge.id);
                }
            }

            std::vector<S> prefix(degree + 1, Monoid.e()), suffix(degree + 1, Monoid.e());
            for(int i = 0; i < degree; i++){
                prefix[i + 1] = Monoid.op(prefix[i], contribution[i]);
            }
            for(int i = degree - 1; i >= 0; i--){
                suffix[i] = Monoid.op(contribution[i], suffix[i + 1]);
            }

            answer[v] = add_vertex(prefix[degree], v);

            for(int i = 0; i < degree; i++){
                const Edge& edge = graph[v][i];
                int to = edge.to;
                if(parent[to] != v){
                    continue;
                }
                S without_child = Monoid.op(prefix[i], suffix[i + 1]);
                S value_at_v = add_vertex(without_child, v);
                parent_contribution[to] = add_edge_func(value_at_v, v, to, edge.id);
            }
        }

        return answer;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_REROOTING_DP_HPP_INCLUDED
