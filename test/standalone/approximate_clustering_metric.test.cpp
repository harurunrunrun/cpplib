// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/clustering/facility_location.hpp"
#include "../../src/approximate/clustering/hierarchical_clustering.hpp"
#include "../../src/approximate/clustering/metric_k_center.hpp"

namespace {
bool self_test(){
    const long double large =
        std::numeric_limits<long double>::max() * 0.75L;
    try{
        (void)approximate::clustering::greedy_facility_location(
            1, 2, std::vector<long double>{0.0L},
            [large](std::size_t, std::size_t){ return large; }
        );
        return false;
    }catch(const std::overflow_error&){
    }
    const std::vector<std::vector<long double>> distance{
        {0.0L, 1.0L, large},
        {1.0L, 0.0L, large},
        {large, large, 0.0L},
    };
    const auto hierarchy = approximate::clustering::agglomerative_clustering(
        distance.size(),
        [&distance](std::size_t left, std::size_t right){
            return distance[left][right];
        },
        approximate::clustering::Linkage::average
    );
    return hierarchy.merges.size() == 2
        && std::isfinite(hierarchy.merges.back().distance);
}
}  // namespace

int main(){
    if(!self_test()) return 1;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int mode;
    if(!(std::cin >> mode)) return 1;
    std::cout << std::fixed << std::setprecision(6);
    if(mode == 0){
        std::size_t point_count, center_count, first_center;
        std::cin >> point_count >> center_count >> first_center;
        std::vector<std::vector<long double>> distance(
            point_count, std::vector<long double>(point_count)
        );
        for(auto& row: distance) for(long double& value: row) std::cin >> value;
        const auto result = approximate::clustering::gonzalez_k_center(
            point_count, center_count,
            [&distance](std::size_t left, std::size_t right){ return distance[left][right]; },
            first_center
        );
        std::cout << result.radius << '\n';
        for(std::size_t index = 0; index < result.centers.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << result.centers[index];
        }
        std::cout << '\n';
        for(std::size_t point = 0; point < result.labels.size(); ++point){
            if(point != 0) std::cout << ' ';
            std::cout << result.labels[point];
        }
        std::cout << '\n';
    }else if(mode >= 1 && mode <= 3){
        std::size_t point_count;
        std::cin >> point_count;
        std::vector<std::vector<long double>> distance(
            point_count, std::vector<long double>(point_count)
        );
        for(auto& row: distance) for(long double& value: row) std::cin >> value;
        const auto linkage = mode == 1 ? approximate::clustering::Linkage::single :
                             mode == 2 ? approximate::clustering::Linkage::complete :
                                         approximate::clustering::Linkage::average;
        const auto result = approximate::clustering::agglomerative_clustering(
            point_count,
            [&distance](std::size_t left, std::size_t right){ return distance[left][right]; },
            linkage
        );
        for(const auto& merge: result.merges){
            std::cout << merge.left << ' ' << merge.right << ' '
                      << merge.distance << ' ' << merge.size << '\n';
        }
    }else if(mode == 4){
        std::size_t facility_count, client_count, maximum_iterations;
        std::cin >> facility_count >> client_count >> maximum_iterations;
        std::vector<long double> opening_cost(facility_count);
        for(long double& value: opening_cost) std::cin >> value;
        std::vector<std::vector<long double>> distance(
            facility_count, std::vector<long double>(client_count)
        );
        for(auto& row: distance) for(long double& value: row) std::cin >> value;
        const auto result = approximate::clustering::facility_add_drop_local_search(
            facility_count, client_count, opening_cost,
            [&distance](std::size_t facility, std::size_t client){
                return distance[facility][client];
            }, maximum_iterations
        );
        std::cout << result.cost << ' ' << result.local_search_iterations << '\n';
        for(std::size_t index = 0; index < result.facilities.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << result.facilities[index];
        }
        std::cout << '\n';
        for(std::size_t client = 0; client < result.labels.size(); ++client){
            if(client != 0) std::cout << ' ';
            std::cout << result.labels[client];
        }
        std::cout << '\n';
    }else{
        return 1;
    }
}
