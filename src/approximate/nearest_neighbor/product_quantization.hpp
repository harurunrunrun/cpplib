#ifndef CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_PRODUCT_QUANTIZATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_PRODUCT_QUANTIZATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "ann_common.hpp"

namespace approximate::nearest_neighbor {

namespace detail {

template<class Real>
void validate_pq_point(
    const std::vector<Real>& point,
    std::size_t dimension
) {
    static_assert(
        std::is_floating_point_v<Real>,
        "product-quantization coordinates must be floating point"
    );
    if(point.size() != dimension){
        throw std::invalid_argument(
            "product-quantization point dimensions differ"
        );
    }
    for(const Real coordinate : point){
        if(!std::isfinite(coordinate)){
            throw std::invalid_argument(
                "product-quantization point contains a non-finite coordinate"
            );
        }
    }
}

template<class LeftReal, class RightReal>
long double pq_squared_distance(
    const std::vector<LeftReal>& first,
    const std::vector<RightReal>& second
) {
    if(first.size() != second.size()){
        throw std::invalid_argument(
            "product-quantization point dimensions differ"
        );
    }
    long double result = 0.0L;
    for(std::size_t coordinate = 0;
        coordinate < first.size(); ++coordinate){
        const long double difference =
            static_cast<long double>(first[coordinate])
            - static_cast<long double>(second[coordinate]);
        result += difference * difference;
    }
    if(!std::isfinite(result)){
        throw std::overflow_error(
            "product-quantization distance overflow"
        );
    }
    return result;
}

template<class Real, class UniformRandomBitGenerator>
std::vector<std::vector<Real>> train_pq_kmeans(
    const std::vector<std::vector<Real>>& points,
    std::size_t centroid_count,
    std::size_t iterations,
    UniformRandomBitGenerator& random
) {
    if(points.empty()){
        throw std::invalid_argument(
            "product-quantization training set must not be empty"
        );
    }
    if(centroid_count == 0 || centroid_count > points.size()){
        throw std::invalid_argument(
            "centroid count must be in [1, training point count]"
        );
    }
    if(iterations == 0){
        throw std::invalid_argument(
            "k-means iteration count must be positive"
        );
    }
    const std::size_t dimension = points.front().size();
    if(dimension == 0){
        throw std::invalid_argument(
            "product-quantization dimension must be positive"
        );
    }
    for(const std::vector<Real>& point : points){
        validate_pq_point(point, dimension);
    }

    std::vector<std::size_t> order(points.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::shuffle(order.begin(), order.end(), random);
    std::vector<std::vector<Real>> centroids;
    centroids.reserve(centroid_count);
    for(std::size_t centroid = 0;
        centroid < centroid_count; ++centroid){
        centroids.push_back(points[order[centroid]]);
    }

    std::vector<std::size_t> assignment(points.size(), 0);
    for(std::size_t iteration = 0;
        iteration < iterations; ++iteration){
        for(std::size_t point = 0; point < points.size(); ++point){
            std::size_t best = 0;
            long double best_distance =
                pq_squared_distance(points[point], centroids[0]);
            for(std::size_t centroid = 1;
                centroid < centroid_count; ++centroid){
                const long double distance =
                    pq_squared_distance(
                        points[point], centroids[centroid]
                    );
                if(distance < best_distance){
                    best = centroid;
                    best_distance = distance;
                }
            }
            assignment[point] = best;
        }

        std::vector<std::size_t> counts(centroid_count, 0);
        std::vector<std::vector<long double>> means(
            centroid_count,
            std::vector<long double>(dimension, 0.0L)
        );
        for(std::size_t point = 0; point < points.size(); ++point){
            const std::size_t centroid = assignment[point];
            ++counts[centroid];
            const long double count =
                static_cast<long double>(counts[centroid]);
            for(std::size_t coordinate = 0;
                coordinate < dimension; ++coordinate){
                const long double value =
                    static_cast<long double>(
                        points[point][coordinate]
                    );
                means[centroid][coordinate] +=
                    (value - means[centroid][coordinate]) / count;
                if(!std::isfinite(means[centroid][coordinate])){
                    throw std::overflow_error(
                        "product-quantization centroid overflow"
                    );
                }
            }
        }
        for(std::size_t centroid = 0;
            centroid < centroid_count; ++centroid){
            if(counts[centroid] == 0) continue;
            for(std::size_t coordinate = 0;
                coordinate < dimension; ++coordinate){
                centroids[centroid][coordinate] =
                    static_cast<Real>(
                        means[centroid][coordinate]
                    );
                if(!std::isfinite(
                       centroids[centroid][coordinate]
                   )){
                    throw std::overflow_error(
                        "product-quantization centroid overflow"
                    );
                }
            }
        }
    }
    return centroids;
}

template<class Real, class QueryReal>
long double pq_exact_distance(
    const std::vector<Real>& point,
    const std::vector<QueryReal>& query
) {
    return pq_squared_distance(point, query);
}

}  // namespace detail

template<class Real>
class ProductQuantizer {
    static_assert(
        std::is_floating_point_v<Real>,
        "ProductQuantizer requires a floating-point coordinate type"
    );

public:
    using Point = std::vector<Real>;
    using Code = std::vector<std::size_t>;

