// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/search/routing/arc_flags.hpp"
#include "../../src/approximate/search/routing/contraction_hierarchies.hpp"
#include "../../src/approximate/search/routing/hub_labeling.hpp"
#include "../../src/approximate/search/routing/reach_based_routing.hpp"
#include "../../src/approximate/search/routing/road_routing_common.hpp"
#include "../../src/approximate/search/routing/transit_node_routing.hpp"

using Cost = long long;
using Graph = std::vector<std::vector<std::pair<int, Cost>>>;

std::optional<Cost> reference_distance(
    const Graph& graph,
    int source,
    int target
){
    const Cost inf = std::numeric_limits<Cost>::max();
    std::vector<Cost> distance(graph.size(), inf);
    using Entry = std::pair<Cost, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    distance[static_cast<std::size_t>(source)] = 0;
    queue.emplace(0, source);
    while(!queue.empty()){
        const auto [current_distance, vertex] = queue.top();
        queue.pop();
        if(distance[static_cast<std::size_t>(vertex)] != current_distance){
            continue;
        }
        if(vertex == target) return current_distance;
        for(const auto& [to, cost] : graph[static_cast<std::size_t>(vertex)]){
            if(current_distance <= inf - cost
               && current_distance + cost
                    < distance[static_cast<std::size_t>(to)]){
                distance[static_cast<std::size_t>(to)] =
                    current_distance + cost;
                queue.emplace(current_distance + cost, to);
            }
        }
    }
    return std::nullopt;
}

template<class Oracle>
void compare_all_pairs(const Graph& graph, const Oracle& oracle){
    const int n = static_cast<int>(graph.size());
    for(int source = 0; source < n; ++source){
        for(int target = 0; target < n; ++target){
            assert(
                oracle.query_distance(source, target)
                == reference_distance(graph, source, target)
            );
        }
    }
}

std::vector<int> regions_for(int n){
    std::vector<int> regions(static_cast<std::size_t>(n));
    for(int vertex = 0; vertex < n; ++vertex){
        regions[static_cast<std::size_t>(vertex)] = vertex % 3;
    }
    return regions;
}

std::vector<int> transit_nodes_for(const Graph& graph){
    std::vector<int> transit;
    for(int vertex = 0; vertex < static_cast<int>(graph.size()); ++vertex){
        if(vertex % 3 == 0) transit.push_back(vertex);
    }
    return transit;
}

void check_graph(const Graph& graph){
    const int n = static_cast<int>(graph.size());
    ArcFlagsRouting<Cost> arc_flags(graph, regions_for(n));
    ReachBasedRouting<Cost> reach(graph);
    ReachBasedRouting<Cost> external_reach(
        graph, std::vector<Cost>(graph.size(), std::numeric_limits<Cost>::max())
    );
    ContractionHierarchy<Cost> contraction(graph);
    TransitNodeRouting<Cost> transit(graph, transit_nodes_for(graph));
    HubLabeling<Cost> labels(graph);
    compare_all_pairs(graph, arc_flags);
    compare_all_pairs(graph, reach);
    compare_all_pairs(graph, external_reach);
    for(int source = 0; source < n; ++source){
        for(int target = 0; target < n; ++target){
            const auto exact_lower_bound = [&graph](int vertex, int destination){
                return reference_distance(graph, vertex, destination)
                    .value_or(std::numeric_limits<Cost>::max());
            };
            assert(external_reach.query_distance(
                source, target, exact_lower_bound
            ) == reference_distance(graph, source, target));
        }
    }
    compare_all_pairs(graph, contraction);
    compare_all_pairs(graph, transit);
    compare_all_pairs(graph, labels);
}

