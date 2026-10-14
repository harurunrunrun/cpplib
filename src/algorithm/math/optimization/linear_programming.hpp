#ifndef CPPLIB_SRC_ALGORITHM_MATH_OPTIMIZATION_LINEAR_PROGRAMMING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_OPTIMIZATION_LINEAR_PROGRAMMING_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

enum LinearProgrammingStatus{
    LINEAR_PROGRAMMING_OPTIMAL,
    LINEAR_PROGRAMMING_INFEASIBLE,
    LINEAR_PROGRAMMING_UNBOUNDED,
};

template<std::size_t MAX_VARIABLE = static_cast<std::size_t>(-1)>
struct LinearProgrammingResult{
    LinearProgrammingStatus status = LINEAR_PROGRAMMING_INFEASIBLE;
    long double value = 0;
    std::array<long double, MAX_VARIABLE> solution{};
    int variable_count = 0;
};

template<>
struct LinearProgrammingResult<static_cast<std::size_t>(-1)>{
    LinearProgrammingStatus status = LINEAR_PROGRAMMING_INFEASIBLE;
    long double value = 0;
    std::vector<long double> solution{};
    int variable_count = 0;
};

template<std::size_t MAX_CONSTRAINT = static_cast<std::size_t>(-1), std::size_t MAX_VARIABLE = static_cast<std::size_t>(-1)>
struct LinearProgramming{
    static constexpr long double EPS = 1e-12L;

private:
    int variable_count_ = 0;
    int constraint_count_ = 0;
    std::array<std::array<long double, MAX_VARIABLE>, MAX_CONSTRAINT>
        coefficients_{};
    std::array<long double, MAX_CONSTRAINT> bounds_{};
    std::array<long double, MAX_VARIABLE> objective_{};

    struct Tableau{
        int constraint_count;
        int variable_count;
        std::array<int, MAX_CONSTRAINT> basis{};
        std::array<int, MAX_VARIABLE + 1> nonbasis{};
        std::array<
            std::array<long double, MAX_VARIABLE + 2>,
            MAX_CONSTRAINT + 2
        > data{};

        Tableau(
            int constraint_count_,
            int variable_count_,
            const std::array<
                std::array<long double, MAX_VARIABLE>,
                MAX_CONSTRAINT
            >& coefficients,
            const std::array<long double, MAX_CONSTRAINT>& bounds,
            const std::array<long double, MAX_VARIABLE>& objective
        ) : constraint_count(constraint_count_), variable_count(variable_count_){
            for(int row = 0; row < constraint_count; ++row){
                for(int column = 0; column < variable_count; ++column){
                    data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(column)] =
                        coefficients[static_cast<std::size_t>(row)]
                                    [static_cast<std::size_t>(column)];
                }
            }
            for(int row = 0; row < constraint_count; ++row){
                basis[static_cast<std::size_t>(row)] = variable_count + row;
                data[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(variable_count)] = -1;
                data[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(variable_count + 1)] =
                    bounds[static_cast<std::size_t>(row)];
            }
            for(int column = 0; column < variable_count; ++column){
                nonbasis[static_cast<std::size_t>(column)] = column;
                data[static_cast<std::size_t>(constraint_count)]
                    [static_cast<std::size_t>(column)] =
                    -objective[static_cast<std::size_t>(column)];
            }
            nonbasis[static_cast<std::size_t>(variable_count)] = -1;
            data[static_cast<std::size_t>(constraint_count + 1)]
                [static_cast<std::size_t>(variable_count)] = 1;
        }