    ProductQuantizer() = default;

    template<class UniformRandomBitGenerator>
    ProductQuantizer(
        const std::vector<Point>& points,
        std::size_t subquantizer_count,
        std::size_t centroid_count,
        std::size_t iterations,
        UniformRandomBitGenerator& random
    ) {
        reset(
            points,
            subquantizer_count,
            centroid_count,
            iterations,
            random
        );
    }

    template<class UniformRandomBitGenerator>
    void reset(
        const std::vector<Point>& points,
        std::size_t subquantizer_count,
        std::size_t centroid_count,
        std::size_t iterations,
        UniformRandomBitGenerator& random
    ) {
        if(points.empty()){
            throw std::invalid_argument(
                "product-quantization training set must not be empty"
            );
        }
        const std::size_t dimension = points.front().size();
        if(dimension == 0 || subquantizer_count == 0
           || dimension % subquantizer_count != 0){
            throw std::invalid_argument(
                "dimension must be positive and divisible by subquantizer count"
            );
        }
        for(const Point& point : points){
            detail::validate_pq_point(point, dimension);
        }

        ProductQuantizer replacement;
        replacement.dimension_ = dimension;
        replacement.subquantizer_count_ =
            subquantizer_count;
        replacement.subvector_dimension_ =
            dimension / subquantizer_count;
        replacement.centroid_count_ = centroid_count;
        replacement.codebooks_.resize(subquantizer_count);

        for(std::size_t part = 0;
            part < subquantizer_count; ++part){
            std::vector<Point> subvectors(
                points.size(),
                Point(replacement.subvector_dimension_)
            );
            const std::size_t offset =
                part * replacement.subvector_dimension_;
            for(std::size_t point = 0;
                point < points.size(); ++point){
                std::copy_n(
                    points[point].begin()
                        + static_cast<std::ptrdiff_t>(offset),
                    replacement.subvector_dimension_,
                    subvectors[point].begin()
                );
            }
            replacement.codebooks_[part] =
                detail::train_pq_kmeans(
                    subvectors, centroid_count,
                    iterations, random
                );
        }
        replacement.codes_.reserve(points.size());
        for(const Point& point : points){
            replacement.codes_.push_back(
                replacement.encode_validated(point)
            );
        }
        *this = std::move(replacement);
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return codes_.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return codes_.empty();
    }

    [[nodiscard]] std::size_t dimension() const noexcept {
        return dimension_;
    }

    [[nodiscard]] std::size_t subquantizer_count() const noexcept {
        return subquantizer_count_;
    }

    [[nodiscard]] std::size_t centroid_count() const noexcept {
        return centroid_count_;
    }

    [[nodiscard]] const Code& code(std::size_t index) const {
        return codes_.at(index);
    }

    [[nodiscard]] const std::vector<Point>& codebook(
        std::size_t subquantizer
    ) const {
        return codebooks_.at(subquantizer);
    }

