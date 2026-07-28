// competitive-verifier: STANDALONE

#include <cctype>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/integer_geometry/result/point_set/point_set_squared_metrics.hpp"

namespace{

integer_geometry::Integer parse_integer(const std::string& text){
    if(text.empty()) throw std::invalid_argument("empty integer");
    std::size_t index = 0;
    bool negative = false;
    if(text[index] == '-' || text[index] == '+'){
        negative = text[index] == '-';
        ++index;
    }
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

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count = 0;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::size_t size = 0;
        std::cin >> size;
        std::vector<integer_geometry::Point> polygon;
        polygon.reserve(size);
        for(std::size_t index = 0; index < size; ++index){
            std::string x;
            std::string y;
            std::cin >> x >> y;
            polygon.emplace_back(
                integer_geometry::Rational(parse_integer(x)),
                integer_geometry::Rational(parse_integer(y))
            );
        }
        const auto answer = integer_geometry::convex_diameter_squared(polygon);
        std::cout << (answer ? answer->to_string() : "none") << '\n';
    }
}
