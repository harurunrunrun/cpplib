#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_AFFINITY_PROPAGATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_AFFINITY_PROPAGATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

#include "common.hpp"
#include "detail/stable_numeric.hpp"

namespace approximate::clustering {

struct AffinityPropagationResult {
    std::vector<std::size_t> exemplars;
    std::vector<std::size_t> labels;
    std::size_t iterations = 0;
    bool converged = false;
};

template<class Real>
[[nodiscard]] AffinityPropagationResult affinity_propagation(
    const DensePoints<Real>& points,
    std::optional<long double> preference = std::nullopt,
    long double damping = 0.5L,
    std::size_t maximum_iterations = 200,
    std::size_t convergence_iterations = 15
){
    detail::validate_dense_points(points, true);
    if((preference.has_value() && !std::isfinite(*preference))
       || !std::isfinite(damping) || damping < 0.0L || damping >= 1.0L
       || convergence_iterations == 0){
        throw std::invalid_argument("invalid affinity propagation parameter");
    }
    const std::size_t point_count = points.size();
    if(point_count == 0) return {};
    if(point_count == 1){
        return {{0}, {0}, 0, true};
    }
    if(point_count > std::numeric_limits<std::size_t>::max() / (point_count - 1)){
        throw std::length_error("too many points for affinity propagation");
    }

    std::vector<std::vector<long double>> similarity(
        point_count, std::vector<long double>(point_count, 0.0L)
    );
    std::vector<long double> off_diagonal;
    off_diagonal.reserve(point_count * (point_count - 1));
    for(std::size_t left = 0; left < point_count; ++left){
        for(std::size_t right = left + 1; right < point_count; ++right){
            const long double distance = detail::stable_euclidean_distance(points[left], points[right]);
            const long double squared_distance = detail::checked_nonnegative_product(
                distance, distance, "affinity similarity overflowed"
            );
            const long double value = -squared_distance;
            similarity[left][right] = value;
            similarity[right][left] = value;
            off_diagonal.push_back(value);
            off_diagonal.push_back(value);
        }
    }
    long double diagonal_preference = 0.0L;
    if(preference.has_value()){
        diagonal_preference = *preference;
    }else{
        const auto middle = off_diagonal.begin()
            + static_cast<std::ptrdiff_t>(off_diagonal.size() / 2);
        std::nth_element(off_diagonal.begin(), middle, off_diagonal.end());
        const long double upper = *middle;
        const long double lower = *std::max_element(off_diagonal.begin(), middle);
        diagonal_preference = std::midpoint(lower, upper);
    }
    for(std::size_t point = 0; point < point_count; ++point){
        similarity[point][point] = diagonal_preference;
    }

    std::vector<std::vector<long double>> responsibility(
        point_count, std::vector<long double>(point_count, 0.0L)
    );
    std::vector<std::vector<long double>> availability(
        point_count, std::vector<long double>(point_count, 0.0L)
    );
    const auto evidence = [&](std::size_t point){
        return detail::checked_finite_sum(
            responsibility[point][point], availability[point][point],
            "affinity evidence overflowed"
        );
    };
    std::vector<bool> previous_exemplar(point_count, false);
    std::size_t stable_iterations = 0;
    AffinityPropagationResult result;
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        for(std::size_t point = 0; point < point_count; ++point){
            std::size_t maximum_index = 0;
            long double maximum_value = detail::checked_finite_sum(
                availability[point][0], similarity[point][0],
                "affinity message score overflowed"
            );
            long double second_value = -std::numeric_limits<long double>::infinity();
            for(std::size_t exemplar = 1; exemplar < point_count; ++exemplar){
                const long double value = detail::checked_finite_sum(
                    availability[point][exemplar], similarity[point][exemplar],
                    "affinity message score overflowed"
                );
                if(value > maximum_value){
                    second_value = maximum_value;
                    maximum_value = value;
                    maximum_index = exemplar;
                }else if(value > second_value){
                    second_value = value;
                }
            }
            for(std::size_t exemplar = 0; exemplar < point_count; ++exemplar){
                const long double competitor = exemplar == maximum_index ? second_value : maximum_value;
                const long double updated = detail::checked_finite_sum(
                    similarity[point][exemplar], -competitor,
                    "affinity responsibility overflowed"
                );
                responsibility[point][exemplar] = std::lerp(
                    updated, responsibility[point][exemplar], damping
                );
            }
        }
        for(std::size_t exemplar = 0; exemplar < point_count; ++exemplar){
            long double positive_sum = 0.0L;
            for(std::size_t point = 0; point < point_count; ++point){
                if(point == exemplar) continue;
                positive_sum = detail::checked_finite_sum(
                    positive_sum, std::max(0.0L, responsibility[point][exemplar]),
                    "affinity availability overflowed"
                );
            }
            for(std::size_t point = 0; point < point_count; ++point){
                long double updated = positive_sum;
                if(point != exemplar){
                    updated = detail::checked_finite_sum(
                        responsibility[exemplar][exemplar], positive_sum,
                        "affinity availability overflowed"
                    );
                    updated = detail::checked_finite_sum(
                        updated, -std::max(0.0L, responsibility[point][exemplar]),
                        "affinity availability overflowed"
                    );
                    updated = std::min(0.0L, updated);
                }
                availability[point][exemplar] = std::lerp(
                    updated, availability[point][exemplar], damping
                );
            }
        }

        std::vector<bool> current_exemplar(point_count, false);
        for(std::size_t point = 0; point < point_count; ++point){
            current_exemplar[point] = evidence(point) > 0.0L;
        }
        const bool has_exemplar = std::find(
            current_exemplar.begin(), current_exemplar.end(), true
        ) != current_exemplar.end();
        if(!has_exemplar){
            previous_exemplar = std::move(current_exemplar);
            stable_iterations = 0;
        }else if(current_exemplar == previous_exemplar){
            ++stable_iterations;
        }else{
            previous_exemplar = std::move(current_exemplar);
            stable_iterations = 1;
        }
        result.iterations = iteration + 1;
        if(stable_iterations >= convergence_iterations){
            result.converged = true;
            break;
        }
    }

    for(std::size_t point = 0; point < point_count; ++point){
        if(evidence(point) > 0.0L){
            result.exemplars.push_back(point);
        }
    }
    if(result.exemplars.empty()){
        std::size_t best = 0;
        for(std::size_t point = 1; point < point_count; ++point){
            if(evidence(point) > evidence(best)){
                best = point;
            }
        }
        result.exemplars.push_back(best);
    }
    result.labels.resize(point_count);
    for(std::size_t point = 0; point < point_count; ++point){
        const auto own = std::lower_bound(result.exemplars.begin(), result.exemplars.end(), point);
        if(own != result.exemplars.end() && *own == point){
            result.labels[point] = static_cast<std::size_t>(own - result.exemplars.begin());
            continue;
        }
        std::size_t best = 0;
        for(std::size_t index = 1; index < result.exemplars.size(); ++index){
            if(similarity[point][result.exemplars[index]]
               > similarity[point][result.exemplars[best]]){
                best = index;
            }
        }
        result.labels[point] = best;
    }
    return result;
}

}  // namespace approximate::clustering

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_AFFINITY_PROPAGATION_HPP_INCLUDED