    template<class QueryReal>
    [[nodiscard]] Code encode(
        const std::vector<QueryReal>& point
    ) const {
        validate_query(point);
        return encode_validated(point);
    }

    [[nodiscard]] Point decode(const Code& code) const {
        validate_code(code);
        Point result(dimension_);
        for(std::size_t part = 0;
            part < subquantizer_count_; ++part){
            const Point& centroid =
                codebooks_[part][code[part]];
            const std::size_t offset =
                part * subvector_dimension_;
            std::copy(
                centroid.begin(), centroid.end(),
                result.begin()
                    + static_cast<std::ptrdiff_t>(offset)
            );
        }
        return result;
    }

    template<class QueryReal>
    [[nodiscard]] long double asymmetric_distance(
        const std::vector<QueryReal>& query,
        const Code& code
    ) const {
        validate_query(query);
        validate_code(code);
        long double distance = 0.0L;
        for(std::size_t part = 0;
            part < subquantizer_count_; ++part){
            const Point& centroid =
                codebooks_[part][code[part]];
            const std::size_t offset =
                part * subvector_dimension_;
            for(std::size_t coordinate = 0;
                coordinate < subvector_dimension_;
                ++coordinate){
                const long double difference =
                    static_cast<long double>(
                        query[offset + coordinate]
                    )
                    - static_cast<long double>(
                        centroid[coordinate]
                    );
                distance += difference * difference;
            }
        }
        if(!std::isfinite(distance)){
            throw std::overflow_error(
                "product-quantization distance overflow"
            );
        }
        return distance;
    }

    template<class QueryReal>
    [[nodiscard]] std::vector<AnnNeighbor> nearest(
        const std::vector<QueryReal>& query,
        std::size_t k
    ) const {
        validate_query(query);
        if(k == 0 || k > codes_.size()){
            throw std::invalid_argument(
                "k must be in [1, encoded point count]"
            );
        }
        std::vector<AnnNeighbor> result;
        result.reserve(codes_.size());
        for(std::size_t index = 0;
            index < codes_.size(); ++index){
            result.push_back({
                index,
                asymmetric_distance(query, codes_[index])
            });
        }
        std::sort(
            result.begin(), result.end(),
            detail::neighbor_less
        );
        result.resize(k);
        return result;
    }

private:
    template<class QueryReal>
    void validate_query(
        const std::vector<QueryReal>& query
    ) const {
        static_assert(
            std::is_floating_point_v<QueryReal>,
            "ProductQuantizer queries must be floating point"
        );
        if(dimension_ == 0){
            throw std::logic_error(
                "ProductQuantizer is not initialized"
            );
        }
        if(query.size() != dimension_){
            throw std::invalid_argument(
                "product-quantization query dimension differs"
            );
        }
        for(const QueryReal coordinate : query){
            if(!std::isfinite(coordinate)){
                throw std::invalid_argument(
                    "product-quantization query is non-finite"
                );
            }
        }
    }

    void validate_code(const Code& code) const {
        if(code.size() != subquantizer_count_){
            throw std::invalid_argument(
                "product-quantization code length differs"
            );
        }
        for(const std::size_t centroid : code){
            if(centroid >= centroid_count_){
                throw std::out_of_range(
                    "product-quantization code is out of range"
                );
            }
        }
    }

    template<class QueryReal>
    Code encode_validated(
        const std::vector<QueryReal>& point
    ) const {
        Code result(subquantizer_count_, 0);
        for(std::size_t part = 0;
            part < subquantizer_count_; ++part){
            const std::size_t offset =
                part * subvector_dimension_;
            long double best_distance = 0.0L;
            for(std::size_t centroid = 0;
                centroid < centroid_count_; ++centroid){
                long double distance = 0.0L;
                for(std::size_t coordinate = 0;
                    coordinate < subvector_dimension_;
                    ++coordinate){
                    const long double difference =
                        static_cast<long double>(
                            point[offset + coordinate]
                        )
                        - static_cast<long double>(
                            codebooks_[part][centroid][coordinate]
                        );
                    distance += difference * difference;
                }
                if(!std::isfinite(distance)){
                    throw std::overflow_error(
                        "product-quantization distance overflow"
                    );
                }
                if(centroid == 0 || distance < best_distance){
                    best_distance = distance;
                    result[part] = centroid;
                }
            }
        }
        return result;
    }

