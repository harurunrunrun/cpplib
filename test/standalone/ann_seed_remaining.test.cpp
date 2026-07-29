// competitive-verifier: STANDALONE

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <span>
#include <string>
#include <vector>

#include "../../src/approximate/geometry/seed_and_extend.hpp"
#include "../../src/approximate/nearest_neighbor/flann_index.hpp"
#include "../../src/approximate/nearest_neighbor/navigating_spreading_graph.hpp"
#include "../../src/approximate/nearest_neighbor/product_quantization.hpp"
#include "../../src/approximate/streaming/bloom_filter.hpp"

namespace {

bool fixed_tests() {
    using approximate::nearest_neighbor::FlannIndex;
    using approximate::nearest_neighbor::IvfAdcIndex;
    using approximate::nearest_neighbor::NsgIndex;
    using approximate::nearest_neighbor::ProductQuantizer;

    const std::vector<std::array<double, 2>> static_points{
        {0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0},
        {8.0, 8.0}, {8.0, 9.0}, {9.0, 8.0}
    };
    std::mt19937_64 random(20260729);
    const FlannIndex<double, 2> flann(
        static_points, 4, 2, random
    );
    const auto flann_result =
        flann.nearest(std::array<double, 2>{0.1, 0.1}, 2, 6);
    if(flann_result.size() != 2
       || flann_result.front().index != 0){
        return false;
    }
    if(flann.neighbors_of(0, 2, 5).size() != 2){
        return false;
    }

    const NsgIndex<double, 2> nsg(static_points, 5, 3);
    const auto nsg_result =
        nsg.nearest(std::array<double, 2>{8.1, 8.1}, 2, 6);
    if(nsg_result.size() != 2
       || nsg_result.front().index != 3){
        return false;
    }
    if(nsg.navigation_point() >= nsg.size()
       || nsg.neighbors_of(3, 2, 6).size() != 2){
        return false;
    }

    const std::vector<std::vector<double>> dynamic_points{
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 0.0, 1.0, 0.0},
        {1.0, 1.0, 1.0, 1.0},
        {9.0, 9.0, 9.0, 9.0},
        {9.0, 10.0, 9.0, 10.0},
        {10.0, 9.0, 10.0, 9.0},
        {10.0, 10.0, 10.0, 10.0}
    };
    ProductQuantizer<double> quantizer(
        dynamic_points, 2, 4, 8, random
    );
    const auto encoded = quantizer.encode(dynamic_points[0]);
    const auto decoded = quantizer.decode(encoded);
    if(quantizer.size() != dynamic_points.size()
       || encoded.size() != 2 || decoded.size() != 4
       || !std::isfinite(
           quantizer.asymmetric_distance(
               dynamic_points[0], encoded
           )
       )){
        return false;
    }
    const auto pq_result =
        quantizer.nearest(dynamic_points[0], 2);
    if(pq_result.size() != 2
       || pq_result.front().index != 0){
        return false;
    }

    IvfAdcIndex<double> ivf(
        dynamic_points, 2, 2, 4, 8, random
    );
    const auto ivf_result =
        ivf.nearest(dynamic_points[7], 2, 2);
    if(ivf_result.size() != 2
       || ivf_result.front().index != 7
       || ivf.quantizer().size() != dynamic_points.size()){
        return false;
    }

    CountingBloomFilter<4096, 4> counting(17);
    counting.add(42);
    counting.add(42);
    if(!counting.contains(42)
       || counting.estimate_multiplicity(42) < 2
       || !counting.remove(42)
       || !counting.contains(42)
       || !counting.remove(42)
       || counting.contains(42)){
        return false;
    }

    using approximate::geometry::SeedExtendMatch;
    using approximate::geometry::seed_and_extend;
    const std::string text = "ACGTACGA";
    const std::string pattern = "ACGA";
    if(seed_and_extend<char>(
           std::span<const char>(text),
           std::span<const char>(pattern),
           1, 0
       ) != std::vector<SeedExtendMatch>{{4, 0}}){
        return false;
    }
    const std::string fallback_text = "AAAA";
    const std::string fallback_pattern = "TT";
    if(seed_and_extend<char>(
           std::span<const char>(fallback_text),
           std::span<const char>(fallback_pattern),
           2, 2
       ) != std::vector<SeedExtendMatch>{
           {0, 2}, {1, 2}, {2, 2}
       }){
        return false;
    }
    return true;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    if(!fixed_tests()) return 2;

    std::size_t query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::string text;
        std::string pattern;
        std::size_t seed_length = 0;
        std::size_t maximum_mismatches = 0;
        std::cin >> text >> pattern
            >> seed_length >> maximum_mismatches;
        const auto result =
            approximate::geometry::seed_and_extend<char>(
                std::span<const char>(text),
                std::span<const char>(pattern),
                seed_length,
                maximum_mismatches
            );
        std::cout << result.size();
        for(const auto& match : result){
            std::cout << ' ' << match.text_position
                << ' ' << match.mismatches;
        }
        std::cout << '\n';
    }
}