        void pivot(int leaving, int entering){
            const std::size_t leaving_index = static_cast<std::size_t>(leaving);
            const std::size_t entering_index = static_cast<std::size_t>(entering);
            const long double inverse =
                1 / data[leaving_index][entering_index];

            for(int row = 0; row < constraint_count + 2; ++row){
                if(row == leaving) continue;
                for(int column = 0; column < variable_count + 2; ++column){
                    if(column == entering) continue;
                    data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(column)] -=
                        data[leaving_index][static_cast<std::size_t>(column)] *
                        data[static_cast<std::size_t>(row)][entering_index] *
                        inverse;
                }
            }
            for(int column = 0; column < variable_count + 2; ++column){
                if(column != entering){
                    data[leaving_index][static_cast<std::size_t>(column)] *=
                        inverse;
                }
            }
            for(int row = 0; row < constraint_count + 2; ++row){
                if(row != leaving){
                    data[static_cast<std::size_t>(row)][entering_index] *=
                        -inverse;
                }
            }
            data[leaving_index][entering_index] = inverse;
            std::swap(
                basis[static_cast<std::size_t>(leaving)],
                nonbasis[static_cast<std::size_t>(entering)]
            );
        }

        bool simplex(int phase){
            const int objective_row =
                phase == 1 ? constraint_count + 1 : constraint_count;
            while(true){
                int entering = -1;
                for(int column = 0; column <= variable_count; ++column){
                    if(
                        phase == 2 &&
                        nonbasis[static_cast<std::size_t>(column)] == -1
                    ){
                        continue;
                    }
                    if(
                        entering == -1 ||
                        data[static_cast<std::size_t>(objective_row)]
                            [static_cast<std::size_t>(column)] <
                            data[static_cast<std::size_t>(objective_row)]
                                [static_cast<std::size_t>(entering)] - EPS ||
                        (
                            std::abs(
                                data[static_cast<std::size_t>(objective_row)]
                                    [static_cast<std::size_t>(column)] -
                                data[static_cast<std::size_t>(objective_row)]
                                    [static_cast<std::size_t>(entering)]
                            ) <= EPS &&
                            nonbasis[static_cast<std::size_t>(column)] <
                                nonbasis[static_cast<std::size_t>(entering)]
                        )
                    ){
                        entering = column;
                    }
                }
                if(
                    entering == -1 ||
                    data[static_cast<std::size_t>(objective_row)]
                        [static_cast<std::size_t>(entering)] >= -EPS
                ){
                    return true;
                }

                int leaving = -1;
                for(int row = 0; row < constraint_count; ++row){
                    if(data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(entering)] <= EPS){
                        continue;
                    }
                    if(leaving == -1){
                        leaving = row;
                        continue;
                    }
                    const long double ratio =
                        data[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(variable_count + 1)] /
                        data[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(entering)];
                    const long double best_ratio =
                        data[static_cast<std::size_t>(leaving)]
                            [static_cast<std::size_t>(variable_count + 1)] /
                        data[static_cast<std::size_t>(leaving)]
                            [static_cast<std::size_t>(entering)];
                    if(
                        ratio < best_ratio - EPS ||
                        (std::abs(ratio - best_ratio) <= EPS &&
                            basis[static_cast<std::size_t>(row)] <
                                basis[static_cast<std::size_t>(leaving)])
                    ){
                        leaving = row;
                    }
                }
                if(leaving == -1) return false;
                pivot(leaving, entering);
            }
        }

        LinearProgrammingResult<MAX_VARIABLE> solve(){
            LinearProgrammingResult<MAX_VARIABLE> result;
            result.variable_count = variable_count;
            if(variable_count == 0){
                for(int row = 0; row < constraint_count; ++row){
                    if(data[static_cast<std::size_t>(row)][1] < -EPS){
                        result.status = LINEAR_PROGRAMMING_INFEASIBLE;
                        return result;
                    }
                }
                result.status = LINEAR_PROGRAMMING_OPTIMAL;
                return result;
            }

            int most_negative = -1;
            for(int row = 0; row < constraint_count; ++row){
                if(
                    most_negative == -1 ||
                    data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(variable_count + 1)] <
                        data[static_cast<std::size_t>(most_negative)]
                            [static_cast<std::size_t>(variable_count + 1)]
                ){
                    most_negative = row;
                }
            }
            if(
                most_negative != -1 &&
                data[static_cast<std::size_t>(most_negative)]
                    [static_cast<std::size_t>(variable_count + 1)] < -EPS
            ){
                pivot(most_negative, variable_count);
                if(!simplex(1) ||
                    data[static_cast<std::size_t>(constraint_count + 1)]
                        [static_cast<std::size_t>(variable_count + 1)] < -EPS
                ){
                    result.status = LINEAR_PROGRAMMING_INFEASIBLE;
                    return result;
                }
                if(std::abs(
                    data[static_cast<std::size_t>(constraint_count + 1)]
                        [static_cast<std::size_t>(variable_count + 1)]
                ) > EPS){
                    result.status = LINEAR_PROGRAMMING_INFEASIBLE;
                    return result;
                }
                for(int row = 0; row < constraint_count; ++row){
                    if(basis[static_cast<std::size_t>(row)] != -1) continue;
                    int entering = 0;
                    for(int column = 1; column <= variable_count; ++column){
                        if(
                            std::abs(data[static_cast<std::size_t>(row)]
                                [static_cast<std::size_t>(column)]) >
                                std::abs(data[static_cast<std::size_t>(row)]
                                    [static_cast<std::size_t>(entering)]) + EPS ||
                            (
                                std::abs(
                                    std::abs(data[static_cast<std::size_t>(row)]
                                        [static_cast<std::size_t>(column)]) -
                                    std::abs(data[static_cast<std::size_t>(row)]
                                        [static_cast<std::size_t>(entering)])
                                ) <= EPS &&
                                nonbasis[static_cast<std::size_t>(column)] <
                                    nonbasis[static_cast<std::size_t>(entering)]
                            )
                        ){
                            entering = column;
                        }
                    }
                    if(std::abs(data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(entering)]) > EPS){
                        pivot(row, entering);
                    }
                }
            }

            if(!simplex(2)){
                result.status = LINEAR_PROGRAMMING_UNBOUNDED;
                return result;
            }
            result.status = LINEAR_PROGRAMMING_OPTIMAL;
            result.value = data[static_cast<std::size_t>(constraint_count)]
                [static_cast<std::size_t>(variable_count + 1)];
            for(int row = 0; row < constraint_count; ++row){
                const int variable = basis[static_cast<std::size_t>(row)];
                if(0 <= variable && variable < variable_count){
                    result.solution[static_cast<std::size_t>(variable)] =
                        data[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(variable_count + 1)];
                }
            }
            return result;
        }
    };

    void check_finite(long double value) const{
        if(!std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument("linear programming coefficients must be finite");
        }
    }

