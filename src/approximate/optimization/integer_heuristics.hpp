#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_INTEGER_HEURISTICS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_INTEGER_HEURISTICS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

enum class IntegerNeighborhoodPurpose{
    local_branching,
    rins,
    relax_and_fix,
    fix_and_optimize,
    proximity
};

template<class Int, class Value>
struct IntegerNeighborhoodRequest{
    std::vector<std::optional<Int>> fixed;
    std::vector<Int> reference;
    std::size_t maximum_distance = std::numeric_limits<std::size_t>::max();
    std::optional<Value> objective_upper_bound;
    IntegerNeighborhoodPurpose purpose = IntegerNeighborhoodPurpose::local_branching;
};

template<class Int, class Value>
struct IntegerHeuristicResult{
    std::vector<Int> solution;
    std::optional<Value> objective;
    std::size_t iterations = 0;
    std::size_t candidates = 0;
    bool feasible = false;

    explicit operator bool() const noexcept{
        return feasible;
    }
};

namespace integer_heuristics_internal{

template<class Int>
void validate_bounds(
    const std::vector<Int>& lower,
    const std::vector<Int>& upper
){
    static_assert(std::is_integral_v<Int>);
    if(lower.size() != upper.size())[[unlikely]]{
        throw std::invalid_argument("integer heuristic bound sizes differ");
    }
    for(std::size_t i = 0; i < lower.size(); ++i){
        if(upper[i] < lower[i])[[unlikely]]{
            throw std::invalid_argument("integer heuristic has an empty bound");
        }
    }
}

template<class Int>
void validate_vector(
    const std::vector<Int>& values,
    const std::vector<Int>& lower,
    const std::vector<Int>& upper
){
    if(values.size() != lower.size())[[unlikely]]{
        throw std::invalid_argument("integer heuristic candidate size differs");
    }
    for(std::size_t i = 0; i < values.size(); ++i){
        if(values[i] < lower[i] || upper[i] < values[i])[[unlikely]]{
            throw std::invalid_argument("integer heuristic candidate is out of bounds");
        }
    }
}

template<class Int>
std::vector<Int> rounded(
    const std::vector<long double>& relaxation,
    const std::vector<Int>& lower,
    const std::vector<Int>& upper
){
    if(relaxation.size() != lower.size())[[unlikely]]{
        throw std::invalid_argument("integer heuristic relaxation size differs");
    }
    std::vector<Int> result(relaxation.size());
    for(std::size_t i = 0; i < relaxation.size(); ++i){
        if(!std::isfinite(relaxation[i]))[[unlikely]]{
            throw std::invalid_argument(
                "integer heuristic requires finite relaxation values"
            );
        }
        const long double clipped = std::clamp(
            relaxation[i],
            static_cast<long double>(lower[i]),
            static_cast<long double>(upper[i])
        );
        result[i] = static_cast<Int>(std::round(clipped));
    }
    return result;
}

template<class Int, class Value, class Feasible, class Objective>
bool consider(
    const std::vector<Int>& candidate,
    Feasible& feasible,
    Objective& objective,
    IntegerHeuristicResult<Int, Value>& result
){
    ++result.candidates;
    if(!static_cast<bool>(std::invoke(feasible, candidate))) return false;
    const Value value = static_cast<Value>(std::invoke(objective, candidate));
    if(!result.objective || value < *result.objective){
        result.solution = candidate;
        result.objective = value;
        result.feasible = true;
        return true;
    }
    return false;
}

template<class Int>
void validate_blocks(
    std::size_t size,
    const std::vector<std::vector<std::size_t>>& blocks
){
    std::vector<unsigned char> seen(size);
    for(const auto& block : blocks){
        for(const std::size_t index : block){
            if(index >= size)[[unlikely]]{
                throw std::out_of_range("integer heuristic block index is out of range");
            }
            if(seen[index])[[unlikely]]{
                throw std::invalid_argument("integer heuristic blocks overlap");
            }
            seen[index] = 1;
        }
    }
}

}  // namespace integer_heuristics_internal

