#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_SUBMODULAR_SUBMODULAR_FUNCTION_MINIMIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_SUBMODULAR_SUBMODULAR_FUNCTION_MINIMIZATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace combinatorial_optimization {

template<class Value>
struct SubmodularMinimizationResult {
    std::vector<std::size_t> elements;
    Value value{};
    std::vector<long double> minimum_norm_point;
    std::size_t iterations = 0;
    long double dual_gap = 0.0L;
    bool minimum_norm_converged = false;
};

namespace submodular_minimization_internal {

inline long double dot(
    const std::vector<long double>& first,
    const std::vector<long double>& second
) {
    long double result = 0.0L;
    for(std::size_t index = 0; index < first.size(); ++index){
        result += first[index] * second[index];
    }
    return result;
}

template<class Oracle>
std::vector<long double> greedy_base(
    std::size_t size,
    const std::vector<long double>& weight,
    Oracle& oracle
) {
    std::vector<std::size_t> order(size);
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(
        order.begin(), order.end(),
        [&](std::size_t first, std::size_t second){
            return weight[first] < weight[second];
        }
    );
    std::vector<unsigned char> set(size, 0);
    long double previous =
        static_cast<long double>(std::invoke(oracle, set));
    std::vector<long double> base(size);
    for(const std::size_t element : order){
        set[element] = 1;
        const long double current =
            static_cast<long double>(std::invoke(oracle, set));
        if(!std::isfinite(current)){
            throw std::invalid_argument(
                "submodular oracle returned a non-finite value"
            );
        }
        base[element] = current - previous;
        previous = current;
    }
    return base;
}

inline std::vector<long double> affine_minimizer(
    const std::vector<std::vector<long double>>& points
) {
    const std::size_t count = points.size();
    std::vector<std::vector<long double>> matrix(
        count + 1,
        std::vector<long double>(count + 2)
    );
    for(std::size_t row = 0; row < count; ++row){
        for(std::size_t column = 0; column < count; ++column){
            matrix[row][column] = dot(points[row], points[column]);
        }
        matrix[row][row] += 1e-28L;
        matrix[row][count] = 1.0L;
    }
    for(std::size_t column = 0; column < count; ++column){
        matrix[count][column] = 1.0L;
    }
    matrix[count][count + 1] = 1.0L;

    const std::size_t dimension = count + 1;
    for(std::size_t column = 0; column < dimension; ++column){
        std::size_t pivot = column;
        for(std::size_t row = column + 1; row < dimension; ++row){
            if(std::fabs(matrix[pivot][column])
               < std::fabs(matrix[row][column])){
                pivot = row;
            }
        }
        if(std::fabs(matrix[pivot][column]) <= 1e-30L){
            throw std::runtime_error(
                "active submodular bases are numerically singular"
            );
        }
        std::swap(matrix[pivot], matrix[column]);
        const long double inverse = 1.0L / matrix[column][column];
        for(std::size_t entry = column;
            entry <= dimension;
            ++entry){
            matrix[column][entry] *= inverse;
        }
        for(std::size_t row = 0; row < dimension; ++row){
            if(row == column) continue;
            const long double factor = matrix[row][column];
            for(std::size_t entry = column;
                entry <= dimension;
                ++entry){
                matrix[row][entry] -= factor * matrix[column][entry];
            }
        }
    }
    std::vector<long double> coefficient(count);
    for(std::size_t index = 0; index < count; ++index){
        coefficient[index] = matrix[index][dimension];
    }
    return coefficient;
}

inline std::vector<long double> convex_combination(
    const std::vector<std::vector<long double>>& points,
    const std::vector<long double>& coefficient
) {
    std::vector<long double> result(
        points.empty() ? 0 : points.front().size()
    );
    for(std::size_t point = 0; point < points.size(); ++point){
        for(std::size_t coordinate = 0;
            coordinate < result.size();
            ++coordinate){
            result[coordinate] +=
                coefficient[point] * points[point][coordinate];
        }
    }
    return result;
}

}  // namespace submodular_minimization_internal

