// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/chinese_whispers.hpp"
#include "../../src/approximate/graph/community_detection.hpp"
#include "../../src/approximate/graph/greedy_modularity.hpp"
#include "../../src/approximate/graph/label_propagation.hpp"
#include "../../src/approximate/graph/louvain.hpp"
#include "../../src/approximate/graph/weighted_undirected_graph.hpp"

namespace {

using approximate::graph::CommunityResult;
using approximate::graph::WeightedUndirectedEdge;

void require(bool condition){
    if(!condition) throw std::runtime_error("community detection test failed");
}

bool close(long double left, long double right){
    const long double scale = std::max({1.0L, std::abs(left), std::abs(right)});
    return std::abs(left - right) <= 1.0e-10L * scale;
}

long double reference_modularity(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    const std::vector<std::size_t>& labels
){
    long double total = 0.0L;
    std::vector<long double> degree(vertex_count, 0.0L);
    for(const auto& edge : edges){
        if(edge.left == edge.right || edge.weight == 0.0L) continue;
        total += edge.weight;
        degree[edge.left] += edge.weight;
        degree[edge.right] += edge.weight;
    }
    if(total == 0.0L) return 0.0L;
    const auto canonical = approximate::graph::canonical_community_labels(labels);
    std::size_t count = 0;
    for(const std::size_t label : canonical) count = std::max(count, label + 1);
    std::vector<long double> degree_sum(count, 0.0L);
    std::vector<long double> internal(count, 0.0L);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        degree_sum[canonical[vertex]] += degree[vertex];
    }
    for(const auto& edge : edges){
        if(edge.left != edge.right && edge.weight != 0.0L && canonical[edge.left] == canonical[edge.right]){
            internal[canonical[edge.left]] += edge.weight;
        }
    }
    long double result = 0.0L;
    for(std::size_t community = 0; community < count; ++community){
        const long double fraction = degree_sum[community] / (2.0L * total);
        result += internal[community] / total - fraction * fraction;
    }
    return result;
}

void check_result(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    const CommunityResult& result
){
    require(result.label.size() == vertex_count);
    require(result.label == approximate::graph::canonical_community_labels(result.label));
    std::size_t count = 0;
    for(const std::size_t label : result.label) count = std::max(count, label + 1);
    require(result.community_count == count);
    require(close(result.modularity, reference_modularity(vertex_count, edges, result.label)));
    require(close(result.modularity, approximate::graph::community_modularity(vertex_count, edges, result.label)));
}

void check_planted(const CommunityResult& result, std::size_t group_count){
    if(group_count == 0) return;
    require(result.label.size() % group_count == 0);
    const std::size_t group_size = result.label.size() / group_count;
    for(std::size_t group = 0; group < group_count; ++group){
        const std::size_t first = group * group_size;
        for(std::size_t offset = 1; offset < group_size; ++offset){
            require(result.label[first + offset] == result.label[first]);
        }
        for(std::size_t previous = 0; previous < group; ++previous){
            require(result.label[first] != result.label[previous * group_size]);
        }
    }
}

template<class Exception, class Function>
void require_throws(Function&& function){
    bool thrown = false;
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        thrown = true;
    }
    require(thrown);
}

void boundary_tests(){
    using namespace approximate::graph;
    require(canonical_community_labels({9, 2, 9, 7}) == std::vector<std::size_t>({0, 1, 0, 2}));
    require_throws<std::invalid_argument>([]{
        static_cast<void>(community_modularity(2, {}, {0}));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(label_propagation(2, {{0, 1, -1.0L}}));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(greedy_modularity(
            2, {{0, 1, std::numeric_limits<long double>::quiet_NaN()}}
        ));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(louvain_communities(2, {{0, 2, 1.0L}}, 1));
    });
    const long double maximum = std::numeric_limits<long double>::max();
    require_throws<std::overflow_error>([maximum]{
        static_cast<void>(community_modularity(
            2, {{0, 1, maximum * 0.75L}, {1, 0, maximum * 0.75L}}, {0, 1}
        ));
    });
    require(label_propagation(0, {}).label.empty());
    require(chinese_whispers(0, {}, 1).label.empty());
    require(greedy_modularity(0, {}).label.empty());
    require(louvain_communities(0, {}, 1).label.empty());
    require(close(community_modularity(2, {{0, 0, maximum}}, {0, 1}), 0.0L));
    const std::vector<WeightedUndirectedEdge> large_weights{
        {0, 1, maximum / 4.0L}, {1, 2, maximum / 8.0L}, {2, 3, maximum / 8.0L}
    };
    require(std::isfinite(label_propagation(4, large_weights).modularity));
    require(std::isfinite(chinese_whispers(4, large_weights, 1).modularity));
    require(std::isfinite(greedy_modularity(4, large_weights).modularity));
    require(std::isfinite(louvain_communities(4, large_weights, 1).modularity));
}

}  // namespace

int main(){
    using namespace approximate::graph;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t case_count = 0;
    std::cin >> case_count;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t identifier = 0;
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::size_t planted_groups = 0;
        std::cin >> identifier >> vertex_count >> edge_count >> planted_groups;
        std::vector<WeightedUndirectedEdge> edges(edge_count);
        for(auto& edge : edges) std::cin >> edge.left >> edge.right >> edge.weight;

        const CommunityResult propagation = label_propagation(vertex_count, edges, 100);
        const CommunityResult whispers = chinese_whispers(vertex_count, edges, 123456789ULL, 100);
        const CommunityResult greedy = greedy_modularity(vertex_count, edges);
        const CommunityResult louvain = louvain_communities(vertex_count, edges, 987654321ULL, 20, 100);
        check_result(vertex_count, edges, propagation);
        check_result(vertex_count, edges, whispers);
        check_result(vertex_count, edges, greedy);
        check_result(vertex_count, edges, louvain);
        check_planted(propagation, planted_groups);
        check_planted(whispers, planted_groups);
        check_planted(greedy, planted_groups);
        check_planted(louvain, planted_groups);

        std::vector<std::size_t> singleton(vertex_count);
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex) singleton[vertex] = vertex;
        const long double singleton_modularity = community_modularity(vertex_count, edges, singleton);
        require(greedy.modularity >= singleton_modularity || close(greedy.modularity, singleton_modularity));
        require(louvain.modularity >= singleton_modularity || close(louvain.modularity, singleton_modularity));
        require(label_propagation(vertex_count, edges, 100).label == propagation.label);
        require(chinese_whispers(vertex_count, edges, 123456789ULL, 100).label == whispers.label);
        require(greedy_modularity(vertex_count, edges).label == greedy.label);
        require(louvain_communities(vertex_count, edges, 987654321ULL, 20, 100).label == louvain.label);
        std::cout << "OK " << identifier << '\n';
    }
}