template<class Int, class Solver, class Feasible, class Objective>
auto local_branching(
    const std::vector<Int>& incumbent,
    std::size_t radius,
    Solver solve,
    Feasible feasible,
    Objective objective,
    std::size_t maximum_iterations = 1
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    Result result;
    integer_heuristics_internal::consider(
        incumbent, feasible, objective, result
    );
    if(!result.feasible)[[unlikely]]{
        throw std::invalid_argument("local_branching requires a feasible incumbent");
    }
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        IntegerNeighborhoodRequest<Int, Value> request;
        request.fixed.resize(incumbent.size());
        request.reference = result.solution;
        request.maximum_distance = radius;
        request.objective_upper_bound = result.objective;
        request.purpose = IntegerNeighborhoodPurpose::local_branching;
        ++result.iterations;
        auto candidate = std::invoke(solve, request);
        if(!candidate) break;
        if(candidate->size() != incumbent.size())[[unlikely]]{
            throw std::invalid_argument("local_branching solver returned wrong size");
        }
        if(!integer_heuristics_internal::consider(
               *candidate, feasible, objective, result
           )){
            break;
        }
    }
    return result;
}

template<class Int, class Solver, class Feasible, class Objective>
auto rins(
    const std::vector<Int>& incumbent,
    const std::vector<long double>& relaxation,
    Solver solve,
    Feasible feasible,
    Objective objective,
    long double equality_tolerance = 1.0e-9L
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    if(relaxation.size() != incumbent.size())[[unlikely]]{
        throw std::invalid_argument("rins relaxation size differs");
    }
    if(equality_tolerance < 0.0L)[[unlikely]]{
        throw std::invalid_argument("rins requires a non-negative tolerance");
    }
    Result result;
    integer_heuristics_internal::consider(
        incumbent, feasible, objective, result
    );
    if(!result.feasible)[[unlikely]]{
        throw std::invalid_argument("rins requires a feasible incumbent");
    }
    IntegerNeighborhoodRequest<Int, Value> request;
    request.fixed.resize(incumbent.size());
    request.reference = incumbent;
    request.objective_upper_bound = result.objective;
    request.purpose = IntegerNeighborhoodPurpose::rins;
    for(std::size_t i = 0; i < incumbent.size(); ++i){
        if(!std::isfinite(relaxation[i]))[[unlikely]]{
            throw std::invalid_argument("rins requires finite relaxation values");
        }
        if(std::fabs(relaxation[i] - static_cast<long double>(incumbent[i]))
           <= equality_tolerance){
            request.fixed[i] = incumbent[i];
        }
    }
    ++result.iterations;
    auto candidate = std::invoke(solve, request);
    if(candidate){
        if(candidate->size() != incumbent.size())[[unlikely]]{
            throw std::invalid_argument("rins solver returned wrong size");
        }
        integer_heuristics_internal::consider(
            *candidate, feasible, objective, result
        );
    }
    return result;
}

template<class Int, class Feasible, class Objective, class Project>
auto feasibility_pump(
    std::vector<long double> relaxation,
    const std::vector<Int>& lower,
    const std::vector<Int>& upper,
    Feasible feasible,
    Objective objective,
    Project project,
    std::size_t maximum_iterations
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    integer_heuristics_internal::validate_bounds(lower, upper);
    if(relaxation.size() != lower.size())[[unlikely]]{
        throw std::invalid_argument("feasibility_pump relaxation size differs");
    }
    Result result;
    std::set<std::vector<Int>> seen;
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        ++result.iterations;
        std::vector<Int> rounded = integer_heuristics_internal::rounded(
            relaxation, lower, upper
        );
        const bool repeated = !seen.insert(rounded).second;
        if(repeated && !rounded.empty()){
            const std::size_t index = iteration % rounded.size();
            if(rounded[index] < upper[index]){
                ++rounded[index];
            }else if(lower[index] < rounded[index]){
                --rounded[index];
            }
        }
        ++result.candidates;
        if(static_cast<bool>(std::invoke(feasible, rounded))){
            result.solution = rounded;
            result.objective = static_cast<Value>(
                std::invoke(objective, rounded)
            );
            result.feasible = true;
            return result;
        }
        auto projected = std::invoke(project, rounded);
        if(!projected) break;
        if(projected->size() != relaxation.size())[[unlikely]]{
            throw std::invalid_argument(
                "feasibility_pump projection returned wrong size"
            );
        }
        for(const long double value : *projected){
            if(!std::isfinite(value))[[unlikely]]{
                throw std::invalid_argument(
                    "feasibility_pump projection returned a non-finite value"
                );
            }
        }
        relaxation = std::move(*projected);
    }
    return result;
}

