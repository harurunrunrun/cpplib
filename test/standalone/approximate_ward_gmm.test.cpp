// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/clustering/gaussian_mixture.hpp"
#include "../../src/approximate/clustering/ward_linkage.hpp"

namespace {

void require(bool condition){
    if(!condition) throw std::runtime_error("Ward/GMM test failed");
}

bool close(long double left, long double right){
    const long double scale = std::max({1.0L, std::abs(left), std::abs(right)});
    return std::abs(left - right) <= 1.0e-9L * scale;
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
    require(ward_linkage(DensePoints<int>{}).merges.empty());
    require_throws<std::invalid_argument>([]{
        static_cast<void>(ward_linkage(DensePoints<int>{{0}, {1, 2}}));
    });
    require_throws<std::invalid_argument>([]{
        std::mt19937_64 generator(1);
        static_cast<void>(gaussian_mixture_em(DensePoints<int>{}, 1, generator));
    });
    require_throws<std::invalid_argument>([]{
        std::mt19937_64 generator(1);
        static_cast<void>(gaussian_mixture_em(DensePoints<int>{{0}}, 0, generator));
    });
    require_throws<std::invalid_argument>([]{
        std::mt19937_64 generator(1);
        static_cast<void>(gaussian_mixture_em(DensePoints<int>{{0}}, 1, generator, 1, -1.0L));
    });
    require_throws<std::invalid_argument>([]{
        std::mt19937_64 generator(1);
        static_cast<void>(gaussian_mixture_em(
            DensePoints<double>{{std::numeric_limits<double>::quiet_NaN()}}, 1, generator
        ));
    });
    require_throws<std::invalid_argument>([]{
        std::mt19937_64 generator(1);
        static_cast<void>(gaussian_mixture_em(DensePoints<int>{{0}, {1, 2}}, 1, generator));
    });
    const long double maximum = std::numeric_limits<long double>::max();
    require_throws<std::overflow_error>([maximum]{
        static_cast<void>(ward_linkage(DensePoints<long double>{{0.0L}, {maximum}}));
    });
    require_throws<std::overflow_error>([maximum]{
        std::mt19937_64 generator(1);
        static_cast<void>(gaussian_mixture_em(
            DensePoints<long double>{{-maximum}, {maximum}}, 1, generator
        ));
    });

    const DensePoints<int> separated{{-11}, {-10}, {-9}, {9}, {10}, {11}};
    std::mt19937_64 generator(123);
    const auto mixture = gaussian_mixture_em(separated, 2, generator, 100, 1.0e-10L, 1.0e-6L);
    require(mixture.labels[0] == mixture.labels[1] && mixture.labels[1] == mixture.labels[2]);
    require(mixture.labels[3] == mixture.labels[4] && mixture.labels[4] == mixture.labels[5]);
    require(mixture.labels[0] != mixture.labels[3]);
    std::mt19937_64 zero_iteration_generator(7);
    const auto zero_iteration = gaussian_mixture_em(
        separated, 2, zero_iteration_generator, 0, 1.0e-10L, 1.0e-6L
    );
    require(zero_iteration.iterations == 0 && zero_iteration.labels.size() == separated.size());
}

template<class Real>
void check_mixture(
    const approximate::clustering::DensePoints<Real>& points,
    std::size_t component_count,
    std::uint64_t seed
){
    using namespace approximate::clustering;
    std::mt19937_64 first_generator(seed);
    std::mt19937_64 second_generator(seed);
    const auto first = gaussian_mixture_em(points, component_count, first_generator, 80, 1.0e-8L, 1.0e-7L);
    const auto second = gaussian_mixture_em(points, component_count, second_generator, 80, 1.0e-8L, 1.0e-7L);
    require(first.weights == second.weights);
    require(first.means == second.means);
    require(first.variances == second.variances);
    require(first.labels == second.labels);
    require(first.iterations <= 80);
    require(first.weights.size() == component_count);
    require(first.means.size() == component_count);
    require(first.variances.size() == component_count);
    require(first.responsibilities.size() == points.size());
    require(first.labels.size() == points.size());
    long double weight_sum = 0.0L;
    for(std::size_t component = 0; component < component_count; ++component){
        require(first.weights[component] > 0.0L && std::isfinite(first.weights[component]));
        weight_sum += first.weights[component];
        require(first.means[component].size() == points.front().size());
        require(first.variances[component].size() == points.front().size());
        for(const long double variance : first.variances[component]){
            require(std::isfinite(variance) && variance >= 1.0e-7L);
        }
    }
    require(close(weight_sum, 1.0L));
    require(std::isfinite(first.log_likelihood));
    for(std::size_t point = 0; point < points.size(); ++point){
        require(first.responsibilities[point].size() == component_count);
        long double sum = 0.0L;
        for(const long double value : first.responsibilities[point]){
            require(std::isfinite(value) && value >= 0.0L && value <= 1.0L);
            sum += value;
        }
        require(close(sum, 1.0L));
        const std::size_t best = static_cast<std::size_t>(std::max_element(
            first.responsibilities[point].begin(), first.responsibilities[point].end()
        ) - first.responsibilities[point].begin());
        require(first.labels[point] == best);
    }
}

}  // namespace

int main(){
    using namespace approximate::clustering;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t point_count = 0;
    std::size_t dimension = 0;
    std::size_t component_count = 0;
    std::uint64_t seed = 0;
    std::cin >> point_count >> dimension >> component_count >> seed;
    DensePoints<long double> points(point_count, DensePoint<long double>(dimension));
    for(auto& point : points) for(long double& coordinate : point) std::cin >> coordinate;
    const WardClusteringResult ward = ward_linkage(points);
    check_mixture(points, component_count, seed);
    std::cout << ward.merges.size();
    for(const WardMerge& merge : ward.merges){
        std::cout << ' ' << merge.left << ' ' << merge.right << ' ' << merge.size;
    }
    std::cout << '\n';
}