    std::size_t dimension_ = 0;
    std::size_t subquantizer_count_ = 0;
    std::size_t subvector_dimension_ = 0;
    std::size_t centroid_count_ = 0;
    std::vector<std::vector<Point>> codebooks_;
    std::vector<Code> codes_;
};

template<class Real>
class IvfAdcIndex {
    static_assert(
        std::is_floating_point_v<Real>,
        "IvfAdcIndex requires a floating-point coordinate type"
    );

public:
    using Point = std::vector<Real>;

    IvfAdcIndex() = default;

    template<class UniformRandomBitGenerator>
    IvfAdcIndex(
        std::vector<Point> points,
        std::size_t coarse_centroid_count,
        std::size_t subquantizer_count,
        std::size_t pq_centroid_count,
        std::size_t iterations,
        UniformRandomBitGenerator& random
    ) {
        reset(
            std::move(points),
            coarse_centroid_count,
            subquantizer_count,
            pq_centroid_count,
            iterations,
            random
        );
    }

    template<class UniformRandomBitGenerator>
    void reset(
        std::vector<Point> points,
        std::size_t coarse_centroid_count,
        std::size_t subquantizer_count,
        std::size_t pq_centroid_count,
        std::size_t iterations,
        UniformRandomBitGenerator& random
    ) {
        if(points.empty()){
            throw std::invalid_argument(
                "IVFADC training set must not be empty"
            );
        }
        const std::size_t dimension = points.front().size();
        for(const Point& point : points){
            detail::validate_pq_point(point, dimension);
        }
        IvfAdcIndex replacement;
        replacement.points_ = std::move(points);
        replacement.dimension_ = dimension;
        replacement.coarse_codebook_ =
            detail::train_pq_kmeans(
                replacement.points_,
                coarse_centroid_count,
                iterations,
                random
            );
        replacement.inverted_lists_.resize(
            coarse_centroid_count
        );
        replacement.coarse_assignment_.resize(
            replacement.points_.size()
        );
        std::vector<Point> residuals(
            replacement.points_.size(),
            Point(dimension)
        );
        for(std::size_t index = 0;
            index < replacement.points_.size(); ++index){
            std::size_t best = 0;
            long double best_distance =
                detail::pq_squared_distance(
                    replacement.points_[index],
                    replacement.coarse_codebook_[0]
                );
            for(std::size_t centroid = 1;
                centroid < coarse_centroid_count;
                ++centroid){
                const long double distance =
                    detail::pq_squared_distance(
                        replacement.points_[index],
                        replacement.coarse_codebook_[centroid]
                    );
                if(distance < best_distance){
                    best = centroid;
                    best_distance = distance;
                }
            }
            replacement.coarse_assignment_[index] = best;
            replacement.inverted_lists_[best].push_back(index);
            for(std::size_t coordinate = 0;
                coordinate < dimension; ++coordinate){
                const long double residual =
                    static_cast<long double>(
                        replacement.points_[index][coordinate]
                    )
                    - static_cast<long double>(
                        replacement.coarse_codebook_[best][coordinate]
                    );
                residuals[index][coordinate] =
                    static_cast<Real>(residual);
                if(!std::isfinite(residuals[index][coordinate])){
                    throw std::overflow_error(
                        "IVFADC residual overflow"
                    );
                }
            }
        }
        replacement.quantizer_.reset(
            residuals,
            subquantizer_count,
            pq_centroid_count,
            iterations,
            random
        );
        *this = std::move(replacement);
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return points_.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return points_.empty();
    }

    [[nodiscard]] std::size_t dimension() const noexcept {
        return dimension_;
    }

