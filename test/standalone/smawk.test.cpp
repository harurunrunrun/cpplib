// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/other/utility/smawk.hpp"

std::vector<int> naive_minima(const std::vector<std::vector<long long>>& matrix){
    std::vector<int> result(matrix.size());
    for(std::size_t row = 0; row < matrix.size(); ++row){
        int best = 0;
        for(int column = 1; column < static_cast<int>(matrix[row].size()); ++column){
            if(matrix[row][static_cast<std::size_t>(column)]
               < matrix[row][static_cast<std::size_t>(best)]){
                best = column;
            }
        }
        result[row] = best;
    }
    return result;
}

void self_test(){
    assert(smawk_row_minima(0, 0, [](int, int){ return 0; }).empty());

    bool thrown = false;
    try{
        (void)smawk_row_minima(1, 0, [](int, int){ return 0; });
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    assert((smawk_row_minima(4, 7, [](int, int){ return 0; })
        == std::vector<int>{0, 0, 0, 0}));

    std::mt19937_64 random(2026071306);
    for(int iteration = 0; iteration < 2000; ++iteration){
        const int rows = 1 + static_cast<int>(random() % 25);
        const int columns = 1 + static_cast<int>(random() % 25);
        std::vector<long long> row_bias(static_cast<std::size_t>(rows));
        std::vector<long long> column_bias(static_cast<std::size_t>(columns));
        for(long long& value: row_bias){
            value = static_cast<long long>(random() % 2001) - 1000;
        }
        for(long long& value: column_bias){
            value = static_cast<long long>(random() % 2001) - 1000;
        }
        std::vector<std::vector<long long>> matrix(
            static_cast<std::size_t>(rows),
            std::vector<long long>(static_cast<std::size_t>(columns))
        );
        for(int row = 0; row < rows; ++row){
            for(int column = 0; column < columns; ++column){
                const long long difference = row - column;
                matrix[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] =
                    difference * difference
                    + row_bias[static_cast<std::size_t>(row)]
                    + column_bias[static_cast<std::size_t>(column)];
            }
        }
        const auto actual = smawk_row_minima(
            rows,
            columns,
            [&](int row, int column){
                return matrix[static_cast<std::size_t>(row)]
                             [static_cast<std::size_t>(column)];
            }
        );
        assert(actual == naive_minima(matrix));
    }

    constexpr int rows = 200000;
    constexpr int columns = 150000;
    std::size_t evaluations = 0;
    const auto large = smawk_row_minima(
        rows,
        columns,
        [&](int row, int column){
            ++evaluations;
            const long long difference = static_cast<long long>(row) - column;
            return difference * difference;
        }
    );
    assert(large.size() == static_cast<std::size_t>(rows));
    for(int row: {0, 1, columns - 1, columns, rows - 1}){
        assert(large[static_cast<std::size_t>(row)] == std::min(row, columns - 1));
    }
    assert(evaluations <= 20ULL * (rows + columns));
}

int main(){
    int rows, columns;
    if(!(std::cin >> rows >> columns)){
        self_test();
        return 0;
    }
    std::vector<std::vector<long long>> matrix(
        static_cast<std::size_t>(rows),
        std::vector<long long>(static_cast<std::size_t>(columns))
    );
    for(auto& row: matrix){
        for(long long& value: row) std::cin >> value;
    }
    const auto answer = smawk_row_minima(
        rows,
        columns,
        [&](int row, int column){
            return matrix[static_cast<std::size_t>(row)]
                         [static_cast<std::size_t>(column)];
        }
    );
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
