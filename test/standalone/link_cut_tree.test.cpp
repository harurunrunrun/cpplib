// competitive-verifier: STANDALONE

#include <algorithm>
#include "../../src/structure/tree/query/tree_edge_point_set_path_maximum.hpp"
#include "../../src/structure/tree/query/monochromatic_component_size_tree.hpp"
#include "../../src/structure/tree/query/monochromatic_component_max_tree.hpp"
#include "../../src/structure/tree/query/root_path_first_marked.hpp"
#include "../../src/structure/tree/query/tree_path_assign_max_subarray.hpp"
#include "../../src/structure/tree/query/subtree_add_sum_minimum_tree.hpp"
#include "../../src/structure/tree/centroid/tree_contour_fenwick.hpp"
#include "../../src/structure/tree/centroid/marked_tree_diameter_queries.hpp"
#include "../../src/structure/tree/centroid/nearest_marked_tree_queries.hpp"
#include "../../src/structure/tree/dynamic_forest/dynamic_forest_connectivity.hpp"
#include "../../src/structure/bbst/sequence/dynamic_maximum_subarray_sum_sequence.hpp"
#include "../../src/structure/bbst/sequence/dynamic_power_moment_sequence.hpp"

#include <cassert>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/tree/dynamic_forest/link_cut_tree.hpp"
#include "../../src/structure/types/monoid/monoid.hpp"

long long add_ll(long long a, long long b){ return a + b; }
long long zero_ll(){ return 0; }
constexpr Monoid<add_ll, zero_ll> sum_monoid;

std::string concat_string(std::string a, std::string b){ return a + b; }
std::string empty_string(){ return ""; }
constexpr Monoid<concat_string, empty_string> concat_monoid;

long long naive_path_sum(
    int s,
    int t,
    const std::vector<std::vector<int>>& graph,
    const std::vector<long long>& value
){
    int n = static_cast<int>(graph.size());
    std::vector<int> parent(n, -1);
    std::queue<int> que;
    parent[s] = s;
    que.push(s);
    while(!que.empty()){
        int v = que.front();
        que.pop();
        if(v == t) break;
        for(int to: graph[v]){
            if(parent[to] == -1){
                parent[to] = v;
                que.push(to);
            }
        }
    }
    assert(parent[t] != -1);
    long long result = 0;
    for(int v = t;; v = parent[v]){
        result += value[v];
        if(v == s) break;
    }
    return result;
}

std::vector<int> component(int s, const std::vector<std::vector<int>>& graph){
    int n = static_cast<int>(graph.size());
    std::vector<int> seen(n, 0), result;
    std::queue<int> que;
    seen[s] = 1;
    que.push(s);
    while(!que.empty()){
        int v = que.front();
        que.pop();
        result.push_back(v);
        for(int to: graph[v]){
            if(!seen[to]){
                seen[to] = 1;
                que.push(to);
            }
        }
    }
    return result;
}

void dynamic_capacity_wrapper_regression(){
    DynamicForestConnectivity<> forest(3);
    assert(forest.add_edge(0, 1)); assert(forest.connected(0, 1));
    assert(forest.remove_edge(0, 1)); assert(!forest.connected(0, 1));
    DynamicMaximumSubarraySumSequence<long long> sequence(std::vector<long long>{-1, 3, -2});
    sequence.insert(3, 5); assert(sequence.maximum_sum(0, 4) == 6);
    DynamicPowerMomentSequence<long long, 2> moments(std::vector<long long>{1, 2, 3});
    assert(moments.power_moment(0, 3, 0) == 6);
    TreePathAssignMaxSubarray<long long> paths(std::vector<long long>{-1, 3, -2});
    assert(paths.add_edge(0, 1)); assert(paths.add_edge(1, 2));
    assert(paths.path_max_subarray(0, 2) == 3); paths.assign_path(0, 2, 2);
    assert(paths.path_max_subarray(2, 0) == 6);
    MonochromaticComponentSizeTree<> sizes(3);
    MonochromaticComponentMaxTree<int> maxima(3);
    RootPathFirstMarked<> marked(3);
    TreeEdgePointSetPathMaximum<int> edges(3);
    SubtreeAddSumMinimumTree<> subtrees(3);
    MarkedTreeDiameterQueries<int> diameter(3);
    NearestMarkedTreeQueries<int> nearest(3);
    for(int v = 0; v < 3; ++v){maxima.set_value(v, v + 1); subtrees.set_value(v, v + 1);}
    for(int v = 1; v < 3; ++v){
        sizes.add_edge(v - 1, v); maxima.add_edge(v - 1, v); marked.add_edge(v - 1, v);
        edges.add_edge(v - 1, v, v); subtrees.add_edge(v - 1, v);
        diameter.add_edge(v - 1, v, 1); nearest.add_edge(v - 1, v);
    }
    sizes.build(); maxima.build(); marked.build(); edges.build(); subtrees.build();
    diameter.build(); nearest.build();
    assert(sizes.component_size(1) == 3); assert(maxima.component_maximum(0) == 3);
    sizes.toggle(1); assert(sizes.component_size(0) == 1);
    assert(marked.toggle(1)); assert(marked.first_marked(2) == 1);
    assert(edges.path_maximum(0, 2) == 2); edges.set_edge(0, 7); assert(edges.path_maximum(0, 2) == 7);
    assert(subtrees.subtree_sum(1) == 5); subtrees.subtree_add(1, 4); assert(subtrees.subtree_minimum(0) == 1);
    assert(diameter.diameter() == 2); nearest.toggle(2); assert(nearest.nearest_distance(0) == 2);
    const std::vector<std::tuple<int, int, int>> weighted{{0, 1, 1}, {1, 2, 1}};
    TreeContourFenwick<long long, int> contours(std::vector<long long>{1, 2, 3}, weighted);
    assert(contours.contour_sum(0, 1, 3) == 5);
    TreeContourFenwick<long long, int, -1, true> dual(std::vector<long long>{1, 2, 3}, weighted);
    dual.contour_add(0, 1, 2, 7); assert(dual.point_get(1) == 9); assert(dual.point_get(2) == 3);
}