    [[nodiscard]] std::size_t coarse_centroid_count() const noexcept {
        return coarse_codebook_.size();
    }

    [[nodiscard]] const Point& point(std::size_t index) const {
        return points_.at(index);
    }

    [[nodiscard]] std::size_t coarse_assignment(
        std::size_t index
    ) const {
        return coarse_assignment_.at(index);
    }

    [[nodiscard]] const std::vector<std::size_t>& inverted_list(
        std::size_t coarse_centroid
    ) const {
        return inverted_lists_.at(coarse_centroid);
    }

    [[nodiscard]] const ProductQuantizer<Real>& quantizer() const noexcept {
        return quantizer_;
    }

    template<class QueryReal>
    [[nodiscard]] std::vector<AnnNeighbor> nearest(
        const std::vector<QueryReal>& query,
        std::size_t k,
        std::size_t probe_count
    ) const {
        validate_query(query);
        if(k == 0 || k > points_.size()){
            throw std::invalid_argument(
                "k must be in [1, IVFADC point count]"
            );
        }
        if(probe_count == 0
           || probe_count > coarse_codebook_.size()){
            throw std::invalid_argument(
                "IVFADC probe count is out of range"
            );
        }

        std::vector<AnnNeighbor> coarse;
        coarse.reserve(coarse_codebook_.size());
        for(std::size_t centroid = 0;
            centroid < coarse_codebook_.size();
            ++centroid){
            coarse.push_back({
                centroid,
                detail::pq_squared_distance(
                    query, coarse_codebook_[centroid]
                )
            });
        }
        std::sort(
            coarse.begin(), coarse.end(),
            detail::neighbor_less
        );
        coarse.resize(probe_count);

        std::vector<AnnNeighbor> result;
        for(const AnnNeighbor& coarse_neighbor : coarse){
            const std::size_t centroid =
                coarse_neighbor.index;
            std::vector<long double> residual_query(
                dimension_
            );
            for(std::size_t coordinate = 0;
                coordinate < dimension_; ++coordinate){
                residual_query[coordinate] =
                    static_cast<long double>(query[coordinate])
                    - static_cast<long double>(
                        coarse_codebook_[centroid][coordinate]
                    );
                if(!std::isfinite(
                       residual_query[coordinate]
                   )){
                    throw std::overflow_error(
                        "IVFADC query residual overflow"
                    );
                }
            }
            for(const std::size_t index :
                inverted_lists_[centroid]){
                result.push_back({
                    index,
                    quantizer_.asymmetric_distance(
                        residual_query,
                        quantizer_.code(index)
                    )
                });
            }
        }
        if(result.size() < k){
            result.clear();
            result.reserve(points_.size());
            for(std::size_t index = 0;
                index < points_.size(); ++index){
                result.push_back({
                    index,
                    detail::pq_exact_distance(
                        points_[index], query
                    )
                });
            }
        }
        std::sort(
            result.begin(), result.end(),
            detail::neighbor_less
        );
        result.resize(k);
        return result;
    }

private:
    template<class QueryReal>
    void validate_query(
        const std::vector<QueryReal>& query
    ) const {
        static_assert(
            std::is_floating_point_v<QueryReal>,
            "IvfAdcIndex queries must be floating point"
        );
        if(points_.empty()){
            throw std::logic_error(
                "IvfAdcIndex is not initialized"
            );
        }
        if(query.size() != dimension_){
            throw std::invalid_argument(
                "IVFADC query dimension differs"
            );
        }
        for(const QueryReal coordinate : query){
            if(!std::isfinite(coordinate)){
                throw std::invalid_argument(
                    "IVFADC query is non-finite"
                );
            }
        }
    }

    std::vector<Point> points_;
    std::size_t dimension_ = 0;
    std::vector<Point> coarse_codebook_;
    std::vector<std::vector<std::size_t>> inverted_lists_;
    std::vector<std::size_t> coarse_assignment_;
    ProductQuantizer<Real> quantizer_;
};

}  // namespace approximate::nearest_neighbor

#endif  // CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_PRODUCT_QUANTIZATION_HPP_INCLUDED
