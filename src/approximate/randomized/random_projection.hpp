#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace approximate::randomized {

template<class RandomEngine>
std::vector<std::vector<double>> rademacher_projection_matrix(
    std::size_t input_dimension,
    std::size_t output_dimension,
    RandomEngine& random
) {
    if(output_dimension == 0) throw std::invalid_argument("output_dimension must be positive");
    const double scale = 1.0 / std::sqrt(static_cast<double>(output_dimension));
    std::uniform_int_distribution<int> sign(0, 1);
    std::vector<std::vector<double>> projection(
        output_dimension, std::vector<double>(input_dimension)
    );
    for(auto& row: projection){
        for(double& value: row) value = sign(random) == 0 ? -scale : scale;
    }
    return projection;
}

template<std::size_t InputDimension, std::size_t OutputDimension, class RandomEngine>
std::array<std::array<double, InputDimension>, OutputDimension>
rademacher_projection_matrix(RandomEngine& random) {
    static_assert(OutputDimension > 0, "OutputDimension must be positive");
    const double scale = 1.0 / std::sqrt(static_cast<double>(OutputDimension));
    std::uniform_int_distribution<int> sign(0, 1);
    std::array<std::array<double, InputDimension>, OutputDimension> projection{};
    for(auto& row: projection){
        for(double& value: row) value = sign(random) == 0 ? -scale : scale;
    }
    return projection;
}

inline std::vector<std::vector<double>> apply_random_projection(
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<double>>& projection
) {
    if(projection.empty()) throw std::invalid_argument("projection must have a positive output dimension");
    const std::size_t input_dimension = projection.front().size();
    for(const auto& row: projection){
        if(row.size() != input_dimension) throw std::invalid_argument("projection is not rectangular");
        for(double value: row){
            if(!std::isfinite(value)) throw std::invalid_argument("projection contains a non-finite value");
        }
    }
    for(const auto& point: points){
        if(point.size() != input_dimension) throw std::invalid_argument("point dimension mismatch");
        for(double value: point){
            if(!std::isfinite(value)) throw std::invalid_argument("point contains a non-finite value");
        }
    }

    std::vector<std::vector<double>> result(
        points.size(), std::vector<double>(projection.size(), 0.0)
    );
    for(std::size_t point_index = 0; point_index < points.size(); ++point_index){
        for(std::size_t output = 0; output < projection.size(); ++output){
            long double value = 0.0L;
            for(std::size_t input = 0; input < input_dimension; ++input){
                value += static_cast<long double>(points[point_index][input])
                    * static_cast<long double>(projection[output][input]);
            }
            if(!std::isfinite(value)
               || std::fabs(value) > static_cast<long double>(std::numeric_limits<double>::max())){
                throw std::overflow_error("projected coordinate overflow");
            }
            result[point_index][output] = static_cast<double>(value);
        }
    }
    return result;
}

template<class RandomEngine>
std::vector<std::vector<double>> johnson_lindenstrauss_transform(
    const std::vector<std::vector<double>>& points,
    std::size_t output_dimension,
    RandomEngine& random
) {
    if(output_dimension == 0) throw std::invalid_argument("output_dimension must be positive");
    const std::size_t input_dimension = points.empty() ? 0 : points.front().size();
    const auto projection = rademacher_projection_matrix(input_dimension, output_dimension, random);
    return apply_random_projection(points, projection);
}

inline std::size_t johnson_lindenstrauss_dimension(
    std::size_t point_count,
    double epsilon,
    double failure_probability
) {
    if(!std::isfinite(epsilon) || epsilon <= 0.0 || epsilon >= 1.0){
        throw std::invalid_argument("epsilon must be in (0, 1)");
    }
    if(!std::isfinite(failure_probability)
       || failure_probability <= 0.0 || failure_probability >= 1.0){
        throw std::invalid_argument("failure_probability must be in (0, 1)");
    }
    if(point_count < 2) return 1;
    const long double e = static_cast<long double>(epsilon);
    const long double denominator = e * e / 4.0L - e * e * e / 6.0L;
    const long double numerator = std::log(static_cast<long double>(point_count))
        + std::log(static_cast<long double>(point_count - 1))
        - std::log(static_cast<long double>(failure_probability));
    const long double dimension = std::ceil(numerator / denominator);
    if(!std::isfinite(dimension)
       || dimension > static_cast<long double>(std::numeric_limits<std::size_t>::max())){
        throw std::overflow_error("Johnson-Lindenstrauss dimension overflows size_t");
    }
    return static_cast<std::size_t>(dimension);
}

template<class T, std::size_t PointCount, std::size_t InputDimension,
         std::size_t OutputDimension>
std::array<std::array<double, OutputDimension>, PointCount> apply_random_projection(
    const std::array<std::array<T, InputDimension>, PointCount>& points,
    const std::array<std::array<double, InputDimension>, OutputDimension>& projection
) {
    static_assert(OutputDimension > 0, "OutputDimension must be positive");
    std::array<std::array<double, OutputDimension>, PointCount> result{};
    for(std::size_t point = 0; point < PointCount; ++point){
        for(std::size_t output = 0; output < OutputDimension; ++output){
            long double value = 0.0L;
            for(std::size_t input = 0; input < InputDimension; ++input){
                const long double coordinate = static_cast<long double>(points[point][input]);
                const long double coefficient = static_cast<long double>(projection[output][input]);
                if(!std::isfinite(coordinate) || !std::isfinite(coefficient)){
                    throw std::invalid_argument("projection input contains a non-finite value");
                }
                value += coordinate * coefficient;
            }
            if(!std::isfinite(value)
               || std::fabs(value) > static_cast<long double>(std::numeric_limits<double>::max())){
                throw std::overflow_error("projected coordinate overflow");
            }
            result[point][output] = static_cast<double>(value);
        }
    }
    return result;
}

}  // namespace approximate::randomized
