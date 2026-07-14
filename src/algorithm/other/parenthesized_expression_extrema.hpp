#pragma once

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

struct ParenthesizedExpressionExtrema {
    unsigned long long minimum;
    unsigned long long maximum;
};

inline ParenthesizedExpressionExtrema parenthesized_expression_extrema(
    const std::string& expression
){
    std::vector<unsigned long long> numbers;
    std::vector<char> operators;
    std::size_t position = 0;
    while(position < expression.size()){
        if(!std::isdigit(static_cast<unsigned char>(expression[position])))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: expected number "
                "(parenthesized_expression_extrema)."
            );
        }
        unsigned long long value = 0;
        while(position < expression.size() &&
              std::isdigit(static_cast<unsigned char>(expression[position]))){
            const int digit = expression[position] - '0';
            if(value > (std::numeric_limits<unsigned long long>::max() - digit) / 10)[[unlikely]]{
                throw std::overflow_error(
                    "library assertion fault: number overflow "
                    "(parenthesized_expression_extrema)."
                );
            }
            value = value * 10 + static_cast<unsigned long long>(digit);
            position++;
        }
        numbers.push_back(value);
        if(position == expression.size()) break;
        const char operation = expression[position++];
        if(operation != '+' && operation != '*')[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid operator "
                "(parenthesized_expression_extrema)."
            );
        }
        operators.push_back(operation);
    }
    if(numbers.empty() || operators.size() + 1 != numbers.size())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: malformed expression "
            "(parenthesized_expression_extrema)."
        );
    }
    const int n = static_cast<int>(numbers.size());
    std::vector<std::vector<unsigned long long>> minimum(
        static_cast<std::size_t>(n), std::vector<unsigned long long>(static_cast<std::size_t>(n))
    );
    auto maximum = minimum;
    for(int index = 0; index < n; index++){
        minimum[static_cast<std::size_t>(index)][static_cast<std::size_t>(index)] =
            maximum[static_cast<std::size_t>(index)][static_cast<std::size_t>(index)] =
                numbers[static_cast<std::size_t>(index)];
    }
    auto combine = [](unsigned long long left, unsigned long long right, char operation){
        const __int128 result = operation == '+'
            ? static_cast<__int128>(left) + right
            : static_cast<__int128>(left) * right;
        if(result > std::numeric_limits<unsigned long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "library assertion fault: result overflow "
                "(parenthesized_expression_extrema)."
            );
        }
        return static_cast<unsigned long long>(result);
    };
    for(int span = 2; span <= n; span++){
        for(int left = 0; left + span <= n; left++){
            const int right = left + span - 1;
            unsigned long long low = std::numeric_limits<unsigned long long>::max();
            unsigned long long high = 0;
            for(int split = left; split < right; split++){
                const char operation = operators[static_cast<std::size_t>(split)];
                low = std::min(low, combine(
                    minimum[static_cast<std::size_t>(left)][static_cast<std::size_t>(split)],
                    minimum[static_cast<std::size_t>(split + 1)][static_cast<std::size_t>(right)],
                    operation
                ));
                high = std::max(high, combine(
                    maximum[static_cast<std::size_t>(left)][static_cast<std::size_t>(split)],
                    maximum[static_cast<std::size_t>(split + 1)][static_cast<std::size_t>(right)],
                    operation
                ));
            }
            minimum[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)] = low;
            maximum[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)] = high;
        }
    }
    return {minimum.front().back(), maximum.front().back()};
}
