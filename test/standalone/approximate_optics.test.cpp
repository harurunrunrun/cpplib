// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/clustering/optics.hpp"

namespace {

void require(bool condition){
    if(!condition) throw std::runtime_error("OPTICS test failed");
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
    using namespace approximate::clustering;
    require(optics(DensePoints<int>{}, 1.0L, 1).ordering.empty());
    require_throws<std::invalid_argument>([]{
        static_cast<void>(optics(DensePoints<int>{{0}, {1}}, -1.0L, 1));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(optics(DensePoints<int>{{0}, {1}}, 1.0L, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(optics(DensePoints<double>{{0.0}, {std::numeric_limits<double>::quiet_NaN()}}, 1.0L, 1));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(optics(DensePoints<int>{{0}, {1, 2}}, 1.0L, 1));
    });
    const long double maximum = std::numeric_limits<long double>::max();
    const auto large = optics(DensePoints<long double>{{0.0L}, {maximum}}, maximum, 2);
    require(large.ordering == std::vector<std::size_t>({0, 1}));
    require(std::isfinite(large.core_distance[0]));
    require_throws<std::overflow_error>([maximum]{
        static_cast<void>(optics(DensePoints<long double>{{-maximum}, {maximum}}, maximum, 2));
    });
}

}  // namespace

int main(){
    using namespace approximate::clustering;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t point_count = 0;
    std::size_t dimension = 0;
    std::size_t minimum_points = 0;
    long double maximum_radius = 0.0L;
    std::cin >> point_count >> dimension >> maximum_radius >> minimum_points;
    DensePoints<long double> points(point_count, DensePoint<long double>(dimension));
    for(auto& point : points) for(long double& coordinate : point) std::cin >> coordinate;
    const OPTICSResult result = optics(points, maximum_radius, minimum_points);
    require(result.ordering.size() == point_count);
    require(result.reachability_distance.size() == point_count);
    require(result.core_distance.size() == point_count);
    std::vector<std::size_t> sorted = result.ordering;
    std::sort(sorted.begin(), sorted.end());
    for(std::size_t point = 0; point < point_count; ++point) require(sorted[point] == point);
    std::cout << result.ordering.size();
    for(const std::size_t point : result.ordering) std::cout << ' ' << point;
    std::cout << '\n';
}