template<class Int, class Solver, class Feasible, class Objective>
auto relax_and_fix(
    std::size_t variable_count,
    const std::vector<std::vector<std::size_t>>& blocks,
    Solver solve,
    Feasible feasible,
    Objective objective
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    integer_heuristics_internal::validate_blocks<Int>(variable_count, blocks);
    Result result;
    std::vector<std::optional<Int>> fixed(variable_count);
    std::vector<Int> current(variable_count);
    for(const auto& block : blocks){
        IntegerNeighborhoodRequest<Int, Value> request;
        request.fixed = fixed;
        request.reference = current;
        request.purpose = IntegerNeighborhoodPurpose::relax_and_fix;
        ++result.iterations;
        auto candidate = std::invoke(solve, request);
        if(!candidate) return result;
        if(candidate->size() != variable_count)[[unlikely]]{
            throw std::invalid_argument(
                "relax_and_fix solver returned wrong size"
            );
        }
        current = std::move(*candidate);
        for(const std::size_t index : block) fixed[index] = current[index];
    }
    integer_heuristics_internal::consider(
        current, feasible, objective, result
    );
    return result;
}

template<class Int, class Solver, class Feasible, class Objective>
auto fix_and_optimize(
    const std::vector<Int>& incumbent,
    const std::vector<std::vector<std::size_t>>& blocks,
    Solver solve,
    Feasible feasible,
    Objective objective,
    std::size_t maximum_passes = 1
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    integer_heuristics_internal::validate_blocks<Int>(incumbent.size(), blocks);
    Result result;
    integer_heuristics_internal::consider(
        incumbent, feasible, objective, result
    );
    if(!result.feasible)[[unlikely]]{
        throw std::invalid_argument(
            "fix_and_optimize requires a feasible incumbent"
        );
    }
    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        bool improved = false;
        for(const auto& block : blocks){
            std::vector<unsigned char> free(incumbent.size());
            for(const std::size_t index : block) free[index] = 1;
            IntegerNeighborhoodRequest<Int, Value> request;
            request.fixed.resize(incumbent.size());
            for(std::size_t i = 0; i < incumbent.size(); ++i){
                if(!free[i]) request.fixed[i] = result.solution[i];
            }
            request.reference = result.solution;
            request.objective_upper_bound = result.objective;
            request.purpose = IntegerNeighborhoodPurpose::fix_and_optimize;
            ++result.iterations;
            auto candidate = std::invoke(solve, request);
            if(!candidate) continue;
            if(candidate->size() != incumbent.size())[[unlikely]]{
                throw std::invalid_argument(
                    "fix_and_optimize solver returned wrong size"
                );
            }
            improved |= integer_heuristics_internal::consider(
                *candidate, feasible, objective, result
            );
        }
        if(!improved) break;
    }
    return result;
}

template<class Int, class RelaxationSolver, class Feasible, class Objective>
auto diving_heuristic(
    const std::vector<Int>& lower,
    const std::vector<Int>& upper,
    RelaxationSolver solve_relaxation,
    Feasible feasible,
    Objective objective,
    std::size_t maximum_fixings = std::numeric_limits<std::size_t>::max(),
    long double integrality_tolerance = 1.0e-9L
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    integer_heuristics_internal::validate_bounds(lower, upper);
    if(integrality_tolerance < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "diving_heuristic requires a non-negative tolerance"
        );
    }
    Result result;
    std::vector<std::optional<Int>> fixed(lower.size());
    for(std::size_t fixing = 0; fixing <= maximum_fixings; ++fixing){
        ++result.iterations;
        auto relaxation = std::invoke(solve_relaxation, fixed);
        if(!relaxation) return result;
        std::vector<Int> rounded = integer_heuristics_internal::rounded(
            *relaxation, lower, upper
        );
        ++result.candidates;
        if(static_cast<bool>(std::invoke(feasible, rounded))){
            result.solution = rounded;
            result.objective = static_cast<Value>(
                std::invoke(objective, rounded)
            );
            result.feasible = true;
            return result;
        }
        if(fixing == maximum_fixings) break;
        std::size_t selected = lower.size();
        long double best_fractionality = std::numeric_limits<long double>::infinity();
        for(std::size_t i = 0; i < lower.size(); ++i){
            if(fixed[i]) continue;
            const long double fractionality = std::fabs(
                (*relaxation)[i] - std::round((*relaxation)[i])
            );
            if(fractionality <= integrality_tolerance) continue;
            if(fractionality < best_fractionality){
                best_fractionality = fractionality;
                selected = i;
            }
        }
        if(selected == lower.size()) break;
        fixed[selected] = rounded[selected];
    }
    return result;
}

