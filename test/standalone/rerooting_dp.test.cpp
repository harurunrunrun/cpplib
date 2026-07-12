// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/rerooting_dp.hpp"

struct MaxIntMonoid{
    using S = int;

    constexpr S op(S a, S b) const{
        return a > b ? a : b;
    }

    constexpr S e() const{
        return 0;
    }
};

constexpr MaxIntMonoid max_int_monoid{};

struct DistanceValue{
    int size;
    long long sum;

    friend bool operator==(const DistanceValue& a, const DistanceValue& b){
        return a.size == b.size && a.sum == b.sum;
    }
};

struct DistanceMonoid{
    using S = DistanceValue;

    constexpr S op(S a, S b) const{
        return {a.size + b.size, a.sum + b.sum};
    }

    constexpr S e() const{
        return {0, 0};
    }
};

constexpr DistanceMonoid distance_monoid{};

void add_edge(std::vector<std::vector<int>>& graph, int u, int v){
    graph[u].push_back(v);
    graph[v].push_back(u);
}

std::vector<int> naive_distances(const std::vector<std::vector<int>>& graph, int root){
    std::vector<int> dist(graph.size(), -1);
    std::vector<int> stack = {root};
    dist[root] = 0;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        for(int to: graph[v]){
            if(dist[to] != -1){
                continue;
            }
            dist[to] = dist[v] + 1;
            stack.push_back(to);
        }
    }
    return dist;
}

void test_eccentricity(){
    const int n = 5;
    std::vector<std::vector<int>> graph(n);
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 3, 4);

    auto add_vertex = [](int aggregate, int v){
        (void)v;
        return aggregate;
    };
    auto add_edge_func = [](int value, int from, int to, int edge_id){
        (void)from;
        (void)to;
        (void)edge_id;
        return value + 1;
    };

    RerootingDP<max_int_monoid, decltype(add_vertex), decltype(add_edge_func)> dp(
        n,
        add_vertex,
        add_edge_func
    );
    dp.add_edge(0, 1);
    dp.add_edge(1, 2);
    dp.add_edge(1, 3);
    dp.add_edge(3, 4);

    auto result = dp.solve();
    for(int root = 0; root < n; root++){
        auto dist = naive_distances(graph, root);
        int expected = 0;
        for(int d: dist){
            expected = expected > d ? expected : d;
        }
        assert(result[root] == expected);
    }

    auto result_from_other_root = dp.solve(2);
    assert(result == result_from_other_root);
}

void test_sum_of_distances(){
    const int n = 7;
    std::vector<std::vector<int>> graph(n);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 1, 4);
    add_edge(graph, 2, 5);
    add_edge(graph, 5, 6);

    auto add_vertex = [](DistanceValue aggregate, int v){
        (void)v;
        return DistanceValue{aggregate.size + 1, aggregate.sum};
    };
    auto add_edge_func = [](DistanceValue value, int from, int to, int edge_id){
        (void)from;
        (void)to;
        (void)edge_id;
        return DistanceValue{value.size, value.sum + value.size};
    };

    RerootingDP<distance_monoid, decltype(add_vertex), decltype(add_edge_func)> dp(
        n,
        add_vertex,
        add_edge_func
    );
    dp.add_edge(0, 1);
    dp.add_edge(0, 2);
    dp.add_edge(1, 3);
    dp.add_edge(1, 4);
    dp.add_edge(2, 5);
    dp.add_edge(5, 6);

    auto result = dp.solve();
    for(int root = 0; root < n; root++){
        auto dist = naive_distances(graph, root);
        long long expected = 0;
        for(int d: dist){
            expected += d;
        }
        assert(result[root] == (DistanceValue{n, expected}));
    }
}

void test_empty_and_single(){
    auto add_vertex = [](int aggregate, int v){
        (void)v;
        return aggregate;
    };
    auto add_edge_func = [](int value, int from, int to, int edge_id){
        (void)from;
        (void)to;
        (void)edge_id;
        return value + 1;
    };

    RerootingDP<max_int_monoid, decltype(add_vertex), decltype(add_edge_func)> empty(
        0,
        add_vertex,
        add_edge_func
    );
    assert(empty.solve().empty());

    RerootingDP<max_int_monoid, decltype(add_vertex), decltype(add_edge_func)> one(
        1,
        add_vertex,
        add_edge_func
    );
    assert(one.solve() == std::vector<int>{0});
}

void test_exceptions(){
    auto add_vertex = [](int aggregate, int v){
        (void)v;
        return aggregate;
    };
    auto add_edge_func = [](int value, int from, int to, int edge_id){
        (void)from;
        (void)to;
        (void)edge_id;
        return value + 1;
    };

    bool thrown = false;
    try{
        RerootingDP<max_int_monoid, decltype(add_vertex), decltype(add_edge_func)> bad(
            3,
            add_vertex,
            add_edge_func
        );
        bad.add_edge(0, 1);
        (void)bad.solve();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        RerootingDP<max_int_monoid, decltype(add_vertex), decltype(add_edge_func)> bad(
            4,
            add_vertex,
            add_edge_func
        );
        bad.add_edge(0, 1);
        bad.add_edge(1, 2);
        bad.add_edge(2, 0);
        (void)bad.solve();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        RerootingDP<max_int_monoid, decltype(add_vertex), decltype(add_edge_func)> bad(
            1,
            add_vertex,
            add_edge_func
        );
        bad.add_edge(0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int case_count;
    if(std::cin >> case_count){
        auto add_ecc_vertex = [](int aggregate, int vertex){
            (void)vertex;
            return aggregate;
        };
        auto add_ecc_edge = [](int value, int from, int to, int edge_id){
            (void)from;
            (void)to;
            (void)edge_id;
            return value + 1;
        };
        auto add_sum_vertex = [](DistanceValue aggregate, int vertex){
            (void)vertex;
            return DistanceValue{aggregate.size + 1, aggregate.sum};
        };
        auto add_sum_edge = [](DistanceValue value, int from, int to, int edge_id){
            (void)from;
            (void)to;
            (void)edge_id;
            return DistanceValue{value.size, value.sum + value.size};
        };
        while(case_count--){
            int n;
            std::cin >> n;
            RerootingDP<max_int_monoid, decltype(add_ecc_vertex), decltype(add_ecc_edge)> eccentricity(
                n, add_ecc_vertex, add_ecc_edge
            );
            RerootingDP<distance_monoid, decltype(add_sum_vertex), decltype(add_sum_edge)> distance_sum(
                n, add_sum_vertex, add_sum_edge
            );
            for(int i = 0; i + 1 < n; i++){
                int u, v;
                std::cin >> u >> v;
                eccentricity.add_edge(u, v);
                distance_sum.add_edge(u, v);
            }
            const auto ecc = eccentricity.solve();
            const auto sums = distance_sum.solve();
            std::cout << n;
            for(int value: ecc) std::cout << ' ' << value;
            std::cout << '\n' << n;
            for(const auto& value: sums) std::cout << ' ' << value.sum;
            std::cout << '\n';
        }
        return 0;
    }
    test_eccentricity();
    test_sum_of_distances();
    test_empty_and_single();
    test_exceptions();
}
