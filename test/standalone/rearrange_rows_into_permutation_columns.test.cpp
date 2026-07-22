// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/specialized/rearrange_rows_into_permutation_columns.hpp"

namespace{

void validate(
    const std::vector<std::vector<int>>& input,
    const std::vector<std::vector<int>>& output
){
    assert(input.size() == output.size());
    if(input.empty()) return;
    const std::size_t column_count = input.front().size();
    for(std::size_t row = 0; row < input.size(); ++row){
        auto expected = input[row];
        auto actual = output[row];
        std::sort(expected.begin(), expected.end());
        std::sort(actual.begin(), actual.end());
        assert(expected == actual);
    }
    for(std::size_t column = 0; column < column_count; ++column){
        std::vector<int> values;
        for(const auto& row: output) values.push_back(row[column]);
        std::sort(values.begin(), values.end());
        for(std::size_t value = 0; value < values.size(); ++value){
            assert(values[value] == static_cast<int>(value));
        }
    }
}

void self_test(){
    const std::vector<std::vector<int>> matrix = {
        {0, 0}, {1, 2}, {1, 2}
    };
    validate(matrix, rearrange_rows_into_permutation_columns(matrix));
    assert(rearrange_rows_into_permutation_columns({}).empty());
    const std::vector<std::vector<int>> no_columns(3);
    validate(
        no_columns,
        rearrange_rows_into_permutation_columns(no_columns)
    );
}

} // namespace

int main(){
    self_test();

    int n, m;
    if(!(std::cin >> n >> m)) return 0;
    std::vector<std::vector<int>> matrix(
        static_cast<std::size_t>(n),
        std::vector<int>(static_cast<std::size_t>(m))
    );
    for(auto& row: matrix){
        for(int& value: row){
            std::cin >> value;
            --value;
        }
    }
    const auto answer = rearrange_rows_into_permutation_columns(matrix);
    std::cout << "Yes\n";
    for(const auto& row: answer){
        for(int column = 0; column < m; ++column){
            if(column != 0) std::cout << ' ';
            std::cout << row[static_cast<std::size_t>(column)] + 1;
        }
        std::cout << '\n';
    }
}