template<class Int, class Feasible, class Objective, class Repair>
auto rounding_heuristic(
    const std::vector<long double>& relaxation,
    const std::vector<Int>& lower,
    const std::vector<Int>& upper,
    Feasible feasible,
    Objective objective,
    Repair repair
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    integer_heuristics_internal::validate_bounds(lower, upper);
    Result result;
    ++result.iterations;
    std::vector<Int> candidate = integer_heuristics_internal::rounded(
        relaxation, lower, upper
    );
    if(integer_heuristics_internal::consider(
           candidate, feasible, objective, result
       )){
        return result;
    }
    auto repaired = std::invoke(repair, candidate);
    if(!repaired) return result;
    integer_heuristics_internal::validate_vector(*repaired, lower, upper);
    integer_heuristics_internal::consider(
        *repaired, feasible, objective, result
    );
    return result;
}

template<class Int, class Solver, class Feasible, class Objective>
auto proximity_search(
    const std::vector<Int>& incumbent,
    std::size_t maximum_distance,
    Solver solve,
    Feasible feasible,
    Objective objective,
    std::size_t maximum_iterations = 1
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    Result result;
    integer_heuristics_internal::consider(
        incumbent, feasible, objective, result
    );
    if(!result.feasible)[[unlikely]]{
        throw std::invalid_argument(
            "proximity_search requires a feasible incumbent"
        );
    }
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        IntegerNeighborhoodRequest<Int, Value> request;
        request.fixed.resize(incumbent.size());
        request.reference = result.solution;
        request.maximum_distance = maximum_distance;
        request.objective_upper_bound = result.objective;
        request.purpose = IntegerNeighborhoodPurpose::proximity;
        ++result.iterations;
        auto candidate = std::invoke(solve, request);
        if(!candidate) break;
        if(candidate->size() != incumbent.size())[[unlikely]]{
            throw std::invalid_argument(
                "proximity_search solver returned wrong size"
            );
        }
        if(!integer_heuristics_internal::consider(
               *candidate, feasible, objective, result
           )){
            break;
        }
    }
    return result;
}

template<class Int, class Feasible, class Objective>
auto solution_polishing(
    const std::vector<Int>& incumbent,
    const std::vector<Int>& lower,
    const std::vector<Int>& upper,
    Feasible feasible,
    Objective objective,
    std::size_t maximum_passes = 1
){
    using Value = std::decay_t<std::invoke_result_t<
        Objective&, const std::vector<Int>&
    >>;
    using Result = IntegerHeuristicResult<Int, Value>;
    integer_heuristics_internal::validate_bounds(lower, upper);
    integer_heuristics_internal::validate_vector(incumbent, lower, upper);
    Result result;
    integer_heuristics_internal::consider(
        incumbent, feasible, objective, result
    );
    if(!result.feasible)[[unlikely]]{
        throw std::invalid_argument(
            "solution_polishing requires a feasible incumbent"
        );
    }
    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        ++result.iterations;
        std::vector<Int> pass_best = result.solution;
        std::optional<Value> pass_value = result.objective;
        for(std::size_t first = 0; first < incumbent.size(); ++first){
            for(const int first_delta : {-1, 1}){
                std::vector<Int> candidate = result.solution;
                if((first_delta < 0 && candidate[first] == lower[first])
                   || (first_delta > 0 && candidate[first] == upper[first])){
                    continue;
                }
                candidate[first] = static_cast<Int>(
                    candidate[first] + first_delta
                );
                ++result.candidates;
                if(static_cast<bool>(std::invoke(feasible, candidate))){
                    const Value value = static_cast<Value>(
                        std::invoke(objective, candidate)
                    );
                    if(!pass_value || value < *pass_value){
                        pass_best = candidate;
                        pass_value = value;
                    }
                }
                for(std::size_t second = first + 1;
                    second < incumbent.size();
                    ++second){
                    for(const int second_delta : {-1, 1}){
                        std::vector<Int> pair_candidate = candidate;
                        if((second_delta < 0
                            && pair_candidate[second] == lower[second])
                           || (second_delta > 0
                               && pair_candidate[second] == upper[second])){
                            continue;
                        }
                        pair_candidate[second] = static_cast<Int>(
                            pair_candidate[second] + second_delta
                        );
                        ++result.candidates;
                        if(!static_cast<bool>(
                               std::invoke(feasible, pair_candidate)
                           )){
                            continue;
                        }
                        const Value value = static_cast<Value>(
                            std::invoke(objective, pair_candidate)
                        );
                        if(!pass_value || value < *pass_value){
                            pass_best = std::move(pair_candidate);
                            pass_value = value;
                        }
                    }
                }
            }
        }
        if(!pass_value || !(*pass_value < *result.objective)) break;
        result.solution = std::move(pass_best);
        result.objective = *pass_value;
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_INTEGER_HEURISTICS_HPP_INCLUDED
