#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_GREEDY_MODULARITY_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_GREEDY_MODULARITY_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "community_detection.hpp"

namespace approximate::graph {

[[nodiscard]] inline CommunityResult greedy_modularity(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t maximum_merges = static_cast<std::size_t>(-1)
){
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> labels(vertex_count);
    std::iota(labels.begin(), labels.end(), 0);
    if(graph.total_edge_weight == 0.0L){
        return detail::make_community_result(vertex_count, edges, std::move(labels), 0);
    }
    std::vector<long double> degree_fraction(vertex_count, 0.0L);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        degree_fraction[vertex] = graph.normalized_degree[vertex] / 2.0L;
    }

    // Incremental CNM-style bookkeeping. For every pair of adjacent
    // communities the between fraction is the sum, in edge order, of the
    // edges joining them; it is rebuilt from the merged incidence list only
    // for the merged community, so every value (and hence every choice) is
    // exactly the one obtained by recomputing from all edges.
    struct PairState {
        long double between;
        long double gain;
    };
    struct Candidate {
        long double gain;
        std::size_t first;
        std::size_t second;
        bool operator<(const Candidate& other) const {
            if(gain != other.gain) return gain > other.gain;
            if(first != other.first) return first < other.first;
            return second < other.second;
        }
    };
    const std::size_t edge_count = graph.edges.size();
    std::vector<long double> term(edge_count);
    for(std::size_t index = 0; index < edge_count; ++index){
        term[index] = graph.edges[index].weight / graph.total_edge_weight;
    }
    std::vector<std::vector<std::size_t>> members(vertex_count);
    std::vector<std::vector<std::size_t>> incident(vertex_count);
    std::vector<std::map<std::size_t, PairState>> neighbor(vertex_count);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        members[vertex].push_back(vertex);
    }
    for(std::size_t index = 0; index < edge_count; ++index){
        const WeightedUndirectedEdge& edge = graph.edges[index];
        incident[edge.left].push_back(index);
        incident[edge.right].push_back(index);
        neighbor[edge.left][edge.right].between += term[index];
        neighbor[edge.right][edge.left].between += term[index];
    }
    const auto pair_gain = [&](std::size_t first, std::size_t second){
        return neighbor[first][second].between
            - 2.0L * degree_fraction[first] * degree_fraction[second];
    };
    std::set<Candidate> candidates;
    for(std::size_t first = 0; first < vertex_count; ++first){
        for(auto& [second, state] : neighbor[first]){
            if(second < first) continue;
            const long double gain = pair_gain(first, second);
            state.gain = gain;
            neighbor[second][first].gain = gain;
            candidates.insert(Candidate{gain, first, second});
        }
    }

    std::size_t merges = 0;
    while(merges < maximum_merges){
        if(candidates.empty() || candidates.begin()->gain <= 0.0L) break;
        const std::size_t kept = candidates.begin()->first;
        const std::size_t absorbed = candidates.begin()->second;
        for(const std::size_t community : {kept, absorbed}){
            for(const auto& [other, state] : neighbor[community]){
                candidates.erase(Candidate{
                    state.gain,
                    std::min(community, other),
                    std::max(community, other)
                });
                if(other != kept && other != absorbed){
                    neighbor[other].erase(community);
                }
            }
            neighbor[community].clear();
        }
        for(const std::size_t vertex : members[absorbed]){
            labels[vertex] = kept;
            members[kept].push_back(vertex);
        }
        members[absorbed].clear();
        std::vector<std::size_t> merged;
        merged.reserve(incident[kept].size() + incident[absorbed].size());
        std::merge(
            incident[kept].begin(), incident[kept].end(),
            incident[absorbed].begin(), incident[absorbed].end(),
            std::back_inserter(merged)
        );
        std::size_t write = 0;
        for(std::size_t read = 0; read < merged.size(); ++read){
            const std::size_t index = merged[read];
            if(read > 0 && merged[read - 1] == index) continue;
            const WeightedUndirectedEdge& edge = graph.edges[index];
            if(labels[edge.left] == labels[edge.right]) continue;
            merged[write++] = index;
            const std::size_t other = labels[edge.left] == kept
                ? labels[edge.right] : labels[edge.left];
            neighbor[kept][other].between += term[index];
        }
        merged.resize(write);
        incident[kept] = std::move(merged);
        std::vector<std::size_t>().swap(incident[absorbed]);
        degree_fraction[kept] += degree_fraction[absorbed];
        degree_fraction[absorbed] = 0.0L;
        for(auto& [other, state] : neighbor[kept]){
            const std::size_t first = std::min(kept, other);
            const std::size_t second = std::max(kept, other);
            const long double gain = state.between
                - 2.0L * degree_fraction[first] * degree_fraction[second];
            state.gain = gain;
            neighbor[other][kept] = state;
            candidates.insert(Candidate{gain, first, second});
        }
        ++merges;
    }
    return detail::make_community_result(vertex_count, edges, std::move(labels), merges);
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_GREEDY_MODULARITY_HPP_INCLUDED
