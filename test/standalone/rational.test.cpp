// competitive-verifier: STANDALONE

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

#include "../../src/structure/types/rational/rational.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if (!(std::cin >> query_count)) return 1;
    while (query_count-- > 0) {
        std::string operation;
        std::string lhs_text;
        std::cin >> operation >> lhs_text;
        const cp::Rational lhs{std::string_view(lhs_text)};

        if (operation == "normalize") {
            std::cout << lhs << '\n';
            continue;
        }
        if (operation == "floor") {
            std::cout << lhs.floor() << '\n';
            continue;
        }
        if (operation == "ceil") {
            std::cout << lhs.ceil() << '\n';
            continue;
        }
        if (operation == "trunc") {
            std::cout << lhs.trunc() << '\n';
            continue;
        }
        if (operation == "round") {
            std::cout << lhs.round() << '\n';
            continue;
        }
        if (operation == "round_even") {
            std::cout << lhs.round_ties_to_even() << '\n';
            continue;
        }
        if (operation == "pow") {
            std::int64_t exponent;
            std::cin >> exponent;
            std::cout << lhs.pow(exponent) << '\n';
            continue;
        }

        std::string rhs_text;
        std::cin >> rhs_text;
        const cp::Rational rhs{std::string_view(rhs_text)};
        if (operation == "add") {
            std::cout << lhs + rhs << '\n';
        } else if (operation == "sub") {
            std::cout << lhs - rhs << '\n';
        } else if (operation == "mul") {
            std::cout << lhs * rhs << '\n';
        } else if (operation == "div") {
            std::cout << lhs / rhs << '\n';
        } else if (operation == "cmp") {
            std::cout << (lhs < rhs ? -1 : rhs < lhs ? 1 : 0) << '\n';
        } else if (operation == "mediant") {
            std::cout << cp::Rational::mediant(lhs, rhs) << '\n';
        } else {
            return 2;
        }
    }
    return 0;
}