public:
    explicit LinearProgramming(int variable_count)
        : variable_count_(variable_count){
        if(variable_count < 0 ||
            static_cast<std::size_t>(variable_count) > MAX_VARIABLE
        )[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: variable capacity exceeded (linear programming)."
            );
        }
    }

    int variable_count() const noexcept{ return variable_count_; }
    int constraint_count() const noexcept{ return constraint_count_; }

    void set_objective(
        const std::array<long double, MAX_VARIABLE>& coefficients
    ){
        for(int variable = 0; variable < variable_count_; ++variable){
            check_finite(coefficients[static_cast<std::size_t>(variable)]);
        }
        objective_ = coefficients;
    }

    void add_less_equal(
        const std::array<long double, MAX_VARIABLE>& coefficients,
        long double bound
    ){
        if(static_cast<std::size_t>(constraint_count_) == MAX_CONSTRAINT)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: constraint capacity exceeded "
                "(linear programming)."
            );
        }
        check_finite(bound);
        for(int variable = 0; variable < variable_count_; ++variable){
            check_finite(coefficients[static_cast<std::size_t>(variable)]);
        }
        coefficients_[static_cast<std::size_t>(constraint_count_)] = coefficients;
        bounds_[static_cast<std::size_t>(constraint_count_)] = bound;
        ++constraint_count_;
    }

    void add_greater_equal(
        std::array<long double, MAX_VARIABLE> coefficients,
        long double bound
    ){
        for(int variable = 0; variable < variable_count_; ++variable){
            coefficients[static_cast<std::size_t>(variable)] =
                -coefficients[static_cast<std::size_t>(variable)];
        }
        add_less_equal(coefficients, -bound);
    }

    void add_equal(
        const std::array<long double, MAX_VARIABLE>& coefficients,
        long double bound
    ){
        if(MAX_CONSTRAINT - static_cast<std::size_t>(constraint_count_) < 2)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: constraint capacity exceeded "
                "(linear programming)."
            );
        }
        for(int variable = 0; variable < variable_count_; ++variable){
            check_finite(coefficients[static_cast<std::size_t>(variable)]);
        }
        check_finite(bound);
        add_less_equal(coefficients, bound);
        std::array<long double, MAX_VARIABLE> negated = coefficients;
        for(int variable = 0; variable < variable_count_; ++variable){
            negated[static_cast<std::size_t>(variable)] =
                -negated[static_cast<std::size_t>(variable)];
        }
        add_less_equal(negated, -bound);
    }

    LinearProgrammingResult<MAX_VARIABLE> maximize() const{
        Tableau tableau(
            constraint_count_, variable_count_, coefficients_, bounds_, objective_
        );
        return tableau.solve();
    }

    LinearProgrammingResult<MAX_VARIABLE> minimize() const{
        auto negated_objective = objective_;
        for(int variable = 0; variable < variable_count_; ++variable){
            negated_objective[static_cast<std::size_t>(variable)] =
                -negated_objective[static_cast<std::size_t>(variable)];
        }
        Tableau tableau(
            constraint_count_, variable_count_, coefficients_, bounds_,
            negated_objective
        );
        auto result = tableau.solve();
        if(result.status == LINEAR_PROGRAMMING_OPTIMAL) result.value = -result.value;
        return result;
    }
};

