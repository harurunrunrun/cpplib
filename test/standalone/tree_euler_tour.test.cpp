// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/decomposition/euler_tour.hpp"

void self_test(){
    TreeEulerTour<8> tour(6);
    tour.add_edge(0, 1);
    tour.add_edge(0, 2);
    tour.add_edge(1, 3);
    tour.add_edge(1, 4);
    tour.add_edge(2, 5);
    tour.build(0);

    assert((tour.preorder() == std::vector<int>{0, 1, 3, 4, 2, 5}));
    assert(tour.parent(0) == -1);
    assert(tour.parent(4) == 1);
    assert(tour.depth(5) == 2);
    assert((tour.subtree_range(1) == std::pair<int, int>{1, 4}));
    assert(tour.subtree_size(2) == 2);
    assert(tour.is_ancestor(0, 5));
    assert(!tour.is_ancestor(1, 5));

    const auto events = tour.euler_tour();
    assert(events.size() == 12);
    assert(events.front().vertex == 0 && events.front().entering);
    assert(events.back().vertex == 0 && !events.back().entering);
    for(int vertex = 0; vertex < tour.size(); ++vertex){
        assert(tour.preorder_vertex(tour.tin(vertex)) == vertex);
    }

    TreeEulerTour<0> empty;
    empty.build();
    assert(empty.root() == -1);
    assert(empty.preorder().empty());
    assert(empty.euler_tour().empty());
}

#include "../../src/structure/graph/spanning_forest/incremental_minimum_spanning_forest.hpp"
#include "../../src/structure/graph/dynamic_connectivity/offline_dynamic_connectivity.hpp"
#include "../../src/algorithm/graph/connectivity/resilience/endpoint_failure_connectivity.hpp"
#include "../../src/algorithm/graph/traversal/functional_graph.hpp"
#include "../../src/algorithm/graph/spanning_tree/matrix_tree_theorem.hpp"
#include "../../src/algorithm/graph/traversal/counting_eulerian_circuits.hpp"
#include "../../src/algorithm/tree/construction/binary_tree_traversal_reconstruction.hpp"
#include "../../src/algorithm/tree/dynamic_programming/tree_edge_split_diameters.hpp"
#include "../../src/algorithm/other/utility/coordinate_compression.hpp"
#include "../../src/algorithm/other/utility/plane_coordinate_compression.hpp"
#include "../../src/algorithm/other/offline_query/grid_aggregation/multidimensional_prefix_sum.hpp"
#include "../../src/algorithm/other/offline_query/batch_framework/time_segment_tree.hpp"
#include "../../src/structure/modint/modint.hpp"

#include "../../src/algorithm/tree/decomposition/dsu_on_tree.hpp"
#include "../../src/algorithm/tree/construction/path_add_edge_get_tree.hpp"
#include "../../src/algorithm/tree/query/lca_subtree_distinct_colors.hpp"
#include "../../src/algorithm/tree/query/offline_tree_path_distinct.hpp"
#include "../../src/algorithm/tree/query/subtree_label_order_statistics.hpp"
#include "../../src/algorithm/tree/query/tree_path_kth_value.hpp"
#include "../../src/algorithm/tree/query/tree_path_value_membership.hpp"
#include "../../src/algorithm/tree/query/tree_path_minmax.hpp"
#include "../../src/algorithm/tree/query/weighted_tree_distance_kth_vertex.hpp"
#include "../../src/algorithm/range/static_query/static_range_count_distinct.hpp"
#include "../../src/algorithm/other/sequence/count_decreasing_triplets.hpp"
#include "../../src/algorithm/other/sequence/count_undominated_rank_triples.hpp"
#include "../../src/algorithm/other/scheduling/round_robin_completion_times.hpp"
#include "../../src/algorithm/other/sequence/reconstruct_permutation_from_larger_before.hpp"
#include "../../src/algorithm/other/sequence/cyclic_order_statistic_placement_permutation.hpp"
#include "../../src/algorithm/range/interval/count_visible_overlaid_intervals.hpp"
#include "../../src/algorithm/range/offline_query/count_unblocked_height_queries.hpp"
#include "../../src/algorithm/range/offline_query/maximum_distinct_subarray_sums.hpp"

