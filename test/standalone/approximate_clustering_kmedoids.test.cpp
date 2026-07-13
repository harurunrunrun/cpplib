// competitive-verifier: STANDALONE

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/clustering/kmedoids.hpp"

namespace {
bool self_test(){
    const std::vector<std::vector<long double>> distance{
        {0, 1, 10, 11}, {1, 0, 9, 10}, {10, 9, 0, 1}, {11, 10, 1, 0}
    };
    const auto metric = [&distance](std::size_t left, std::size_t right){
        return distance[left][right];
    };
    std::mt19937 clara_random(91U);
    const auto clara = approximate::clustering::clara_k_medoids(
        distance.size(), 2, metric, 4, 3, clara_random
    );
    if(clara.cost != 2.0L || clara.medoids.size() != 2) return false;
    std::mt19937 clarans_random(19U);
    const auto clarans = approximate::clustering::clarans_k_medoids(
        distance.size(), 2, metric, 20, 3, clarans_random
    );
    if(clarans.cost > 2.0L || clarans.labels.size() != distance.size()) return false;
    std::mt19937 all_random(1U);
    const auto all = approximate::clustering::clarans_k_medoids(
        distance.size(), distance.size(), metric, 1, 1, all_random
    );
    if(all.cost != 0.0L || all.medoids.size() != distance.size()) return false;
    try{
        (void)approximate::clustering::pam_k_medoids(3, 0, metric);
        return false;
    }catch(const std::invalid_argument&){
    }
    const long double large =
        std::numeric_limits<long double>::max() * 0.75L;
    try{
        (void)approximate::clustering::pam_k_medoids(
            3, 1,
            [large](std::size_t left, std::size_t right){
                return left == right ? 0.0L : large;
            },
            0
        );
        return false;
    }catch(const std::overflow_error&){
    }
    return true;
}
}  // namespace

int main(){
    if(!self_test()) return 1;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t point_count, cluster_count, maximum_iterations;
    if(!(std::cin >> point_count >> cluster_count >> maximum_iterations)) return 1;
    std::vector<std::vector<long double>> distance(
        point_count, std::vector<long double>(point_count)
    );
    for(auto& row: distance) for(long double& value: row) std::cin >> value;
    const auto result = approximate::clustering::pam_k_medoids(
        point_count, cluster_count,
        [&distance](std::size_t left, std::size_t right){
            return distance[left][right];
        },
        maximum_iterations
    );
    std::cout << result.iterations << ' ' << std::fixed << std::setprecision(6)
              << result.cost << '\n';
    for(std::size_t index = 0; index < result.medoids.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << result.medoids[index];
    }
    std::cout << '\n';
    for(std::size_t point = 0; point < result.labels.size(); ++point){
        if(point != 0) std::cout << ' ';
        std::cout << result.labels[point];
    }
    std::cout << '\n';
}