template<>
struct LinearProgramming<static_cast<std::size_t>(-1), static_cast<std::size_t>(-1)>{
    static constexpr long double EPS = 1e-12L;

private:
    int variable_count_ = 0;
    int constraint_count_ = 0;
    std::vector<std::vector<long double>>
        coefficients_{};
    std::vector<long double> bounds_{};
    std::vector<long double> objective_{};

    struct Tableau{
        int constraint_count;
        int variable_count;
        std::vector<int> basis{};
        std::vector<int> nonbasis{};
        std::vector<std::vector<long double>> data{};

        Tableau(
            int constraint_count_,
            int variable_count_,
            const std::vector<std::vector<long double>>& coefficients,
            const std::vector<long double>& bounds,
            const std::vector<long double>& objective
        ) : constraint_count(constraint_count_), variable_count(variable_count_),
            basis(static_cast<std::size_t>(constraint_count_)),
            nonbasis(static_cast<std::size_t>(variable_count_) + 1),
            data(static_cast<std::size_t>(constraint_count_) + 2,
                 std::vector<long double>(static_cast<std::size_t>(variable_count_) + 2)){
            for(int row = 0; row < constraint_count; ++row){
                for(int column = 0; column < variable_count; ++column){
                    data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(column)] =
                        coefficients[static_cast<std::size_t>(row)]
                                    [static_cast<std::size_t>(column)];
                }
            }
            for(int row = 0; row < constraint_count; ++row){
                basis[static_cast<std::size_t>(row)] = variable_count + row;
                data[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(variable_count)] = -1;
                data[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(variable_count + 1)] =
                    bounds[static_cast<std::size_t>(row)];
            }
            for(int column = 0; column < variable_count; ++column){
                nonbasis[static_cast<std::size_t>(column)] = column;
                data[static_cast<std::size_t>(constraint_count)]
                    [static_cast<std::size_t>(column)] =
                    -objective[static_cast<std::size_t>(column)];
            }
            nonbasis[static_cast<std::size_t>(variable_count)] = -1;
            data[static_cast<std::size_t>(constraint_count + 1)]
                [static_cast<std::size_t>(variable_count)] = 1;
        }

