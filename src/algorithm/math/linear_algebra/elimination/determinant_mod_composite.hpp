#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_DETERMINANT_MOD_COMPOSITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_DETERMINANT_MOD_COMPOSITE_HPP_INCLUDED

#include <climits>
#include <stdexcept>
#include <utility>
#include <vector>

inline long long determinant_mod_composite(
    std::vector<std::vector<long long>> matrix, long long modulus
){
    if(modulus < 1 || modulus > INT_MAX)
        throw std::invalid_argument("determinant_mod_composite: modulus outside [1, INT_MAX]");
    const int n = static_cast<int>(matrix.size());
    for(auto& row: matrix){
        if(row.size() != matrix.size())
            throw std::invalid_argument("determinant_mod_composite: matrix must be square");
        for(auto& value: row){ value %= modulus; if(value < 0) value += modulus; }
    }
    long long answer = 1 % modulus;
    for(int column = 0; column < n; ++column){
        for(int row = column+1; row < n; ++row){
            while(matrix[row][column] != 0){
                const long long quotient = matrix[column][column]/matrix[row][column];
                for(int next = column; next < n; ++next){
                    long long& value = matrix[column][next];
                    value = (value-quotient*matrix[row][next]) % modulus;
                    if(value < 0) value += modulus;
                }
                std::swap(matrix[column],matrix[row]);
                answer = answer == 0 ? 0 : modulus-answer;
            }
        }
        if(matrix[column][column] == 0) return 0;
        answer = answer*matrix[column][column] % modulus;
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_DETERMINANT_MOD_COMPOSITE_HPP_INCLUDED
