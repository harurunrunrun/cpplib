// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/linear_programming.hpp"

struct NaiveLinearProgram{
    int variable_count;
    std::vector<std::vector<long double>> coefficients;
    std::vector<long double> bounds;
    std::vector<long double> objective;
};

std::optional<std::vector<long double>> solve_square(
    std::vector<std::vector<long double>> matrix,
    std::vector<long double> values
){
    const int size = static_cast<int>(matrix.size());
    for(int column = 0; column < size; ++column){
        int pivot = column;
        for(int row = column + 1; row < size; ++row){
            if(std::abs(matrix[static_cast<std::size_t>(row)]
                [static_cast<std::size_t>(column)]) >
                std::abs(matrix[static_cast<std::size_t>(pivot)]
                    [static_cast<std::size_t>(column)])){
                pivot = row;
            }
        }
        if(std::abs(matrix[static_cast<std::size_t>(pivot)]
            [static_cast<std::size_t>(column)]) < 1e-12L){
            return std::nullopt;
        }
        std::swap(matrix[static_cast<std::size_t>(pivot)],
            matrix[static_cast<std::size_t>(column)]);
        std::swap(values[static_cast<std::size_t>(pivot)],
            values[static_cast<std::size_t>(column)]);
        const long double inverse = 1 / matrix[static_cast<std::size_t>(column)]
            [static_cast<std::size_t>(column)];
        for(int entry = column; entry < size; ++entry){
            matrix[static_cast<std::size_t>(column)]
                [static_cast<std::size_t>(entry)] *= inverse;
        }
        values[static_cast<std::size_t>(column)] *= inverse;
        for(int row = 0; row < size; ++row){
            if(row == column) continue;
            const long double factor = matrix[static_cast<std::size_t>(row)]
                [static_cast<std::size_t>(column)];
            for(int entry = column; entry < size; ++entry){
                matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(entry)] -=
                    factor * matrix[static_cast<std::size_t>(column)]
                        [static_cast<std::size_t>(entry)];
            }
            values[static_cast<std::size_t>(row)] -=
                factor * values[static_cast<std::size_t>(column)];
        }
    }
    return values;
}

long double naive_bounded_maximum(const NaiveLinearProgram& problem){
    const int variable_count = problem.variable_count;
    const int original_constraints = static_cast<int>(problem.bounds.size());
    const int total_constraints = original_constraints + variable_count;
    long double answer = -std::numeric_limits<long double>::infinity();
    std::vector<int> selected;

    const auto evaluate = [&](){
        std::vector<std::vector<long double>> matrix;
        std::vector<long double> values;
        for(int constraint: selected){
            if(constraint < original_constraints){
                matrix.push_back(problem.coefficients[
                    static_cast<std::size_t>(constraint)
                ]);
                values.push_back(problem.bounds[static_cast<std::size_t>(constraint)]);
            }else{
                std::vector<long double> row(
                    static_cast<std::size_t>(variable_count), 0
                );
                row[static_cast<std::size_t>(constraint - original_constraints)] = -1;
                matrix.push_back(std::move(row));
                values.push_back(0);
            }
        }
        const auto solution = solve_square(std::move(matrix), std::move(values));
        if(!solution) return;
        for(int variable = 0; variable < variable_count; ++variable){
            if((*solution)[static_cast<std::size_t>(variable)] < -1e-8L) return;
        }
        for(int constraint = 0; constraint < original_constraints; ++constraint){
            long double left = 0;
            for(int variable = 0; variable < variable_count; ++variable){
                left += problem.coefficients[static_cast<std::size_t>(constraint)]
                    [static_cast<std::size_t>(variable)] *
                    (*solution)[static_cast<std::size_t>(variable)];
            }
            if(left > problem.bounds[static_cast<std::size_t>(constraint)] + 1e-8L){
                return;
            }
        }
        long double value = 0;
        for(int variable = 0; variable < variable_count; ++variable){
            value += problem.objective[static_cast<std::size_t>(variable)] *
                (*solution)[static_cast<std::size_t>(variable)];
        }
        answer = std::max(answer, value);
    };

    const auto enumerate = [&](const auto& self, int next) -> void{
        if(static_cast<int>(selected.size()) == variable_count){
            evaluate();
            return;
        }
        const int need = variable_count - static_cast<int>(selected.size());
        for(int constraint = next;
            constraint + need <= total_constraints;
            ++constraint
        ){
            selected.push_back(constraint);
            self(self, constraint + 1);
            selected.pop_back();
        }
    };
    enumerate(enumerate, 0);
    return answer;
}

