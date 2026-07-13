// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/nearest_neighbor/random_hyperplane_lsh.hpp"

long double cosine(const std::vector<long double>& first, const std::vector<long double>& second){
    long double dot = 0.0L;
    long double first_norm = 0.0L;
    long double second_norm = 0.0L;
    for(std::size_t index = 0; index < first.size(); ++index){
        dot += first[index] * second[index];
        first_norm = std::hypot(first_norm, first[index]);
        second_norm = std::hypot(second_norm, second[index]);
    }
    return dot / (first_norm * second_norm);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t dimension;
    std::size_t n;
    std::size_t query_count;
    std::size_t tables;
    std::size_t bits;
    std::uint64_t seed;
    if(!(std::cin >> dimension >> n >> query_count >> tables >> bits >> seed)) return 0;
    std::vector<std::vector<long double>> points(n, std::vector<long double>(dimension));
    for(auto& point: points) for(long double& value: point) std::cin >> value;
    std::mt19937_64 first_random(seed);
    std::mt19937_64 second_random(seed);
    approximate::nearest_neighbor::RandomHyperplaneLsh<long double> first(
        dimension, tables, bits, first_random
    );
    approximate::nearest_neighbor::RandomHyperplaneLsh<long double> second(
        dimension, tables, bits, second_random
    );
    first.reset(points);
    second.reset(points);
    assert(first.size() == points.size());
    for(std::size_t query_index = 0; query_index < query_count; ++query_index){
        std::vector<long double> query(dimension);
        for(long double& value: query) std::cin >> value;
        const auto result = first.nearest(query);
        const auto repeated = second.nearest(query);
        assert(result.has_value());
        assert(repeated.has_value());
        assert(result->index == repeated->index);
        assert(result->cosine_similarity == repeated->cosine_similarity);
        std::size_t expected_index = 0;
        long double expected_similarity = -2.0L;
        for(std::size_t index = 0; index < points.size(); ++index){
            const long double similarity = cosine(query, points[index]);
            if(similarity > expected_similarity){
                expected_similarity = similarity;
                expected_index = index;
            }
        }
        assert(result->index == expected_index);
        assert(std::fabs(result->cosine_similarity - 1.0L) < 1.0e-15L);
        const auto multiprobe = first.nearest(query, 1);
        assert(multiprobe.has_value());
        assert(multiprobe->cosine_similarity >= result->cosine_similarity - 1.0e-18L);
        std::cout << result->index << " 1\n";
    }
    bool invalid = false;
    try{ static_cast<void>(first.nearest(std::vector<long double>(dimension + 1, 1.0L))); }
    catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(first.insert(std::vector<long double>(dimension, 0.0L))); }
    catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{
        std::mt19937_64 random(1);
        approximate::nearest_neighbor::RandomHyperplaneLsh<double> bad(2, 1, 64, random);
        static_cast<void>(bad);
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    std::mt19937_64 empty_random(7);
    approximate::nearest_neighbor::RandomHyperplaneLsh<double> empty(2, 2, 2, empty_random);
    assert(!empty.nearest(std::vector<double>{1.0, 0.0}).has_value());
}
