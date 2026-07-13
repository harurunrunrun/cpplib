// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>

#include "../../src/approximate/clustering/kmeans.hpp"

namespace {
bool self_test(){
    using approximate::clustering::DensePoints;
    const DensePoints<int> points{{0, 0}, {0, 1}, {9, 9}, {10, 9}};
    std::mt19937 first_random(123456U), second_random(123456U);
    const auto first = approximate::clustering::k_means(points, 2, first_random, 30);
    const auto second = approximate::clustering::k_means(points, 2, second_random, 30);
    if(first.centers != second.centers || first.labels != second.labels) return false;
    std::mt19937 mini_random(7U);
    const auto mini = approximate::clustering::mini_batch_k_means(
        points, 2, 3, 20, mini_random
    );
    if(mini.centers.size() != 2 || mini.labels.size() != points.size() ||
       !std::isfinite(mini.squared_error)) return false;
    try{
        (void)approximate::clustering::lloyd_k_means(
            DensePoints<int>{{0}, {1, 2}}, DensePoints<long double>{{0.0L}}
        );
        return false;
    }catch(const std::invalid_argument&){
    }
    const long double large = std::sqrt(
        std::numeric_limits<long double>::max() * 0.75L
    );
    try{
        (void)approximate::clustering::lloyd_k_means(
            DensePoints<long double>{{large}, {large}},
            DensePoints<long double>{{0.0L}}, 0
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
    std::size_t point_count, dimension, cluster_count, maximum_iterations;
    if(!(std::cin >> point_count >> dimension >> cluster_count >> maximum_iterations)) return 1;
    approximate::clustering::DensePoints<long double> points(
        point_count, approximate::clustering::DensePoint<long double>(dimension)
    );
    for(auto& point: points) for(long double& coordinate: point) std::cin >> coordinate;
    approximate::clustering::DensePoints<long double> centers(
        cluster_count, approximate::clustering::DensePoint<long double>(dimension)
    );
    for(auto& center: centers) for(long double& coordinate: center) std::cin >> coordinate;
    const auto result = approximate::clustering::lloyd_k_means(
        points, std::move(centers), maximum_iterations, 0.0L
    );
    std::cout << result.iterations << ' ' << std::fixed << std::setprecision(6)
              << result.squared_error << '\n';
    for(const auto& center: result.centers){
        for(std::size_t coordinate = 0; coordinate < center.size(); ++coordinate){
            if(coordinate != 0) std::cout << ' ';
            std::cout << center[coordinate];
        }
        std::cout << '\n';
    }
    for(std::size_t point = 0; point < result.labels.size(); ++point){
        if(point != 0) std::cout << ' ';
        std::cout << result.labels[point];
    }
    std::cout << '\n';
}
