// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/stern_brocot_tree

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/combinatorics/stern_brocot_tree.hpp"

using SBT = math::SternBrocotTree<long long, 64>;
using Fraction = SBT::Fraction;

char direction_char(SBT::Direction direction){
    return direction == SBT::Direction::Left ? 'L' : 'R';
}

SBT::Direction parse_direction(char direction){
    if(direction == 'L') return SBT::Direction::Left;
    if(direction == 'R') return SBT::Direction::Right;
    throw std::invalid_argument("invalid direction");
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "ENCODE_PATH"){
            long long numerator, denominator;
            std::cin >> numerator >> denominator;
            const auto path = SBT::encode_path(Fraction{numerator, denominator});
            std::cout << path.size();
            for(const auto& run: path){
                std::cout << ' ' << direction_char(run.direction) << ' ' << run.length;
            }
            std::cout << '\n';
        }else if(type == "DECODE_PATH"){
            int run_count;
            std::cin >> run_count;
            SBT::Path path;
            for(int i = 0; i < run_count; ++i){
                char direction;
                std::uint64_t length;
                std::cin >> direction >> length;
                path.push_back(parse_direction(direction), length);
            }
            const Fraction answer = SBT::from_path(path);
            std::cout << answer.numerator << ' ' << answer.denominator << '\n';
        }else if(type == "LCA"){
            long long a, b, c, d;
            std::cin >> a >> b >> c >> d;
            const Fraction answer = SBT::lca(Fraction{a, b}, Fraction{c, d});
            std::cout << answer.numerator << ' ' << answer.denominator << '\n';
        }else if(type == "ANCESTOR"){
            std::uint64_t depth;
            long long numerator, denominator;
            std::cin >> depth >> numerator >> denominator;
            const auto answer = SBT::ancestor(Fraction{numerator, denominator}, depth);
            if(answer){
                std::cout << answer->numerator << ' ' << answer->denominator << '\n';
            }else{
                std::cout << -1 << '\n';
            }
        }else if(type == "RANGE"){
            long long numerator, denominator;
            std::cin >> numerator >> denominator;
            const auto answer = SBT::range(Fraction{numerator, denominator});
            std::cout << answer.lower.numerator << ' ' << answer.lower.denominator << ' '
                      << answer.upper.numerator << ' ' << answer.upper.denominator << '\n';
        }
    }
}
