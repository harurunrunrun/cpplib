#ifndef CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_RANDOM_HYPERPLANE_LSH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_RANDOM_HYPERPLANE_LSH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace approximate::nearest_neighbor {

struct CosineLshNeighbor {
    std::size_t index = 0;
    long double cosine_similarity = 0.0L;
    std::size_t candidates_examined = 0;
};

template<class T = double>
class RandomHyperplaneLsh {
public:
    template<class RandomEngine>
    RandomHyperplaneLsh(
        std::size_t dimension,
        std::size_t table_count,
        std::size_t bits_per_table,
        RandomEngine& random
    ) : dimension_(dimension), bits_per_table_(bits_per_table) {
        if(dimension == 0) throw std::invalid_argument("dimension must be positive");
        if(table_count == 0) throw std::invalid_argument("table_count must be positive");
        if(bits_per_table == 0 || bits_per_table > 63){
            throw std::invalid_argument("bits_per_table must be in [1, 63]");
        }
        std::normal_distribution<double> distribution(0.0, 1.0);
        hyperplanes_.resize(
            table_count,
            std::vector<std::vector<long double>>(
                bits_per_table, std::vector<long double>(dimension)
            )
        );
        for(auto& table: hyperplanes_){
            for(auto& hyperplane: table){
                long double norm = 0.0L;
                do {
                    norm = 0.0L;
                    for(long double& coefficient: hyperplane){
                        coefficient = static_cast<long double>(distribution(random));
                        norm = std::hypot(norm, coefficient);
                    }
                } while(norm == 0.0L);
                for(long double& coefficient: hyperplane) coefficient /= norm;
            }
        }
        buckets_.resize(table_count);
    }

    [[nodiscard]] std::size_t dimension() const noexcept { return dimension_; }
    [[nodiscard]] std::size_t size() const noexcept { return normalized_points_.size(); }
    [[nodiscard]] std::size_t table_count() const noexcept { return hyperplanes_.size(); }
    [[nodiscard]] std::size_t bits_per_table() const noexcept { return bits_per_table_; }

    void clear() {
        normalized_points_.clear();
        for(auto& buckets: buckets_) buckets.clear();
    }

    void reset(const std::vector<std::vector<T>>& points) {
        clear();
        normalized_points_.reserve(points.size());
        for(const auto& point: points) insert(point);
    }

    std::size_t insert(const std::vector<T>& point) {
        std::vector<long double> normalized = normalize(point);
        const std::size_t index = normalized_points_.size();
        normalized_points_.push_back(std::move(normalized));
        for(std::size_t table = 0; table < hyperplanes_.size(); ++table){
            buckets_[table][hash(normalized_points_.back(), table)].push_back(index);
        }
        return index;
    }

    template<class QueryT>
    [[nodiscard]] std::optional<CosineLshNeighbor> nearest(
        const std::vector<QueryT>& query,
        std::size_t max_hamming_distance = 0
    ) const {
        if(max_hamming_distance > 1){
            throw std::invalid_argument("max_hamming_distance must be zero or one");
        }
        const std::vector<long double> normalized_query = normalize(query);
        if(normalized_points_.empty()) return std::nullopt;
        std::vector<unsigned char> seen(normalized_points_.size(), 0U);
        std::vector<std::size_t> candidates;
        for(std::size_t table = 0; table < hyperplanes_.size(); ++table){
            const std::uint64_t signature = hash(normalized_query, table);
            append_bucket(table, signature, seen, candidates);
            if(max_hamming_distance == 1){
                for(std::size_t bit = 0; bit < bits_per_table_; ++bit){
                    append_bucket(
                        table,
                        signature ^ (std::uint64_t{1} << bit),
                        seen,
                        candidates
                    );
                }
            }
        }
        if(candidates.empty()) return std::nullopt;
        CosineLshNeighbor result;
        result.index = candidates.front();
        result.cosine_similarity = cosine(normalized_query, normalized_points_[result.index]);
        result.candidates_examined = candidates.size();
        for(std::size_t candidate: candidates){
            const long double similarity = cosine(
                normalized_query, normalized_points_[candidate]
            );
            if(similarity > result.cosine_similarity
               || (similarity == result.cosine_similarity && candidate < result.index)){
                result.index = candidate;
                result.cosine_similarity = similarity;
            }
        }
        return result;
    }

private:
    template<class U>
    [[nodiscard]] std::vector<long double> normalize(const std::vector<U>& point) const {
        if(point.size() != dimension_) throw std::invalid_argument("point dimension mismatch");
        std::vector<long double> result(dimension_);
        long double norm = 0.0L;
        for(std::size_t coordinate = 0; coordinate < dimension_; ++coordinate){
            result[coordinate] = static_cast<long double>(point[coordinate]);
            if(!std::isfinite(result[coordinate])){
                throw std::invalid_argument("point contains a non-finite coordinate");
            }
            norm = std::hypot(norm, result[coordinate]);
        }
        if(norm == 0.0L) throw std::invalid_argument("cosine LSH rejects the zero vector");
        if(!std::isfinite(norm)) throw std::overflow_error("point norm overflow");
        for(long double& coordinate: result) coordinate /= norm;
        return result;
    }

    [[nodiscard]] std::uint64_t hash(
        const std::vector<long double>& point,
        std::size_t table
    ) const {
        std::uint64_t result = 0;
        for(std::size_t bit = 0; bit < bits_per_table_; ++bit){
            long double dot = 0.0L;
            for(std::size_t coordinate = 0; coordinate < dimension_; ++coordinate){
                dot += point[coordinate] * hyperplanes_[table][bit][coordinate];
            }
            if(!std::isfinite(dot)) throw std::overflow_error("LSH dot product overflow");
            if(dot >= 0.0L) result |= std::uint64_t{1} << bit;
        }
        return result;
    }

    void append_bucket(
        std::size_t table,
        std::uint64_t signature,
        std::vector<unsigned char>& seen,
        std::vector<std::size_t>& candidates
    ) const {
        const auto iterator = buckets_[table].find(signature);
        if(iterator == buckets_[table].end()) return;
        for(std::size_t index: iterator->second){
            if(seen[index] == 0U){
                seen[index] = 1U;
                candidates.push_back(index);
            }
        }
    }

    static long double cosine(
        const std::vector<long double>& first,
        const std::vector<long double>& second
    ) {
        long double result = 0.0L;
        for(std::size_t coordinate = 0; coordinate < first.size(); ++coordinate){
            result += first[coordinate] * second[coordinate];
        }
        if(!std::isfinite(result)) throw std::overflow_error("cosine similarity overflow");
        return std::clamp(result, -1.0L, 1.0L);
    }

    std::size_t dimension_ = 0;
    std::size_t bits_per_table_ = 0;
    std::vector<std::vector<std::vector<long double>>> hyperplanes_;
    std::vector<std::unordered_map<std::uint64_t, std::vector<std::size_t>>> buckets_;
    std::vector<std::vector<long double>> normalized_points_;
};

}  // namespace approximate::nearest_neighbor

#endif  // CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_RANDOM_HYPERPLANE_LSH_HPP_INCLUDED
