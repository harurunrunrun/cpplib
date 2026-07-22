// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree/matrix_tree_theorem.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

using Mint = FastModint998244353;
using Edge = MatrixTreeEdge<Mint>;

Mint brute_undirected(int n, const std::vector<Edge>& edges){
    if(n == 0) return Mint(1);
    const int m = static_cast<int>(edges.size());
    assert(m <= 20);
    Mint answer;
    const std::uint64_t limit = std::uint64_t(1) << m;
    for(std::uint64_t mask = 0; mask < limit; mask++){
        if(__builtin_popcountll(mask) != n - 1) continue;
        std::vector<int> parent(static_cast<std::size_t>(n));
        for(int i = 0; i < n; i++) parent[static_cast<std::size_t>(i)] = i;
        const auto find = [&](auto&& self, int vertex) -> int{
            int& p = parent[static_cast<std::size_t>(vertex)];
            return p == vertex ? vertex : p = self(self, p);
        };

        Mint product = 1;
        bool tree = true;
        for(int index = 0; index < m; index++){
            if((mask >> index & 1U) == 0) continue;
            const auto& edge = edges[static_cast<std::size_t>(index)];
            int from = find(find, edge.from);
            int to = find(find, edge.to);
            if(from == to){
                tree = false;
                break;
            }
            parent[static_cast<std::size_t>(from)] = to;
            product *= edge.weight;
        }
        if(!tree) continue;
        const int component = find(find, 0);
        for(int vertex = 1; vertex < n; vertex++){
            if(find(find, vertex) != component) tree = false;
        }
        if(tree) answer += product;
    }
    return answer;
}

Mint brute_out_arborescences(
    int n,
    int root,
    const std::vector<Edge>& edges
){
    const int m = static_cast<int>(edges.size());
    assert(m <= 20);
    Mint answer;
    const std::uint64_t limit = std::uint64_t(1) << m;
    for(std::uint64_t mask = 0; mask < limit; mask++){
        if(__builtin_popcountll(mask) != n - 1) continue;
        std::vector<int> indegree(static_cast<std::size_t>(n));
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
        Mint product = 1;
        for(int index = 0; index < m; index++){
            if((mask >> index & 1U) == 0) continue;
            const auto& edge = edges[static_cast<std::size_t>(index)];
            indegree[static_cast<std::size_t>(edge.to)]++;
            graph[static_cast<std::size_t>(edge.from)].push_back(edge.to);
            product *= edge.weight;
        }
        bool tree = indegree[static_cast<std::size_t>(root)] == 0;
        for(int vertex = 0; vertex < n; vertex++){
            if(vertex != root && indegree[static_cast<std::size_t>(vertex)] != 1){
                tree = false;
            }
        }
        if(!tree) continue;

        std::vector<char> reached(static_cast<std::size_t>(n));
        std::queue<int> queue;
        reached[static_cast<std::size_t>(root)] = 1;
        queue.push(root);
        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(reached[static_cast<std::size_t>(to)]) continue;
                reached[static_cast<std::size_t>(to)] = 1;
                queue.push(to);
            }
        }
        for(char value: reached) if(!value) tree = false;
        if(tree) answer += product;
    }
    return answer;
}

Mint brute_in_arborescences(
    int n,
    int root,
    const std::vector<Edge>& edges
){
    const int m = static_cast<int>(edges.size());
    assert(m <= 20);
    Mint answer;
    const std::uint64_t limit = std::uint64_t(1) << m;
    for(std::uint64_t mask = 0; mask < limit; mask++){
        if(__builtin_popcountll(mask) != n - 1) continue;
        std::vector<int> outdegree(static_cast<std::size_t>(n));
        std::vector<std::vector<int>> reverse(static_cast<std::size_t>(n));
        Mint product = 1;
        for(int index = 0; index < m; index++){
            if((mask >> index & 1U) == 0) continue;
            const auto& edge = edges[static_cast<std::size_t>(index)];
            outdegree[static_cast<std::size_t>(edge.from)]++;
            reverse[static_cast<std::size_t>(edge.to)].push_back(edge.from);
            product *= edge.weight;
        }
        bool tree = outdegree[static_cast<std::size_t>(root)] == 0;
        for(int vertex = 0; vertex < n; vertex++){
            if(vertex != root && outdegree[static_cast<std::size_t>(vertex)] != 1){
                tree = false;
            }
        }
        if(!tree) continue;

        std::vector<char> reached(static_cast<std::size_t>(n));
        std::queue<int> queue;
        reached[static_cast<std::size_t>(root)] = 1;
        queue.push(root);
        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            for(int from: reverse[static_cast<std::size_t>(vertex)]){
                if(reached[static_cast<std::size_t>(from)]) continue;
                reached[static_cast<std::size_t>(from)] = 1;
                queue.push(from);
            }
        }
        for(char value: reached) if(!value) tree = false;
        if(tree) answer += product;
    }
    return answer;
}

void check_against_brute(
    int n,
    int root,
    const std::vector<Edge>& edges
){
    assert((count_weighted_undirected_spanning_trees<Mint, 256>(n, edges)
        == brute_undirected(n, edges)));
    assert((count_weighted_directed_out_arborescences<Mint, 256>(
        n, root, edges
    ) == brute_out_arborescences(n, root, edges)));
    assert((count_weighted_directed_in_arborescences<Mint, 256>(
        n, root, edges
    ) == brute_in_arborescences(n, root, edges)));
}

