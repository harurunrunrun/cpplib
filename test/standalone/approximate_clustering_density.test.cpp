// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "../../src/approximate/clustering/density_clustering.hpp"

namespace {
bool self_test(){
    using approximate::clustering::DensePoints;
    const DensePoints<int> points{{0}, {1}, {9}, {10}};
    const auto shifted = approximate::clustering::mean_shift(
        points, 2.0L, 20, 0.0L, 0.0L
    );
    if(shifted.labels.size() != points.size() || shifted.modes.size() != 2) return false;
    const auto empty = approximate::clustering::dbscan(DensePoints<int>{}, 1.0L, 1);
    if(!empty.labels.empty() || empty.cluster_count != 0) return false;
    try{
        (void)approximate::clustering::dbscan(points, -1.0L, 2);
        return false;
    }catch(const std::invalid_argument&){
    }
    const long double large = std::numeric_limits<long double>::max();
    const auto far = approximate::clustering::dbscan(
        DensePoints<long double>{{-large}, {large}}, large, 2
    );
    if(far.cluster_count != 0 || far.labels.size() != 2 ||
       far.labels[0] != -1 || far.labels[1] != -1) return false;
    return true;
}
}  // namespace

int main(){
    if(!self_test()) return 1;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t point_count, dimension, minimum_points;
    long double epsilon;
    if(!(std::cin >> point_count >> dimension >> epsilon >> minimum_points)) return 1;
    approximate::clustering::DensePoints<long double> points(
        point_count, approximate::clustering::DensePoint<long double>(dimension)
    );
    for(auto& point: points) for(long double& coordinate: point) std::cin >> coordinate;
    const auto result = approximate::clustering::dbscan(points, epsilon, minimum_points);
    std::cout << result.cluster_count << '\n';
    for(std::size_t point = 0; point < result.labels.size(); ++point){
        if(point != 0) std::cout << ' ';
        std::cout << result.labels[point];
    }
    std::cout << '\n';
}