        void pivot(int leaving, int entering){
            const std::size_t leaving_index = static_cast<std::size_t>(leaving);
            const std::size_t entering_index = static_cast<std::size_t>(entering);
            const long double inverse =
                1 / data[leaving_index][entering_index];

            for(int row = 0; row < constraint_count + 2; ++row){
                if(row == leaving) continue;
                for(int column = 0; column < variable_count + 2; ++column){
                    if(column == entering) continue;
                    data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(column)] -=
                        data[leaving_index][static_cast<std::size_t>(column)] *
                        data[static_cast<std::size_t>(row)][entering_index] *
                        inverse;
                }
            }
            for(int column = 0; column < variable_count + 2; ++column){
                if(column != entering){
                    data[leaving_index][static_cast<std::size_t>(column)] *=
                        inverse;
                }
            }
            for(int row = 0; row < constraint_count + 2; ++row){
                if(row != leaving){
                    data[static_cast<std::size_t>(row)][entering_index] *=
                        -inverse;
                }
            }
            data[leaving_index][entering_index] = inverse;
            std::swap(
                basis[static_cast<std::size_t>(leaving)],
                nonbasis[static_cast<std::size_t>(entering)]
            );
        }

        bool simplex(int phase){
            const int objective_row =
                phase == 1 ? constraint_count + 1 : constraint_count;
            while(true){
                int entering = -1;
                for(int column = 0; column <= variable_count; ++column){
                    if(
                        phase == 2 &&
                        nonbasis[static_cast<std::size_t>(column)] == -1
                    ){
                        continue;
                    }
                    if(
                        entering == -1 ||
                        data[static_cast<std::size_t>(objective_row)]
                            [static_cast<std::size_t>(column)] <
                            data[static_cast<std::size_t>(objective_row)]
                                [static_cast<std::size_t>(entering)] - EPS ||
                        (
                            std::abs(
                                data[static_cast<std::size_t>(objective_row)]
                                    [static_cast<std::size_t>(column)] -
                                data[static_cast<std::size_t>(objective_row)]
                                    [static_cast<std::size_t>(entering)]
                            ) <= EPS &&
                            nonbasis[static_cast<std::size_t>(column)] <
                                nonbasis[static_cast<std::size_t>(entering)]
                        )
                    ){
                        entering = column;
                    }
                }
                if(
                    entering == -1 ||
                    data[static_cast<std::size_t>(objective_row)]
                        [static_cast<std::size_t>(entering)] >= -EPS
                ){
                    return true;
                }

                int leaving = -1;
                for(int row = 0; row < constraint_count; ++row){
                    if(data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(entering)] <= EPS){
                        continue;
                    }
                    if(leaving == -1){
                        leaving = row;
                        continue;
                    }
                    const long double ratio =
                        data[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(variable_count + 1)] /
                        data[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(entering)];
                    const long double best_ratio =
                        data[static_cast<std::size_t>(leaving)]
                            [static_cast<std::size_t>(variable_count + 1)] /
                        data[static_cast<std::size_t>(leaving)]
                            [static_cast<std::size_t>(entering)];
                    if(
                        ratio < best_ratio - EPS ||
                        (std::abs(ratio - best_ratio) <= EPS &&
                            basis[static_cast<std::size_t>(row)] <
                                basis[static_cast<std::size_t>(leaving)])
                    ){
                        leaving = row;
                    }
                }
                if(leaving == -1) return false;
                pivot(leaving, entering);
            }
        }

        LinearProgrammingResult<static_cast<std::size_t>(-1)> solve(){
            LinearProgrammingResult<static_cast<std::size_t>(-1)> result;
            result.variable_count = variable_count;
            result.solution.resize(static_cast<std::size_t>(variable_count));
            if(variable_count == 0){
                for(int row = 0; row < constraint_count; ++row){
                    if(data[static_cast<std::size_t>(row)][1] < -EPS){
                        result.status = LINEAR_PROGRAMMING_INFEASIBLE;
                        return result;
                    }
                }
                result.status = LINEAR_PROGRAMMING_OPTIMAL;
                return result;
            }

            int most_negative = -1;
            for(int row = 0; row < constraint_count; ++row){
                if(
                    most_negative == -1 ||
                    data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(variable_count + 1)] <
                        data[static_cast<std::size_t>(most_negative)]
                            [static_cast<std::size_t>(variable_count + 1)]
                ){
                    most_negative = row;
                }
            }
            if(
                most_negative != -1 &&
                data[static_cast<std::size_t>(most_negative)]
                    [static_cast<std::size_t>(variable_count + 1)] < -EPS
            ){
                pivot(most_negative, variable_count);
                if(!simplex(1) ||
                    data[static_cast<std::size_t>(constraint_count + 1)]
                        [static_cast<std::size_t>(variable_count + 1)] < -EPS
                ){
                    result.status = LINEAR_PROGRAMMING_INFEASIBLE;
                    return result;
                }
                if(std::abs(
                    data[static_cast<std::size_t>(constraint_count + 1)]
                        [static_cast<std::size_t>(variable_count + 1)]
                ) > EPS){
                    result.status = LINEAR_PROGRAMMING_INFEASIBLE;
                    return result;
                }
                for(int row = 0; row < constraint_count; ++row){
                    if(basis[static_cast<std::size_t>(row)] != -1) continue;
                    int entering = 0;
                    for(int column = 1; column <= variable_count; ++column){
                        if(
                            std::abs(data[static_cast<std::size_t>(row)]
                                [static_cast<std::size_t>(column)]) >
                                std::abs(data[static_cast<std::size_t>(row)]
                                    [static_cast<std::size_t>(entering)]) + EPS ||
                            (
                                std::abs(
                                    std::abs(data[static_cast<std::size_t>(row)]
                                        [static_cast<std::size_t>(column)]) -
                                    std::abs(data[static_cast<std::size_t>(row)]
                                        [static_cast<std::size_t>(entering)])
                                ) <= EPS &&
                                nonbasis[static_cast<std::size_t>(column)] <
                                    nonbasis[static_cast<std::size_t>(entering)]
                            )
                        ){
                            entering = column;
                        }
                    }
                    if(std::abs(data[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(entering)]) > EPS){
                        pivot(row, entering);
                    }
                }
            }

            if(!simplex(2)){
                result.status = LINEAR_PROGRAMMING_UNBOUNDED;
                return result;
            }
            result.status = LINEAR_PROGRAMMING_OPTIMAL;
            result.value = data[static_cast<std::size_t>(constraint_count)]
                [static_cast<std::size_t>(variable_count + 1)];
            for(int row = 0; row < constraint_count; ++row){
                const int variable = basis[static_cast<std::size_t>(row)];
                if(0 <= variable && variable < variable_count){
                    result.solution[static_cast<std::size_t>(variable)] =
                        data[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(variable_count + 1)];
                }
            }
            return result;
        }
    };

    void check_finite(long double value) const{
        if(!std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument("linear programming coefficients must be finite");
        }
    }