template<class Oracle>
auto submodular_function_minimization(
    std::size_t ground_set_size,
    Oracle oracle,
    std::size_t maximum_iterations = 10'000,
    long double tolerance = 1e-12L
) {
    using Value = std::decay_t<std::invoke_result_t<
        Oracle&, const std::vector<unsigned char>&
    >>;
    if(maximum_iterations == 0 || !(tolerance > 0.0L)
       || !std::isfinite(tolerance)){
        throw std::invalid_argument(
            "invalid submodular minimization stopping parameters"
        );
    }
    SubmodularMinimizationResult<Value> result;
    std::vector<unsigned char> empty(ground_set_size, 0);
    result.value = std::invoke(oracle, empty);
    if(ground_set_size == 0) return result;

    std::vector<long double> initial_weight(ground_set_size);
    std::iota(initial_weight.begin(), initial_weight.end(), 0.0L);
    std::vector<std::vector<long double>> active{
        submodular_minimization_internal::greedy_base(
            ground_set_size, initial_weight, oracle
        )
    };
    std::vector<long double> coefficient{1.0L};
    std::vector<long double> point = active.front();

    for(std::size_t iteration = 0;
        iteration < maximum_iterations;
        ++iteration){
        result.iterations = iteration + 1;
        std::vector<long double> extreme =
            submodular_minimization_internal::greedy_base(
                ground_set_size, point, oracle
            );
        const long double norm =
            submodular_minimization_internal::dot(point, point);
        const long double support =
            submodular_minimization_internal::dot(point, extreme);
        result.dual_gap = norm - support;
        if(result.dual_gap
           <= tolerance * std::max(1.0L, std::fabs(norm))){
            result.minimum_norm_converged = true;
            break;
        }
        bool duplicate = false;
        for(const auto& old : active){
            long double difference = 0.0L;
            for(std::size_t index = 0;
                index < ground_set_size;
                ++index){
                difference = std::max(
                    difference, std::fabs(old[index] - extreme[index])
                );
            }
            if(difference <= tolerance){
                duplicate = true;
                break;
            }
        }
        if(duplicate) break;
        active.push_back(std::move(extreme));
        coefficient.push_back(0.0L);

        while(true){
            const std::vector<long double> affine =
                submodular_minimization_internal::affine_minimizer(active);
            bool all_positive = true;
            for(const long double value : affine){
                if(value <= tolerance){
                    all_positive = false;
                    break;
                }
            }
            if(all_positive){
                coefficient = affine;
                point =
                    submodular_minimization_internal::convex_combination(
                        active, coefficient
                    );
                break;
            }
            long double step = 1.0L;
            for(std::size_t index = 0;
                index < coefficient.size();
                ++index){
                if(affine[index] >= coefficient[index]) continue;
                step = std::min(
                    step,
                    coefficient[index]
                        / (coefficient[index] - affine[index])
                );
            }
            for(std::size_t index = 0;
                index < coefficient.size();
                ++index){
                coefficient[index] +=
                    step * (affine[index] - coefficient[index]);
            }
            for(std::size_t offset = 0;
                offset < coefficient.size();){
                if(coefficient[offset] > tolerance){
                    ++offset;
                    continue;
                }
                coefficient.erase(
                    coefficient.begin()
                    + static_cast<std::vector<long double>::difference_type>(
                        offset
                    )
                );
                active.erase(
                    active.begin()
                    + static_cast<
                        std::vector<std::vector<long double>>::difference_type
                    >(offset)
                );
            }
            point = submodular_minimization_internal::convex_combination(
                active, coefficient
            );
        }
    }
    result.minimum_norm_point = point;

    std::vector<std::size_t> order(ground_set_size);
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(
        order.begin(), order.end(),
        [&](std::size_t first, std::size_t second){
            return point[first] < point[second];
        }
    );
    std::vector<unsigned char> candidate(ground_set_size, 0);
    for(const std::size_t element : order){
        candidate[element] = 1;
        const Value value = std::invoke(oracle, candidate);
        if(value < result.value){
            result.value = value;
            result.elements.clear();
            for(std::size_t index = 0;
                index < ground_set_size;
                ++index){
                if(candidate[index]) result.elements.push_back(index);
            }
        }
    }

    std::vector<unsigned char> exact_set(ground_set_size, 0);
    while(true){
        const Value exact_value = std::invoke(oracle, exact_set);
        if(exact_value < result.value){
            result.value = exact_value;
            result.elements.clear();
            for(std::size_t index = 0;
                index < ground_set_size;
                ++index){
                if(exact_set[index]) result.elements.push_back(index);
            }
        }
        std::size_t changed = 0;
        while(changed < ground_set_size && exact_set[changed]){
            exact_set[changed] = 0;
            ++changed;
        }
        if(changed == ground_set_size) break;
        exact_set[changed] = 1;
    }
    return result;
}

}  // namespace combinatorial_optimization

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_SUBMODULAR_SUBMODULAR_FUNCTION_MINIMIZATION_HPP_INCLUDED
