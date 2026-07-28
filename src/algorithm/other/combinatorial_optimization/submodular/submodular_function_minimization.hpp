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
    bool exact_certificate_used = false;
};

namespace submodular_minimization_internal {

template<class Value>
inline constexpr bool supports_integral_certificate =
    std::is_integral_v<Value> && sizeof(Value) <= sizeof(unsigned long long);

template<class Value>
struct GreedyBase {
    std::vector<long double> floating;
    std::vector<__int128_t> exact_integral;
};

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

template<class Value, class Oracle>
GreedyBase<Value> greedy_base(
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
    const Value initial = std::invoke(oracle, set);
    long double previous = static_cast<long double>(initial);
    if(!std::isfinite(previous)){
        throw std::invalid_argument(
            "submodular oracle returned a non-finite value"
        );
    }
    GreedyBase<Value> base;
    base.floating.resize(size);
    if constexpr(supports_integral_certificate<Value>){
        base.exact_integral.resize(size);
    }
    __int128_t previous_exact = 0;
    if constexpr(supports_integral_certificate<Value>){
        previous_exact = static_cast<__int128_t>(initial);
    }
    for(const std::size_t element : order){
        set[element] = 1;
        const Value current_value = std::invoke(oracle, set);
        const long double current = static_cast<long double>(current_value);
        if(!std::isfinite(current)){
            throw std::invalid_argument(
                "submodular oracle returned a non-finite value"
            );
        }
        base.floating[element] = current - previous;
        if constexpr(supports_integral_certificate<Value>){
            const __int128_t current_exact =
                static_cast<__int128_t>(current_value);
            base.exact_integral[element] = current_exact - previous_exact;
            previous_exact = current_exact;
        }
        previous = current;
    }
    return base;
}

inline std::vector<unsigned long long> dyadic_weights(
    const std::vector<long double>& coefficient
) {
    constexpr unsigned long long scale =
        static_cast<unsigned long long>(1) << 48;
    std::vector<unsigned long long> weight(coefficient.size());
    std::vector<std::pair<long double, std::size_t>> remainder;
    remainder.reserve(coefficient.size());
    long double sum = 0.0L;
    for(const long double value : coefficient){
        sum += std::max(0.0L, value);
    }
    if(!(sum > 0.0L) || !std::isfinite(sum)) return {};

    unsigned long long assigned = 0;
    for(std::size_t index = 0; index < coefficient.size(); ++index){
        const long double scaled =
            std::max(0.0L, coefficient[index]) / sum
            * static_cast<long double>(scale);
        unsigned long long integral =
            static_cast<unsigned long long>(std::floor(scaled));
        integral = std::min(integral, scale - assigned);
        weight[index] = integral;
        assigned += integral;
        remainder.emplace_back(scaled - std::floor(scaled), index);
    }
    std::stable_sort(
        remainder.begin(), remainder.end(),
        [](const auto& first, const auto& second){
            return first.first > second.first;
        }
    );
    for(std::size_t offset = 0; assigned < scale; ++assigned, ++offset){
        ++weight[remainder[offset % remainder.size()].second];
    }
    return weight;
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
    auto initial_base =
        submodular_minimization_internal::greedy_base<Value>(
            ground_set_size, initial_weight, oracle
        );
    std::vector<std::vector<long double>> active{
        std::move(initial_base.floating)
    };
    std::vector<std::vector<__int128_t>> exact_active;
    if constexpr(
        submodular_minimization_internal::supports_integral_certificate<Value>
    ){
        exact_active.push_back(std::move(initial_base.exact_integral));
    }
    std::vector<long double> coefficient{1.0L};
    std::vector<long double> point = active.front();

    for(std::size_t iteration = 0;
        iteration < maximum_iterations;
        ++iteration){
        result.iterations = iteration + 1;
        auto greedy =
            submodular_minimization_internal::greedy_base<Value>(
                ground_set_size, point, oracle
            );
        std::vector<long double> extreme = std::move(greedy.floating);
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
        if constexpr(
            submodular_minimization_internal::supports_integral_certificate<Value>
        ){
            exact_active.push_back(std::move(greedy.exact_integral));
        }
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
                if constexpr(
                    submodular_minimization_internal::
                        supports_integral_certificate<Value>
                ){
                    exact_active.erase(
                        exact_active.begin()
                        + static_cast<
                            std::vector<std::vector<__int128_t>>::
                                difference_type
                        >(offset)
                    );
                }
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
    std::vector<Value> chain_value;
    chain_value.reserve(ground_set_size + 1);
    chain_value.push_back(result.value);
    std::size_t last_decrease = 0;
    std::size_t first_increase = ground_set_size + 1;
    for(std::size_t offset = 0; offset < order.size(); ++offset){
        const std::size_t element = order[offset];
        candidate[element] = 1;
        const Value value = std::invoke(oracle, candidate);
        if(value < chain_value.back()) last_decrease = offset + 1;
        if(chain_value.back() < value){
            first_increase = std::min(first_increase, offset + 1);
        }
        chain_value.push_back(value);
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

    if(last_decrease < first_increase){
        candidate.assign(ground_set_size, 0);
        result.elements.clear();
        for(std::size_t offset = 0; offset < last_decrease; ++offset){
            candidate[order[offset]] = 1;
        }
        for(std::size_t element = 0; element < ground_set_size; ++element){
            if(candidate[element]) result.elements.push_back(element);
        }
        result.value = chain_value[last_decrease];
        result.exact_certificate_used = true;
        return result;
    }

    if constexpr(
        submodular_minimization_internal::supports_integral_certificate<Value>
    ){
        constexpr __int128_t scale = static_cast<__int128_t>(1) << 48;
        const auto weight =
            submodular_minimization_internal::dyadic_weights(coefficient);
        if(!weight.empty() && exact_active.size() == weight.size()){
            std::vector<__int128_t> aggregate(ground_set_size);
            for(std::size_t base = 0; base < exact_active.size(); ++base){
                for(std::size_t element = 0;
                    element < ground_set_size;
                    ++element){
                    aggregate[element] +=
                        static_cast<__int128_t>(weight[base])
                        * exact_active[base][element];
                }
            }
            candidate.assign(ground_set_size, 0);
            __int128_t lower_bound = 0;
            for(std::size_t element = 0;
                element < ground_set_size;
                ++element){
                if(aggregate[element] < 0){
                    candidate[element] = 1;
                    lower_bound += aggregate[element];
                }
            }
            const Value certified_value = std::invoke(oracle, candidate);
            const __int128_t normalized_value =
                static_cast<__int128_t>(certified_value)
                - static_cast<__int128_t>(chain_value.front());
            if(scale * normalized_value == lower_bound){
                result.value = certified_value;
                result.elements.clear();
                for(std::size_t element = 0;
                    element < ground_set_size;
                    ++element){
                    if(candidate[element]){
                        result.elements.push_back(element);
                    }
                }
                result.exact_certificate_used = true;
                return result;
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
