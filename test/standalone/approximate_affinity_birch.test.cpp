// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/clustering/affinity_propagation.hpp"
#include "../../src/approximate/clustering/birch_features.hpp"

namespace {

void require(bool condition){
    if(!condition) throw std::runtime_error("affinity/BIRCH test failed");
}

bool close(long double left, long double right){
    const long double scale = std::max({1.0L, std::abs(left), std::abs(right)});
    return std::abs(left - right) <= 1.0e-10L * scale;
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
    require(affinity_propagation(DensePoints<int>{}).labels.empty());
    const auto singleton = affinity_propagation(DensePoints<int>{{7}});
    require(singleton.exemplars == std::vector<std::size_t>({0}));
    require(singleton.labels == std::vector<std::size_t>({0}));
    const auto all_exemplars = affinity_propagation(
        DensePoints<int>{{0}, {3}, {9}}, 0.0L, 0.5L, 100, 5
    );
    require(all_exemplars.exemplars.size() == 3);
    const auto no_iterations = affinity_propagation(
        DensePoints<int>{{0}, {1}}, std::nullopt, 0.5L, 0, 2
    );
    require(no_iterations.iterations == 0 && !no_iterations.converged);
    const auto no_false_convergence = affinity_propagation(
        DensePoints<int>{{0}, {1}}, -1000.0L, 0.99L, 3, 2
    );
    require(no_false_convergence.iterations == 3 && !no_false_convergence.converged);
    const DensePoints<int> even_similarity_count{{0}, {1}, {4}, {10}};
    const auto implicit_median = affinity_propagation(even_similarity_count);
    const auto explicit_median = affinity_propagation(even_similarity_count, -26.0L);
    require(implicit_median.exemplars == explicit_median.exemplars);
    require(implicit_median.labels == explicit_median.labels);
    require_throws<std::invalid_argument>([]{
        static_cast<void>(affinity_propagation(DensePoints<int>{{0}, {1}}, std::nullopt, 1.0L));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(affinity_propagation(DensePoints<int>{{0}, {1}}, std::nullopt, 0.5L, 10, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(affinity_propagation(
            DensePoints<int>{{0}, {1}}, std::numeric_limits<long double>::infinity()
        ));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(affinity_propagation(DensePoints<double>{{0.0}, {
            std::numeric_limits<double>::quiet_NaN()
        }}));
    });
    const long double maximum = std::numeric_limits<long double>::max();
    require_throws<std::overflow_error>([maximum]{
        static_cast<void>(affinity_propagation(DensePoints<long double>{{0.0L}, {maximum}}));
    });

    require(birch_clustering_features(DensePoints<int>{}, 1.0L, 1).features.empty());
    require_throws<std::invalid_argument>([]{
        static_cast<void>(birch_clustering_features(DensePoints<int>{{0}}, -1.0L, 1));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(birch_clustering_features(DensePoints<int>{{0}}, 1.0L, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(birch_clustering_features(DensePoints<int>{{0}, {1, 2}}, 1.0L, 2));
    });
    const auto identical = birch_clustering_features(
        DensePoints<long double>{{maximum}, {maximum}}, 0.0L, 1
    );
    require(identical.features.size() == 1 && identical.features[0].count == 2);
}

template<class Real>
void check_results(
    const approximate::clustering::DensePoints<Real>& points,
    long double threshold,
    std::size_t maximum_features
){
    using namespace approximate::clustering;
    const auto affinity = affinity_propagation(points, std::nullopt, 0.6L, 120, 12);
    const auto affinity_again = affinity_propagation(points, std::nullopt, 0.6L, 120, 12);
    require(affinity.exemplars == affinity_again.exemplars);
    require(affinity.labels == affinity_again.labels);
    require(affinity.iterations <= 120);
    require(affinity.labels.size() == points.size());
    require(std::is_sorted(affinity.exemplars.begin(), affinity.exemplars.end()));
    for(std::size_t exemplar_index = 0; exemplar_index < affinity.exemplars.size(); ++exemplar_index){
        require(affinity.labels[affinity.exemplars[exemplar_index]] == exemplar_index);
    }
    for(const std::size_t label : affinity.labels) require(label < affinity.exemplars.size());

    const BirchFeatureResult birch = birch_clustering_features(points, threshold, maximum_features);
    const BirchFeatureResult birch_again = birch_clustering_features(points, threshold, maximum_features);
    require(birch.labels == birch_again.labels);
    require(birch.features.size() == birch_again.features.size());
    require(birch.labels.size() == points.size());
    require(birch.features.size() <= maximum_features);
    std::size_t total_count = 0;
    for(const ClusteringFeature& feature : birch.features){
        total_count += feature.count;
        require(feature.centroid.size() == points.front().size());
        require(feature.coordinate_variance.size() == points.front().size());
        long double radius = 0.0L;
        for(const long double variance : feature.coordinate_variance){
            require(std::isfinite(variance) && variance >= 0.0L);
            radius += variance;
        }
        require(close(radius, feature.squared_radius));
    }
    require(total_count == points.size());
    for(const std::size_t label : birch.labels) require(label < birch.features.size());
}

}  // namespace

int main(){
    using namespace approximate::clustering;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t point_count = 0;
    std::size_t dimension = 0;
    std::size_t maximum_features = 0;
    long double threshold = 0.0L;
    std::cin >> point_count >> dimension >> threshold >> maximum_features;
    DensePoints<long double> points(point_count, DensePoint<long double>(dimension));
    for(auto& point : points) for(long double& coordinate : point) std::cin >> coordinate;
    if(points.empty()){
        const auto birch = birch_clustering_features(points, threshold, maximum_features);
        std::cout << "0\n";
        return birch.features.empty() && birch.labels.empty() ? 0 : 1;
    }
    check_results(points, threshold, maximum_features);
    const BirchFeatureResult birch = birch_clustering_features(points, threshold, maximum_features);
    std::cout << birch.features.size();
    for(const ClusteringFeature& feature : birch.features) std::cout << ' ' << feature.count;
    for(const std::size_t label : birch.labels) std::cout << ' ' << label;
    std::cout << '\n';
}