void dynamic_capacity_graph_test(){
    TreeEulerTour<> dynamic_tour(3); dynamic_tour.add_edge(0,1); dynamic_tour.add_edge(1,2); dynamic_tour.build(); assert(dynamic_tour.depth(2)==2);
    DsuOnTree<> dynamic_dsu_tree(3); dynamic_dsu_tree.add_edge(0,1); dynamic_dsu_tree.add_edge(1,2); dynamic_dsu_tree.build(); assert(dynamic_dsu_tree.subtree_size(1)==2);
    PathAddEdgeGetTree<long long> path_add(3); path_add.add_edge(0,1); path_add.add_edge(1,2); path_add.build(); path_add.path_add(0,2,7); assert(path_add.edge_get(0,1)==7);
    LcaSubtreeDistinctColors<int> colors({1,2,1}); colors.add_edge(0,1); colors.add_edge(0,2); colors.build(0); assert(colors.count_at_lca_subtree(1,2)==2);
    OfflineTreePathDistinct<int> distinct_path({1,2,1}); distinct_path.add_edge(0,1); distinct_path.add_edge(1,2); distinct_path.build(); distinct_path.add_query(0,2); assert((distinct_path.solve()==std::vector<int>{2}));
    SubtreeLabelOrderStatistics<> subtree_labels({3,1,2}); subtree_labels.add_edge(0,1); subtree_labels.add_edge(0,2); subtree_labels.build(); assert(subtree_labels.kth_smallest_vertex(0,0)==1);
    TreePathKthValue<int> kth(3); kth.add_edge(0,1); kth.add_edge(1,2); kth.build({3,1,2}); assert(kth.kth_smallest(0,2,1)==2);
    TreePathValueMembership<int> member(3); member.add_edge(0,1); member.add_edge(1,2); member.build({3,1,2}); assert(member.contains(0,2,2));
    TreePathMinmax<long long> extrema(3); extrema.add_edge(0,1,2); extrema.add_edge(1,2,5); extrema.build(); assert((extrema.path_minmax(0,2)==std::pair<long long,long long>{2,5}));
    WeightedTreeDistanceKthVertex<long long> distance(3); distance.add_edge(0,1,2); distance.add_edge(1,2,5); distance.build(); assert(distance.distance(0,2)==7 && distance.kth_vertex(0,2,1)==1);
    StaticRangeCountDistinct<int> distinct({1,2,1}); assert(distinct.count(0,3)==2);
    assert(count_decreasing_triplets(std::vector<int>{3,2,1})==1);
    assert(count_undominated_rank_triples(std::vector<std::array<int,3>>{{0,0,0},{1,1,1}})==1);
    assert((round_robin_completion_times(std::vector<long long>{1,2})==std::vector<long long>{1,3}));
    assert((reconstruct_permutation_from_larger_before(std::vector<unsigned int>{0,1,0})==std::vector<int>{2,1,3}));
    assert(cyclic_order_statistic_placement_permutation(5)==cyclic_order_statistic_placement_permutation<8>(5));
    assert(count_visible_overlaid_intervals(std::vector<std::pair<int,int>>{{0,2},{1,3}})==2);
    assert(count_unblocked_height_queries(std::vector<int>{3,1,2},{{0,2}})==1);
    assert((maximum_distinct_subarray_sums(std::vector<long long>{1,2,1},{{0,3}})==maximum_distinct_subarray_sums<long long,8>(std::vector<long long>{1,2,1},{{0,3}})));

IncrementalMinimumSpanningForest<long long> forest(3);assert(forest.add_edge(0,1,5)==-1);assert(forest.add_edge(1,2,3)==-1);assert(forest.add_edge(0,2,2)==0);assert(!forest.contains_edge(0));assert(forest.contains_edge(2));
 OfflineDynamicConnectivity<> dc(3,4);dc.add_edge(0,0,1);dc.add_edge(1,1,2);dc.erase_edge(3,0,1);dc.run([](int t,auto& d){assert(d.same(0,2)==(t==1||t==2));});
 EndpointFailureConnectivity<> ef(3,{{0,1},{1,2}});assert(!ef.connected_without_vertex(0,2,1));
 FunctionalGraph<> fg(std::vector<int>{1,2,1});assert(fg.jump(0,3)==1);
 using mint=Modint998244353;std::vector<MatrixTreeEdge<mint>> edges{{0,1,1},{1,2,1},{2,0,1}};assert(count_weighted_undirected_spanning_trees(3,edges)==mint(3));assert(count_weighted_directed_in_arborescences(3,0,edges)==mint(1));assert(count_eulerian_circuits<mint>(3,{{0,1},{1,2},{2,0}})==mint(1));
 assert(reconstruct_binary_tree_traversals(std::vector<int>{1,2,3},std::vector<int>{2,1,3},std::vector<int>{2,3,1}));
 auto diameters=tree_edge_split_diameters(4,{{0,1},{1,2},{2,3}});assert(diameters[1].first_diameter==1&&diameters[1].second_diameter==1);
 CoordinateCompression<int> cc({3,1,3});assert(cc.index(3)==1);PlaneCoordinateCompression<int,int> pc({3,1},{5,2});assert(pc.x_index(3)==1);
 MultidimensionalPrefixSum<long long,2> grid({2,3});grid.set({1,2},7);grid.build();assert(grid.range_sum({0,0},{2,3})==7);
 TimeSegmentTree<int> time(4);time.add_interval(0,2,3);time.add_interval(2,4,5);int sum=0;std::vector<int> stack;time.run([&](int x){stack.push_back(sum);sum+=x;},[&](){return stack.size();},[&](std::size_t s){while(stack.size()>s){sum=stack.back();stack.pop_back();}},[&](int t){assert(sum==(t<2?3:5));});
}