bool close_value(long double left, long double right){
    return std::abs(left - right) <=
        1e-8L * std::max({1.0L, std::abs(left), std::abs(right)});
}

void self_test(){
    {
        LinearProgramming<8, 2> problem(2);
        problem.set_objective({3, 2});
        problem.add_less_equal({1, 1}, 4);
        problem.add_less_equal({1, 0}, 2);
        problem.add_less_equal({0, 1}, 3);
        const auto result = problem.maximize();
        assert(result.status == LINEAR_PROGRAMMING_OPTIMAL);
        assert(close_value(result.value, 10));
        assert(close_value(result.solution[0], 2));
        assert(close_value(result.solution[1], 2));
    }
    {
        LinearProgramming<4, 1> problem(1);
        problem.set_objective({1});
        problem.add_greater_equal({1}, 2);
        problem.add_less_equal({1}, 1);
        assert(problem.maximize().status == LINEAR_PROGRAMMING_INFEASIBLE);
    }
    {
        LinearProgramming<1, 1> problem(1);
        problem.set_objective({1});
        assert(problem.maximize().status == LINEAR_PROGRAMMING_UNBOUNDED);
        problem.set_objective({-1});
        const auto result = problem.maximize();
        assert(result.status == LINEAR_PROGRAMMING_OPTIMAL);
        assert(close_value(result.value, 0));
    }
    {
        LinearProgramming<4, 2> problem(2);
        problem.set_objective({1, 1});
        problem.add_greater_equal({1, 1}, 3);
        const auto result = problem.minimize();
        assert(result.status == LINEAR_PROGRAMMING_OPTIMAL);
        assert(close_value(result.value, 3));
    }
    {
        LinearProgramming<4, 2> problem(2);
        problem.set_objective({2, -1});
        problem.add_equal({1, 1}, 5);
        problem.add_less_equal({1, 0}, 2);
        const auto result = problem.maximize();
        assert(result.status == LINEAR_PROGRAMMING_OPTIMAL);
        assert(close_value(result.value, 1));
        assert(close_value(result.solution[0] + result.solution[1], 5));
    }
    {
        LinearProgramming<2, 0> feasible(0);
        feasible.add_less_equal({}, 0);
        assert(feasible.maximize().status == LINEAR_PROGRAMMING_OPTIMAL);
        LinearProgramming<2, 0> infeasible(0);
        infeasible.add_less_equal({}, -1);
        assert(infeasible.maximize().status == LINEAR_PROGRAMMING_INFEASIBLE);
    }

    std::mt19937_64 random(2026071318);
    for(int iteration = 0; iteration < 3000; ++iteration){
        const int variable_count = 1 + static_cast<int>(random() % 4);
        LinearProgramming<16, 6> problem(variable_count);
        NaiveLinearProgram naive;
        naive.variable_count = variable_count;
        naive.objective.resize(static_cast<std::size_t>(variable_count));
        std::array<long double, 6> objective{};
        for(int variable = 0; variable < variable_count; ++variable){
            objective[static_cast<std::size_t>(variable)] =
                static_cast<long double>(static_cast<long long>(random() % 11) - 5);
            naive.objective[static_cast<std::size_t>(variable)] =
                objective[static_cast<std::size_t>(variable)];
        }
        problem.set_objective(objective);

        for(int variable = 0; variable < variable_count; ++variable){
            std::array<long double, 6> row{};
            row[static_cast<std::size_t>(variable)] = 1;
            const long double bound = 1 + static_cast<long double>(random() % 10);
            problem.add_less_equal(row, bound);
            naive.coefficients.emplace_back(
                static_cast<std::size_t>(variable_count), 0
            );
            naive.coefficients.back()[static_cast<std::size_t>(variable)] = 1;
            naive.bounds.push_back(bound);
        }
        const int additional = static_cast<int>(random() % 5);
        for(int constraint = 0; constraint < additional; ++constraint){
            std::array<long double, 6> row{};
            std::vector<long double> naive_row(
                static_cast<std::size_t>(variable_count)
            );
            for(int variable = 0; variable < variable_count; ++variable){
                const long double value = static_cast<long double>(
                    static_cast<long long>(random() % 9) - 4
                );
                row[static_cast<std::size_t>(variable)] = value;
                naive_row[static_cast<std::size_t>(variable)] = value;
            }
            const long double bound = static_cast<long double>(random() % 21);
            problem.add_less_equal(row, bound);
            naive.coefficients.push_back(std::move(naive_row));
            naive.bounds.push_back(bound);
        }

        const auto result = problem.maximize();
        assert(result.status == LINEAR_PROGRAMMING_OPTIMAL);
        const long double expected = naive_bounded_maximum(naive);
        assert(std::isfinite(expected));
        assert(close_value(result.value, expected));
        for(int variable = 0; variable < variable_count; ++variable){
            assert(result.solution[static_cast<std::size_t>(variable)] >= -1e-8L);
        }
        for(std::size_t constraint = 0; constraint < naive.bounds.size(); ++constraint){
            long double left = 0;
            for(int variable = 0; variable < variable_count; ++variable){
                left += naive.coefficients[constraint]
                    [static_cast<std::size_t>(variable)] *
                    result.solution[static_cast<std::size_t>(variable)];
            }
            assert(left <= naive.bounds[constraint] + 1e-7L);
        }
    }

    constexpr int large_variables = 150;
    static LinearProgramming<320, 160> large(large_variables);
    std::array<long double, 160> objective{};
    long double expected = 0;
    for(int variable = 0; variable < large_variables; ++variable){
        objective[static_cast<std::size_t>(variable)] = variable + 1;
        std::array<long double, 160> row{};
        row[static_cast<std::size_t>(variable)] = 1;
        const long double bound = variable % 17 + 1;
        large.add_less_equal(row, bound);
        expected += (variable + 1) * bound;
    }
    large.set_objective(objective);
    const auto large_result = large.maximize();
    assert(large_result.status == LINEAR_PROGRAMMING_OPTIMAL);
    assert(close_value(large_result.value, expected));
}

