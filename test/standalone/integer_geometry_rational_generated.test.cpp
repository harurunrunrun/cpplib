// competitive-verifier: STANDALONE

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/integer_geometry/2d/intersection.hpp"

namespace{

integer_geometry::Integer parse_integer(const std::string& text){
    if(text.empty()) throw std::invalid_argument("empty integer");
    std::size_t index = 0;
    const bool negative = text[index] == '-';
    if(negative || text[index] == '+') ++index;
    if(index == text.size()) throw std::invalid_argument("missing digits");
    integer_geometry::Integer result = 0;
    for(; index < text.size(); ++index){
        const unsigned char character =
            static_cast<unsigned char>(text[index]);
        if(!std::isdigit(character)){
            throw std::invalid_argument("invalid integer digit");
        }
        result *= 10;
        result += static_cast<int>(character - '0');
    }
    return negative ? -result : result;
}

integer_geometry::Rational read_rational(){
    std::string numerator;
    std::string denominator;
    std::cin >> numerator >> denominator;
    return {
        parse_integer(numerator),
        parse_integer(denominator),
    };
}

integer_geometry::Point read_point(){
    return {read_rational(), read_rational()};
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count = 0;
    std::cin >> case_count;
    while(case_count-- > 0){
        char operation = 0;
        std::cin >> operation;
        if(operation == 'O'){
            const integer_geometry::Point first = read_point();
            const integer_geometry::Point second = read_point();
            const integer_geometry::Point third = read_point();
            std::cout << integer_geometry::orientation(first, second, third)
                      << '\n';
            continue;
        }
        if(operation == 'I'){
            const integer_geometry::Line first{read_point(), read_point()};
            const integer_geometry::Line second{read_point(), read_point()};
            const auto point = integer_geometry::line_intersection(
                first, second
            );
            if(!point){
                std::cout << "none\n";
            }else{
                std::cout << point->x << ' ' << point->y << '\n';
            }
            continue;
        }

        const integer_geometry::Rational first = read_rational();
        const integer_geometry::Rational second = read_rational();
        if(operation == 'A') std::cout << first + second << '\n';
        if(operation == 'S') std::cout << first - second << '\n';
        if(operation == 'M') std::cout << first * second << '\n';
        if(operation == 'D') std::cout << first / second << '\n';
        if(operation == 'C'){
            std::cout << ((first > second) - (first < second)) << '\n';
        }
    }
}
