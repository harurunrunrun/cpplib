#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_HIERARCHICAL_CLUSTERING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_HIERARCHICAL_CLUSTERING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::clustering {

enum class Linkage {
    single,
    complete,
    average,
};

struct HierarchicalMerge {
    std::size_t left = 0;
    std::size_t right = 0;
    long double distance = 0.0L;
    std::size_t size = 0;
};

struct HierarchicalClusteringResult {
    // Leaves are 0..n-1; merge i creates node n+i.
    std::vector<HierarchicalMerge> merges;
};

template<class Distance>
[[nodiscard]] HierarchicalClusteringResult agglomerative_clustering(
    std::size_t point_count,
    Distance distance,
    Linkage linkage
){
    if(point_count == 0) return {};
    if(linkage != Linkage::single && linkage != Linkage::complete &&
       linkage != Linkage::average){
        throw std::invalid_argument("unknown linkage kind");
    }
    if(point_count > (std::numeric_limits<std::size_t>::max() / 2) + 1){
        throw std::length_error("too many points for a dendrogram");
    }
    const std::size_t node_count = point_count * 2 - 1;
    const long double infinity = std::numeric_limits<long double>::infinity();
    std::vector<std::vector<long double>> cluster_distance(
        node_count, std::vector<long double>(node_count, infinity)
    );
    for(std::size_t left = 0; left < point_count; ++left){
        for(std::size_t right = left + 1; right < point_count; ++right){
            const long double value = detail::checked_distance(
                static_cast<long double>(std::invoke(distance, left, right))
            );
            cluster_distance[left][right] = value;
            cluster_distance[right][left] = value;
        }
    }
    std::vector<bool> active(node_count, false);
    std::fill(active.begin(), active.begin() +
              static_cast<std::ptrdiff_t>(point_count), true);
    std::vector<std::size_t> sizes(node_count, 0);
    std::fill(sizes.begin(), sizes.begin() +
              static_cast<std::ptrdiff_t>(point_count), std::size_t{1});

    HierarchicalClusteringResult result;
    result.merges.reserve(point_count - 1);
    for(std::size_t step = 0; step + 1 < point_count; ++step){
        std::size_t best_left = node_count;
        std::size_t best_right = node_count;
        long double best_distance = infinity;
        const std::size_t existing_nodes = point_count + step;
        for(std::size_t left = 0; left < existing_nodes; ++left){
            if(!active[left]) continue;
            for(std::size_t right = left + 1; right < existing_nodes; ++right){
                if(!active[right]) continue;
                const long double value = cluster_distance[left][right];
                if(value < best_distance ||
                   (value == best_distance &&
                    std::pair{left, right} < std::pair{best_left, best_right})){
                    best_distance = value;
                    best_left = left;
                    best_right = right;
                }
            }
        }
        const std::size_t merged = point_count + step;
        const std::size_t merged_size = sizes[best_left] + sizes[best_right];
        result.merges.push_back({
            best_left, best_right, best_distance, merged_size
        });
        active[best_left] = false;
        active[best_right] = false;
        active[merged] = true;
        sizes[merged] = merged_size;
        for(std::size_t other = 0; other < existing_nodes; ++other){
            if(!active[other]) continue;
            long double value = 0.0L;
            if(linkage == Linkage::single){
                value = std::min(
                    cluster_distance[best_left][other],
                    cluster_distance[best_right][other]
                );
            }else if(linkage == Linkage::complete){
                value = std::max(
                    cluster_distance[best_left][other],
                    cluster_distance[best_right][other]
                );
            }else{
                const long double right_weight =
                    static_cast<long double>(sizes[best_right]) /
                    static_cast<long double>(merged_size);
                value = std::lerp(
                    cluster_distance[best_left][other],
                    cluster_distance[best_right][other],
                    right_weight
                );
            }
            cluster_distance[merged][other] = value;
            cluster_distance[other][merged] = value;
        }
    }
    return result;
}

}  // namespace approximate::clustering

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_HIERARCHICAL_CLUSTERING_HPP_INCLUDED
