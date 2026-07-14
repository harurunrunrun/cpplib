// competitive-verifier: PROBLEM https://www.spoj.com/problems/BRCKTS/

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../../src/structure/segtree/segtree.hpp"

namespace{

struct BracketAggregate{
    int balance;
    int minimum_prefix;
};

struct BracketMonoid{
    using S = BracketAggregate;

    static constexpr S op(S left, S right){
        return {
            left.balance + right.balance,
            std::min(left.minimum_prefix, left.balance + right.minimum_prefix)
        };
    }

    static constexpr S e(){
        return {0, 0};
    }
};

constexpr BracketMonoid bracket_monoid{};

BracketAggregate singleton(char bracket){
    return bracket == '(' ? BracketAggregate{1, 0} : BracketAggregate{-1, -1};
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    for(int test = 1; test <= 10; test++){
        int size;
        std::string brackets;
        std::cin >> size >> brackets;
        std::vector<BracketAggregate> values(static_cast<std::size_t>(size));
        for(int i = 0; i < size; i++){
            values[static_cast<std::size_t>(i)] = singleton(brackets[static_cast<std::size_t>(i)]);
        }
        auto tree = std::make_unique<Segtree<bracket_monoid, 30000>>(values);

        std::cout << "Test " << test << ":\n";
        int operation_count;
        std::cin >> operation_count;
        while(operation_count-- != 0){
            int position;
            std::cin >> position;
            if(position == 0){
                const BracketAggregate whole = tree->all_prod();
                std::cout << (whole.balance == 0 && whole.minimum_prefix == 0 ? "YES\n" : "NO\n");
                continue;
            }
            --position;
            char& bracket = brackets[static_cast<std::size_t>(position)];
            bracket = bracket == '(' ? ')' : '(';
            tree->set(position, singleton(bracket));
        }
    }
}