int main(){
    dynamic_capacity_graph_test();
    int n, root;
    if(!(std::cin >> n >> root)){
        self_test();
        return 0;
    }

    TreeEulerTour<256> tour(n);
    for(int edge = 0; edge + 1 < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tour.add_edge(u, v);
    }
    tour.build(root);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "PARENT"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.parent(vertex) << '\n';
        }else if(type == "DEPTH"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.depth(vertex) << '\n';
        }else if(type == "TIN"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.tin(vertex) << '\n';
        }else if(type == "TOUT"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.tout(vertex) << '\n';
        }else if(type == "RANGE"){
            int vertex;
            std::cin >> vertex;
            const auto [left, right] = tour.subtree_range(vertex);
            std::cout << left << ' ' << right << '\n';
        }else if(type == "SUBSIZE"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.subtree_size(vertex) << '\n';
        }else if(type == "ANCESTOR"){
            int ancestor, descendant;
            std::cin >> ancestor >> descendant;
            std::cout << tour.is_ancestor(ancestor, descendant) << '\n';
        }else if(type == "PREORDER_VERTEX"){
            int index;
            std::cin >> index;
            std::cout << tour.preorder_vertex(index) << '\n';
        }else if(type == "PREORDER"){
            const auto order = tour.preorder();
            std::cout << order.size();
            for(int vertex : order) std::cout << ' ' << vertex;
            std::cout << '\n';
        }else if(type == "EULER_SIZE"){
            std::cout << tour.euler_size() << '\n';
        }else if(type == "EVENT"){
            int index;
            std::cin >> index;
            const auto event = tour.euler_event(index);
            std::cout
                << event.vertex << ' '
                << event.entering << '\n';
        }else if(type == "EULER"){
            const auto events = tour.euler_tour();
            std::cout << events.size();
            for(const auto event : events){
                std::cout
                    << ' ' << event.vertex
                    << ' ' << event.entering;
            }
            std::cout << '\n';
        }else if(type == "ROOT"){
            std::cout << tour.root() << '\n';
        }else if(type == "SIZE"){
            std::cout << tour.size() << '\n';
        }else if(type == "EDGES"){
            std::cout << tour.edge_count() << '\n';
        }
    }
}