void edge_cases(){
    check_graph(Graph(1));

    Graph directed(6);
    directed[0] = {{1, 8}, {1, 3}, {2, 20}};
    directed[1] = {{2, 0}, {3, 4}};
    directed[2] = {{3, 1}};
    directed[3] = {{1, 2}, {4, 7}};
    directed[4] = {};
    directed[5] = {{5, 0}};
    check_graph(directed);

    ArcFlagsRouting<Cost> flags(directed, {0, 0, 1, 1, 2, 2}, 4);
    assert(flags.vertex_count() == 6);
    assert(flags.region_count() == 4);
    assert(flags.region_of(4) == 2);
    bool some_flag = false;
    for(int region = 0; region < flags.region_count(); ++region){
        some_flag = some_flag || flags.arc_allows_region(0, 1, region);
    }
    assert(some_flag);

    ContractionHierarchy<Cost> contraction(
        directed, {5, 4, 3, 2, 1, 0}
    );
    assert(contraction.rank(5) == 0);

    TransitNodeRouting<Cost> transit(directed, {1, 4});
    assert(transit.transit_node_count() == 2);
    assert(transit.transit_nodes()[0] == 1);
    assert(!transit.forward_access_nodes(0).empty());
    assert(!transit.backward_access_nodes(4).empty());

    HubLabeling<Cost> labels(directed, {1, 3, 2, 0, 4, 5});
    assert(!labels.forward_labels(0).empty());
    assert(!labels.backward_labels(4).empty());

    bool threw = false;
    try{
        Graph invalid(2);
        invalid[0].push_back({1, -1});
        static_cast<void>(ReachBasedRouting<Cost>(invalid));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        static_cast<void>(ContractionHierarchy<Cost>(directed, {0, 1}));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        static_cast<void>(TransitNodeRouting<Cost>(directed, {1, 1}));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

void randomized_comparison(){
    std::mt19937_64 generator(0x9e3779b97f4a7c15ULL);
    for(int iteration = 0; iteration < 45; ++iteration){
        const int n = 1 + static_cast<int>(generator() % 8);
        Graph graph(static_cast<std::size_t>(n));
        for(int from = 0; from < n; ++from){
            for(int to = 0; to < n; ++to){
                if(generator() % 5 != 0) continue;
                graph[static_cast<std::size_t>(from)].emplace_back(
                    to, static_cast<Cost>(generator() % 12)
                );
                if(generator() % 7 == 0){
                    graph[static_cast<std::size_t>(from)].emplace_back(
                        to, static_cast<Cost>(generator() % 12)
                    );
                }
            }
        }
        check_graph(graph);
    }
}

Cost output_value(const std::optional<Cost>& value){
    return value.value_or(-1);
}

int main(){
    edge_cases();
    randomized_comparison();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int case_index = 0; case_index < test_count; ++case_index){
        int vertex_count;
        int edge_count;
        int source;
        int target;
        std::cin >> vertex_count >> edge_count >> source >> target;
        Graph graph(static_cast<std::size_t>(vertex_count));
        for(int edge_index = 0; edge_index < edge_count; ++edge_index){
            int from;
            int to;
            Cost cost;
            std::cin >> from >> to >> cost;
            graph[static_cast<std::size_t>(from)].emplace_back(to, cost);
        }
        ArcFlagsRouting<Cost> arc_flags(graph, regions_for(vertex_count));
        ReachBasedRouting<Cost> reach(graph);
    ReachBasedRouting<Cost> external_reach(
        graph, std::vector<Cost>(graph.size(), std::numeric_limits<Cost>::max())
    );
        ContractionHierarchy<Cost> contraction(graph);
        TransitNodeRouting<Cost> transit(graph, transit_nodes_for(graph));
        HubLabeling<Cost> labels(graph);
        std::cout
            << output_value(arc_flags.query_distance(source, target)) << ' '
            << output_value(reach.query_distance(source, target)) << ' '
            << output_value(contraction.query_distance(source, target)) << ' '
            << output_value(transit.query_distance(source, target)) << ' '
            << output_value(labels.query_distance(source, target)) << '\n';
    }
}