public:
    explicit LinearProgramming(int variable_count)
        : variable_count_(variable_count){
        if(variable_count < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: variable capacity exceeded (linear programming)."
            );
        }
        objective_.resize(static_cast<std::size_t>(variable_count));
    }

    int variable_count() const noexcept{ return variable_count_; }
    int constraint_count() const noexcept{ return constraint_count_; }

    void set_objective(
        const std::vector<long double>& coefficients
    ){
        if(coefficients.size() != static_cast<std::size_t>(variable_count_))
            throw std::invalid_argument("linear programming coefficient shape");
        for(int variable = 0; variable < variable_count_; ++variable){
            check_finite(coefficients[static_cast<std::size_t>(variable)]);
        }
        objective_ = coefficients;
    }

    void add_less_equal(
        const std::vector<long double>& coefficients,
        long double bound
    ){
        if(constraint_count_ == std::numeric_limits<int>::max())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: constraint capacity exceeded "
                "(linear programming)."
            );
        }
        check_finite(bound);
        if(coefficients.size() != static_cast<std::size_t>(variable_count_))
            throw std::invalid_argument("linear programming coefficient shape");
        for(int variable = 0; variable < variable_count_; ++variable){
            check_finite(coefficients[static_cast<std::size_t>(variable)]);
        }
        coefficients_.push_back(coefficients);
        bounds_.push_back(bound);
        ++constraint_count_;
    }

    void add_greater_equal(
        std::vector<long double> coefficients,
        long double bound
    ){
        if(coefficients.size() != static_cast<std::size_t>(variable_count_))
            throw std::invalid_argument("linear programming coefficient shape");
        for(int variable = 0; variable < variable_count_; ++variable){
            coefficients[static_cast<std::size_t>(variable)] =
                -coefficients[static_cast<std::size_t>(variable)];
        }
        add_less_equal(coefficients, -bound);
    }

    void add_equal(
        const std::vector<long double>& coefficients,
        long double bound
    ){
        if(constraint_count_ > std::numeric_limits<int>::max() - 2)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: constraint capacity exceeded "
                "(linear programming)."
            );
        }
        if(coefficients.size() != static_cast<std::size_t>(variable_count_))
            throw std::invalid_argument("linear programming coefficient shape");
        for(int variable = 0; variable < variable_count_; ++variable){
            check_finite(coefficients[static_cast<std::size_t>(variable)]);
        }
        check_finite(bound);
        add_less_equal(coefficients, bound);
        std::vector<long double> negated = coefficients;
        if(coefficients.size() != static_cast<std::size_t>(variable_count_))
            throw std::invalid_argument("linear programming coefficient shape");
        for(int variable = 0; variable < variable_count_; ++variable){
            negated[static_cast<std::size_t>(variable)] =
                -negated[static_cast<std::size_t>(variable)];
        }
        add_less_equal(negated, -bound);
    }

    LinearProgrammingResult<static_cast<std::size_t>(-1)> maximize() const{
        Tableau tableau(
            constraint_count_, variable_count_, coefficients_, bounds_, objective_
        );
        return tableau.solve();
    }

    LinearProgrammingResult<static_cast<std::size_t>(-1)> minimize() const{
        auto negated_objective = objective_;
        for(int variable = 0; variable < variable_count_; ++variable){
            negated_objective[static_cast<std::size_t>(variable)] =
                -negated_objective[static_cast<std::size_t>(variable)];
        }
        Tableau tableau(
            constraint_count_, variable_count_, coefficients_, bounds_,
            negated_objective
        );
        auto result = tableau.solve();
        if(result.status == LINEAR_PROGRAMMING_OPTIMAL) result.value = -result.value;
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_OPTIMIZATION_LINEAR_PROGRAMMING_HPP_INCLUDED