void self_test(){
    {
        MatrixTreeFixedMatrix<Mint, 4> matrix{};
        assert((matrix_tree_determinant<Mint, 4>(matrix, 0) == Mint(1)));
        matrix[0][1] = 1;
        matrix[1][0] = 1;
        assert((matrix_tree_determinant<Mint, 4>(matrix, 2) == Mint(-1)));
        matrix = {};
        matrix[0][0] = 1;
        matrix[0][1] = 2;
        matrix[1][0] = 2;
        matrix[1][1] = 4;
        assert((matrix_tree_determinant<Mint, 4>(matrix, 2) == Mint(0)));
    }
    {
        const std::vector<Edge> empty;
        assert((count_weighted_undirected_spanning_trees<Mint, 0>(
            0, empty
        ) == Mint(1)));
    }
    {
        const std::vector<Edge> edges{{0, 0, 91}, {0, 0, -7}};
        check_against_brute(1, 0, edges);
    }
    {
        const std::vector<Edge> edges{
            {0, 1, 2},
            {0, 1, -3},
            {1, 0, 5},
            {0, 0, 100},
            {1, 1, -100},
        };
        check_against_brute(2, 0, edges);
    }
    {
        const std::vector<Edge> edges{
            {0, 1, 2},
            {0, 2, 3},
            {1, 2, 5},
            {2, 1, 7},
            {1, 0, -2},
            {2, 0, 11},
            {1, 1, 999},
        };
        check_against_brute(3, 0, edges);
        assert((count_weighted_directed_out_arborescences<Mint, 3>(
            3, 0, edges
        ) == Mint(37)));
    }
    {
        const std::vector<Edge> edges{{0, 1, 1}, {2, 3, 1}};
        assert((count_weighted_undirected_spanning_trees<Mint, 4>(
            4, edges
        ) == Mint(0)));
    }
    {
        bool thrown = false;
        try{
            const std::vector<Edge> edges;
            (void)count_weighted_undirected_spanning_trees<Mint, 2>(3, edges);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);

        thrown = false;
        try{
            const std::vector<Edge> edges{{0, 2, 1}};
            (void)count_weighted_undirected_spanning_trees<Mint, 2>(2, edges);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);

        thrown = false;
        try{
            const std::vector<Edge> edges;
            (void)count_weighted_directed_out_arborescences<Mint, 2>(
                0, 0, edges
            );
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);

        thrown = false;
        try{
            MatrixTreeFixedMatrix<Mint, 2> matrix{};
            (void)matrix_tree_determinant<Mint, 2>(matrix, 3);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    std::mt19937 rng(20260801);
    for(int n = 1; n <= 6; n++){
        for(int iteration = 0; iteration < 180; iteration++){
            const int m = static_cast<int>(rng() % 10);
            std::vector<Edge> edges;
            edges.reserve(static_cast<std::size_t>(m));
            for(int index = 0; index < m; index++){
                const int from = static_cast<int>(
                    rng() % static_cast<unsigned>(n)
                );
                const int to = static_cast<int>(
                    rng() % static_cast<unsigned>(n)
                );
                const int weight = static_cast<int>(rng() % 7) - 3;
                edges.push_back({from, to, weight});
            }
            const int root = static_cast<int>(
                rng() % static_cast<unsigned>(n)
            );
            check_against_brute(n, root, edges);
        }
    }

    {
        constexpr int n = 180;
        std::vector<Edge> undirected;
        for(int from = 0; from < n; from++){
            for(int to = from + 1; to < n; to++){
                undirected.push_back({from, to, 1});
            }
        }
        assert((count_weighted_undirected_spanning_trees<Mint, 256>(
            n, undirected
        ) == Mint(n).pow(n - 2)));

        std::vector<Edge> directed;
        for(int from = 0; from < n; from++){
            for(int to = 0; to < n; to++){
                if(from != to) directed.push_back({from, to, 1});
            }
        }
        const Mint expected = Mint(n).pow(n - 2);
        assert((count_weighted_directed_out_arborescences<Mint, 256>(
            n, 73, directed
        ) == expected));
        assert((count_weighted_directed_in_arborescences<Mint, 256>(
            n, 73, directed
        ) == expected));
    }
}

int main(){
    char mode;
    int n, m, root;
    if(!(std::cin >> mode >> n >> m >> root)){
        self_test();
        return 0;
    }

    std::vector<Edge> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int index = 0; index < m; index++){
        int from, to;
        long long weight;
        std::cin >> from >> to >> weight;
        edges.push_back({from, to, Mint(weight)});
    }

    if(mode == 'U'){
        std::cout << count_weighted_undirected_spanning_trees<Mint, 256>(
            n, edges
        ) << '\n';
    }else if(mode == 'O'){
        std::cout << count_weighted_directed_out_arborescences<Mint, 256>(
            n, root, edges
        ) << '\n';
    }else if(mode == 'I'){
        std::cout << count_weighted_directed_in_arborescences<Mint, 256>(
            n, root, edges
        ) << '\n';
    }else{
        throw std::runtime_error("unknown matrix-tree test mode");
    }
}