long long scaled_round(long double value){
    return std::llround(value * 1000000);
}

int main(){
    int constraint_count;
    int variable_count;
    if(!(std::cin >> constraint_count >> variable_count)){
        self_test();
        return 0;
    }
    LinearProgramming<12, 8> problem(variable_count);
    std::vector<std::array<long double, 8>> rows(
        static_cast<std::size_t>(constraint_count)
    );
    std::vector<long double> bounds(static_cast<std::size_t>(constraint_count));
    for(int constraint = 0; constraint < constraint_count; ++constraint){
        for(int variable = 0; variable < variable_count; ++variable){
            std::cin >> rows[static_cast<std::size_t>(constraint)]
                [static_cast<std::size_t>(variable)];
        }
        std::cin >> bounds[static_cast<std::size_t>(constraint)];
        problem.add_less_equal(
            rows[static_cast<std::size_t>(constraint)],
            bounds[static_cast<std::size_t>(constraint)]
        );
    }
    std::array<long double, 8> objective{};
    for(int variable = 0; variable < variable_count; ++variable){
        std::cin >> objective[static_cast<std::size_t>(variable)];
    }
    problem.set_objective(objective);
    const auto result = problem.maximize();
    if(result.status == LINEAR_PROGRAMMING_INFEASIBLE){
        std::cout << "INFEASIBLE\n";
    }else if(result.status == LINEAR_PROGRAMMING_UNBOUNDED){
        std::cout << "UNBOUNDED\n";
    }else{
        std::cout << "OPTIMAL\n" << scaled_round(result.value) << '\n';
    }
}
