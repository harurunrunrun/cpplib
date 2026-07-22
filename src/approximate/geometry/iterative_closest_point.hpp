#ifndef CPPLIB_SRC_APPROXIMATE_GEOMETRY_ITERATIVE_CLOSEST_POINT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GEOMETRY_ITERATIVE_CLOSEST_POINT_HPP_INCLUDED

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace approximate::geometry {

template<std::size_t Dimension>
struct TranslationIcpResult {
    std::array<long double, Dimension> translation{};
    std::vector<std::array<long double, Dimension>> transformed_points;
    std::vector<std::size_t> correspondences;
    long double mean_squared_error = 0.0L;
    std::size_t iterations = 0;
    bool converged = false;
};

namespace detail {

template<class T, std::size_t Dimension>
void validate_icp_points(const std::vector<std::array<T, Dimension>>& points) {
    for(const auto& point: points){
        for(const T& coordinate: point){
            if(!std::isfinite(static_cast<long double>(coordinate))){
                throw std::invalid_argument("point cloud contains a non-finite coordinate");
            }
        }
    }
}

template<std::size_t Dimension>
long double icp_squared_distance(
    const std::array<long double, Dimension>& first,
    const std::array<long double, Dimension>& second
) {
    long double result = 0.0L;
    for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
        const long double difference = first[coordinate] - second[coordinate];
        result += difference * difference;
    }
    if(!std::isfinite(result)) throw std::overflow_error("ICP squared distance overflow");
    return result;
}

template<std::size_t Dimension>
std::size_t nearest_icp_point_index(
    const std::array<long double, Dimension>& query,
    const std::vector<std::array<long double, Dimension>>& targets
) {
    std::size_t best_index = 0;
    long double best_distance = icp_squared_distance(query, targets[0]);
    for(std::size_t index = 1; index < targets.size(); ++index){
        const long double candidate = icp_squared_distance(query, targets[index]);
        if(candidate < best_distance){
            best_distance = candidate;
            best_index = index;
        }
    }
    return best_index;
}

template<class T, std::size_t Dimension>
std::vector<std::array<long double, Dimension>> to_long_double_points(
    const std::vector<std::array<T, Dimension>>& points
) {
    std::vector<std::array<long double, Dimension>> result(points.size());
    for(std::size_t index = 0; index < points.size(); ++index){
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            result[index][coordinate] = static_cast<long double>(points[index][coordinate]);
        }
    }
    return result;
}

}  // namespace detail

template<std::size_t Dimension, class SourceT, class TargetT>
TranslationIcpResult<Dimension> brute_force_translation_icp(
    const std::vector<std::array<SourceT, Dimension>>& source,
    const std::vector<std::array<TargetT, Dimension>>& target,
    std::size_t max_iterations = 100,
    long double tolerance = 1.0e-12L,
    std::array<long double, Dimension> initial_translation = {}
) {
    static_assert(Dimension > 0, "Dimension must be positive");
    if(max_iterations == 0) throw std::invalid_argument("max_iterations must be positive");
    if(!std::isfinite(tolerance) || tolerance < 0.0L){
        throw std::invalid_argument("tolerance must be finite and non-negative");
    }
    detail::validate_icp_points(source);
    detail::validate_icp_points(target);
    for(long double coordinate: initial_translation){
        if(!std::isfinite(coordinate)){
            throw std::invalid_argument("initial translation contains a non-finite value");
        }
    }
    TranslationIcpResult<Dimension> result;
    result.translation = initial_translation;
    if(source.empty()){
        result.converged = true;
        return result;
    }
    if(target.empty()) throw std::invalid_argument("target point cloud is empty");
    const auto source_points = detail::to_long_double_points(source);
    const auto target_points = detail::to_long_double_points(target);
    result.transformed_points.resize(source.size());
    result.correspondences.resize(source.size());
    long double previous_error = std::numeric_limits<long double>::infinity();
    const long double squared_tolerance = tolerance * tolerance;
    if(!std::isfinite(squared_tolerance)){
        throw std::overflow_error("squared ICP tolerance overflow");
    }
    for(std::size_t iteration = 0; iteration < max_iterations; ++iteration){
        for(std::size_t index = 0; index < source.size(); ++index){
            for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
                result.transformed_points[index][coordinate] = source_points[index][coordinate]
                    + result.translation[coordinate];
                if(!std::isfinite(result.transformed_points[index][coordinate])){
                    throw std::overflow_error("transformed ICP coordinate overflow");
                }
            }
            result.correspondences[index] = detail::nearest_icp_point_index(
                result.transformed_points[index], target_points
            );
        }
        std::array<long double, Dimension> update{};
        for(std::size_t index = 0; index < source.size(); ++index){
            for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
                const long double difference =
                    target_points[result.correspondences[index]][coordinate]
                    - result.transformed_points[index][coordinate];
                update[coordinate] += difference / static_cast<long double>(source.size());
                if(!std::isfinite(update[coordinate])){
                    throw std::overflow_error("ICP centroid overflow");
                }
            }
        }
        long double update_squared_norm = 0.0L;
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            result.translation[coordinate] += update[coordinate];
            if(!std::isfinite(result.translation[coordinate])){
                throw std::overflow_error("ICP translation overflow");
            }
            update_squared_norm += update[coordinate] * update[coordinate];
        }
        long double error = 0.0L;
        for(std::size_t index = 0; index < source.size(); ++index){
            for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
                result.transformed_points[index][coordinate] += update[coordinate];
            }
            error += detail::icp_squared_distance(
                result.transformed_points[index], target_points[result.correspondences[index]]
            ) / static_cast<long double>(source.size());
            if(!std::isfinite(error)) throw std::overflow_error("ICP objective overflow");
        }
        if(!std::isfinite(update_squared_norm)){
            throw std::overflow_error("ICP update overflow");
        }
        result.mean_squared_error = error;
        result.iterations = iteration + 1;
        const long double improvement = previous_error - error;
        if(update_squared_norm <= squared_tolerance
           || (improvement >= 0.0L && improvement <= tolerance)){
            result.converged = true;
            break;
        }
        previous_error = error;
    }
    long double final_error = 0.0L;
    for(std::size_t index = 0; index < source.size(); ++index){
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            result.transformed_points[index][coordinate] = source_points[index][coordinate]
                + result.translation[coordinate];
        }
        result.correspondences[index] = detail::nearest_icp_point_index(
            result.transformed_points[index], target_points
        );
        final_error += detail::icp_squared_distance(
            result.transformed_points[index], target_points[result.correspondences[index]]
        ) / static_cast<long double>(source.size());
        if(!std::isfinite(final_error)) throw std::overflow_error("ICP objective overflow");
    }
    result.mean_squared_error = final_error;
    return result;
}

}  // namespace approximate::geometry

#endif  // CPPLIB_SRC_APPROXIMATE_GEOMETRY_ITERATIVE_CLOSEST_POINT_HPP_INCLUDED
