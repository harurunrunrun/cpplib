// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/elimination/bareiss_elimination.hpp"
#include "../../src/algorithm/math/linear_algebra/integer_normal_forms/hermite_normal_form.hpp"
#include "../../src/algorithm/math/linear_algebra/integer_normal_forms/smith_normal_form.hpp"
#include "../../src/algorithm/math/linear_algebra/lattice/lll_lattice_reduction.hpp"
#include "../../src/algorithm/math/number_theory/finite_field/finite_field_extension.hpp"
#include "../../src/algorithm/math/number_theory/prime/quadratic_sieve.hpp"

namespace {

using Matrix = std::vector<std::vector<long long>>;

Matrix multiply(const Matrix& first, const Matrix& second) {
    assert(first.empty() || first.front().size() == second.size());
    Matrix result(
        first.size(),
        std::vector<long long>(
            second.empty() ? 0 : second.front().size()
        )
    );
    for(std::size_t i = 0; i < first.size(); ++i){
        for(std::size_t k = 0; k < second.size(); ++k){
            for(std::size_t j = 0; j < result[i].size(); ++j){
                const __int128 value =
                    static_cast<__int128>(result[i][j])
                    + static_cast<__int128>(first[i][k]) * second[k][j];
                assert(value >= -9'000'000'000'000'000'000LL);
                assert(value <= 9'000'000'000'000'000'000LL);
                result[i][j] = static_cast<long long>(value);
            }
        }
    }
    return result;
}

long long brute_determinant(const Matrix& matrix) {
    const std::size_t n = matrix.size();
    if(n == 0) return 1;
    long long result = 0;
    for(std::size_t column = 0; column < n; ++column){
        Matrix minor(n - 1, std::vector<long long>(n - 1));
        for(std::size_t row = 1; row < n; ++row){
            std::size_t output = 0;
            for(std::size_t next = 0; next < n; ++next){
                if(next == column) continue;
                minor[row - 1][output++] = matrix[row][next];
            }
        }
        const long long term =
            matrix[0][column] * brute_determinant(minor);
        result += column % 2 == 0 ? term : -term;
    }
    return result;
}

void verify_smith(const Matrix& input) {
    const auto result = linear_algebra::smith_normal_form(input);
    assert(multiply(multiply(result.left, input), result.right)
           == result.diagonal);
    const std::size_t diagonal_size =
        std::min(input.size(), input.empty() ? 0U : input.front().size());
    long long previous = 1;
    std::size_t rank = 0;
    for(std::size_t i = 0; i < result.diagonal.size(); ++i){
        for(std::size_t j = 0; j < result.diagonal[i].size(); ++j){
            if(i != j) assert(result.diagonal[i][j] == 0);
        }
    }
    for(std::size_t i = 0; i < diagonal_size; ++i){
        const long long value = result.diagonal[i][i];
        if(value == 0) continue;
        assert(value > 0);
        assert(value % previous == 0);
        previous = value;
        ++rank;
    }
    assert(rank == result.rank);
}

void verify_hermite(const Matrix& input) {
    const auto result = linear_algebra::row_hermite_normal_form(input);
    assert(multiply(result.left, input) == result.matrix);
    assert(result.pivot_columns.size() <= result.matrix.size());
    for(std::size_t pivot_row = 0;
        pivot_row < result.pivot_columns.size();
        ++pivot_row){
        const std::size_t column = result.pivot_columns[pivot_row];
        const long long pivot = result.matrix[pivot_row][column];
        assert(pivot > 0);
        if(pivot_row != 0){
            assert(result.pivot_columns[pivot_row - 1] < column);
        }
        for(std::size_t row = pivot_row + 1;
            row < result.matrix.size();
            ++row){
            assert(result.matrix[row][column] == 0);
        }
        for(std::size_t row = 0; row < pivot_row; ++row){
            assert(0 <= result.matrix[row][column]);
            assert(result.matrix[row][column] < pivot);
        }
    }
}

void verify_lll() {
    const Matrix input{{1, 1, 1}, {-1, 0, 2}, {3, 5, 6}};
    const auto result = linear_algebra::lll_lattice_reduction(input);
    assert(multiply(result.transformation, input) == result.basis);
    for(const auto& vector : result.basis){
        long long squared_norm = 0;
        for(const long long value : vector) squared_norm += value * value;
        assert(squared_norm > 0);
    }
}

void verify_extension_field() {
    number_theory::FiniteFieldExtension<2> field({1, 1, 0, 1});
    for(std::uint64_t bits = 1; bits < 8; ++bits){
        std::vector<std::uint64_t> value(3);
        for(std::size_t index = 0; index < 3; ++index){
            value[index] = (bits >> index) & 1U;
        }
        assert(field.multiply(value, field.inverse(value)) == field.one());
        assert(field.power(value, 7) == field.one());
    }
}

}  // namespace

int main() {
    std::uint64_t seed;
    int rounds;
    if(!(std::cin >> seed >> rounds)) return 0;
    std::mt19937_64 random(seed);
    for(int round = 0; round < rounds; ++round){
        const std::size_t rows = 1 + random() % 4;
        const std::size_t columns = 1 + random() % 4;
        Matrix matrix(rows, std::vector<long long>(columns));
        for(auto& row : matrix){
            for(long long& value : row){
                value = static_cast<long long>(random() % 11) - 5;
            }
        }
        verify_smith(matrix);
        verify_hermite(matrix);
        if(rows == columns){
            assert(linear_algebra::bareiss_determinant(matrix)
                   == brute_determinant(matrix));
        }
    }
    verify_lll();
    verify_extension_field();
    for(const std::uint64_t value :
        {77ULL, 221ULL, 10403ULL, 1009ULL * 1013ULL}){
        const auto result = math::quadratic_sieve_factor(
            value, 200, 200'000, 24
        );
        assert(result.factor.has_value());
        assert(value % *result.factor == 0);
        assert(*result.factor != 1);
    }
    std::cout << "OK\n";
}