int main(){
    {
        LinkCutTree<AddMonoid<bool>{}> boolean_tree(2);
        assert(boolean_tree.link(0, 1)); boolean_tree.set(1, true);
        boolean_tree.evert(0); assert(boolean_tree.path_prod(0, 1));
    }
    dynamic_capacity_wrapper_regression();
    {
        LinkCutTree<sum_monoid> growing(0);
        for(int v = 0; v < 1024; ++v){
            assert(growing.add_vertex(1) == v);
            if(v) assert(growing.link(v - 1, v));
            assert(growing.path_prod(0, v) == v + 1);
        }
    }
    int input_n, input_m, q;
    if(std::cin >> input_n >> input_m >> q){
        std::vector<long long> input(static_cast<std::size_t>(input_n));
        for(long long& value: input) std::cin >> value;
        LinkCutTree<sum_monoid> tree(input);
        for(int i = 0; i < input_m; i++){
            int u, v;
            std::cin >> u >> v;
            assert(tree.link(u, v));
        }
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "LINK" || type == "CUT"){
                int u, v;
                std::cin >> u >> v;
                if(type == "LINK") std::cout << tree.link(u, v) << '\n';
                else std::cout << tree.cut(u, v) << '\n';
            }else if(type == "CONNECTED"){
                int u, v;
                std::cin >> u >> v;
                std::cout << tree.connected(u, v) << '\n';
            }else if(type == "SET"){
                int v;
                long long value;
                std::cin >> v >> value;
                tree.set(v, value);
            }else if(type == "GET"){
                int v;
                std::cin >> v;
                std::cout << tree.get(v) << '\n';
            }else if(type == "PROD"){
                int u, v;
                std::cin >> u >> v;
                std::cout << tree.path_prod(u, v) << '\n';
            }
        }
        return 0;
    }


    {
        std::vector<std::string> value = {"a", "b", "c"};
        LinkCutTree<concat_monoid, 10> tree(value);
        assert(tree.link(0, 1));
        assert(tree.link(1, 2));
        assert(tree.path_prod(0, 2) == "abc");
        assert(tree.path_prod(2, 0) == "cba");
    }

    constexpr int n = 50;
    std::mt19937 rng(424242);
    std::vector<long long> value(n);
    for(auto& x: value) x = static_cast<int>(rng() % 200) - 80;
    LinkCutTree<sum_monoid> tree(value);

    std::vector<std::vector<int>> graph(n);
    std::vector<std::pair<int, int>> edges;
    for(int v = 1; v < n; v++){
        int p = static_cast<int>(rng() % v);
        assert(tree.link(v, p));
        graph[v].push_back(p);
        graph[p].push_back(v);
        edges.emplace_back(v, p);
    }

    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 10);
        if(type < 4){
            int v = static_cast<int>(rng() % n);
            long long delta = static_cast<int>(rng() % 101) - 40;
            value[v] += delta;
            tree.set(v, value[v]);
            assert(tree.get(v) == value[v]);
        }else if(type < 8){
            int u = static_cast<int>(rng() % n);
            int v = static_cast<int>(rng() % n);
            assert(tree.path_prod(u, v) == naive_path_sum(u, v, graph, value));
            assert(tree.connected(u, v));
        }else{
            int id = static_cast<int>(rng() % edges.size());
            auto [a, b] = edges[id];
            assert(tree.cut(a, b));
            graph[a].erase(std::find(graph[a].begin(), graph[a].end(), b));
            graph[b].erase(std::find(graph[b].begin(), graph[b].end(), a));
            edges.erase(edges.begin() + id);

            auto comp = component(a, graph);
            std::vector<int> in_a(n, 0);
            for(int v: comp) in_a[v] = 1;
            int u = comp[static_cast<std::size_t>(rng() % comp.size())];
            std::vector<int> other;
            for(int v = 0; v < n; v++) if(!in_a[v]) other.push_back(v);
            int v = other[static_cast<std::size_t>(rng() % other.size())];
            assert(!tree.connected(u, v));
            assert(tree.link(u, v));
            graph[u].push_back(v);
            graph[v].push_back(u);
            edges.emplace_back(u, v);
        }
    }
}
