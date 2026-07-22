// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/combinatorics/stern_brocot_tree.hpp"

using SBT = math::SternBrocotTree<long long>;
using Fraction = SBT::Fraction;
using Direction = SBT::Direction;

char direction_char(Direction direction){
    return direction == Direction::Left ? 'L' : 'R';
}

Direction parse_direction(char direction){
    if(direction == 'L') return Direction::Left;
    if(direction == 'R') return Direction::Right;
    throw std::invalid_argument("invalid direction");
}

std::string expand(const SBT::Path& path){
    std::string result;
    for(const auto& run: path){
        result.append(static_cast<std::size_t>(run.length), direction_char(run.direction));
    }
    return result;
}

struct NaiveNode{
    Fraction lower;
    Fraction value;
    Fraction upper;
    std::string path;
};

Fraction add(Fraction lhs, Fraction rhs){
    return Fraction{lhs.numerator + rhs.numerator, lhs.denominator + rhs.denominator};
}

std::vector<NaiveNode> enumerate_nodes(int max_depth){
    std::vector<NaiveNode> result;
    std::vector<NaiveNode> level{{Fraction{0, 1}, Fraction{1, 1}, Fraction{1, 0}, ""}};
    for(int depth = 0; depth <= max_depth; ++depth){
        result.insert(result.end(), level.begin(), level.end());
        std::vector<NaiveNode> next;
        for(const NaiveNode& node: level){
            const Fraction left_value = add(node.lower, node.value);
            const Fraction right_value = add(node.value, node.upper);
            next.push_back(NaiveNode{node.lower, left_value, node.value, node.path + 'L'});
            next.push_back(NaiveNode{node.value, right_value, node.upper, node.path + 'R'});
        }
        level = std::move(next);
    }
    return result;
}

void test_against_naive_tree(){
    const auto nodes = enumerate_nodes(7);
    for(const NaiveNode& node: nodes){
        const auto path = SBT::encode_path(node.value);
        assert(expand(path) == node.path);
        assert(SBT::from_path(path) == node.value);
        assert(SBT::decode_path(path) == node.value);
        assert(SBT::depth(node.value) == node.path.size());
        assert(SBT::range(node.value) == (SBT::Range{node.lower, node.upper}));

        for(std::size_t depth = 0; depth <= node.path.size(); ++depth){
            const auto ancestor = SBT::ancestor(node.value, depth);
            assert(ancestor.has_value());
            assert(expand(SBT::encode_path(*ancestor)) == node.path.substr(0, depth));
            assert(SBT::is_ancestor(*ancestor, node.value));
        }
        assert(!SBT::ancestor(node.value, node.path.size() + 1).has_value());
    }

    const auto shallow = enumerate_nodes(4);
    for(const NaiveNode& lhs: shallow){
        for(const NaiveNode& rhs: shallow){
            std::size_t common = 0;
            while(common < lhs.path.size() && common < rhs.path.size() &&
                  lhs.path[common] == rhs.path[common]){
                ++common;
            }
            assert(expand(SBT::encode_path(SBT::lca(lhs.value, rhs.value))) ==
                   lhs.path.substr(0, common));
        }
    }
}

void test_open_intervals(){
    assert(SBT::simplest_in_open_interval(
        Fraction{1, 1}, Fraction{3, 2}
    ) == (Fraction{4, 3}));

    const auto endpoints = enumerate_nodes(6);
    const auto candidates = enumerate_nodes(14);
    for(const NaiveNode& lhs: endpoints){
        for(const NaiveNode& rhs: endpoints){
            if(SBT::compare(lhs.value, rhs.value) >= 0) continue;
            Fraction expected{};
            bool found = false;
            for(const NaiveNode& candidate: candidates){
                if(SBT::compare(lhs.value, candidate.value) < 0 &&
                   SBT::compare(candidate.value, rhs.value) < 0){
                    expected = candidate.value;
                    found = true;
                    break;
                }
            }
            assert(found);
            assert(SBT::simplest_in_open_interval(lhs.value, rhs.value) == expected);
            assert(SBT::simplest_in_closed_interval(rhs.value, lhs.value) ==
                   SBT::lca(lhs.value, rhs.value));
        }
    }
}

void test_search(){
    for(long long numerator = 1; numerator <= 40; ++numerator){
        for(long long denominator = 1; denominator <= 40; ++denominator){
            if(std::gcd(numerator, denominator) != 1) continue;
            const Fraction target{numerator, denominator};
            const auto predicate = [=](SBT::WideFraction value){
                return value.numerator * denominator <
                    static_cast<__int128_t>(numerator) * value.denominator;
            };
            assert(SBT::search(40, predicate) == target);
        }
    }
}

void test_boundaries_and_exceptions(){
    SBT::Path empty_path;
    assert(empty_path.capacity() > 0);
    assert(SBT::is_node(Fraction{1, 1}));
    assert(SBT::is_node(Fraction{2, 3}));
    assert(!SBT::is_node(Fraction{2, 2}));
    assert(!SBT::is_node(Fraction{-1, 2}));

    constexpr long long maximum = std::numeric_limits<long long>::max();
    const Fraction large{maximum, 1};
    const auto large_path = SBT::encode_path(large);
    assert(large_path.size() == 1);
    assert(large_path[0] == (SBT::Run{Direction::Right, static_cast<std::uint64_t>(maximum - 1)}));
    assert(SBT::from_path(large_path) == large);
    assert(SBT::range(large) == (SBT::Range{Fraction{maximum - 1, 1}, Fraction{1, 0}}));

    SBT::Path merged;
    merged.push_back(Direction::Left, 2);
    merged.push_back(Direction::Left, 3);
    assert(merged.size() == 1 && merged[0].length == 5);
    merged.clear();
    assert(merged.empty());

    bool thrown = false;
    try{
        (void)SBT::encode_path(Fraction{2, 2});
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        SBT::Path path;
        path.push_back(Direction::Right, static_cast<std::uint64_t>(maximum));
        (void)SBT::from_path(path);
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        using TinySBT = math::SternBrocotTree<long long, 1>;
        (void)TinySBT::encode_path(TinySBT::Fraction{2, 3});
    }catch(const std::length_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        SBT::Path path;
        path.push_back(Direction::Left, 0);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        SBT::Path path;
        path.push_back(Direction::Left, std::numeric_limits<std::uint64_t>::max());
        path.push_back(Direction::Left, 1);
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        SBT::Path path;
        (void)path[0];
    }catch(const std::out_of_range&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)SBT::simplest_in_open_interval(Fraction{2, 3}, Fraction{2, 3});
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)SBT::search(0, [](SBT::WideFraction){ return false; });
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    {
        using RootOnlySBT = math::SternBrocotTree<long long, 0>;
        RootOnlySBT::Path path;
        assert(RootOnlySBT::from_path(path) == (RootOnlySBT::Fraction{1, 1}));
        assert(RootOnlySBT::encode_path(RootOnlySBT::Fraction{1, 1}).empty());
    }

    assert(SBT::reduce(Fraction{12, 18}) == (Fraction{2, 3}));
    assert(SBT::compare(Fraction{0, 1}, Fraction{1, 0}) < 0);
    assert(SBT::descend(Fraction{1, 1}, Direction::Left, 3) == (Fraction{1, 4}));
    assert(SBT::descend(Fraction{1, 1}, Direction::Right, 3) == (Fraction{4, 1}));
}

void answer_queries(){
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
            const Fraction result = SBT::from_path(path);
            std::cout << result.numerator << ' ' << result.denominator << '\n';
        }else if(type == "LCA"){
            long long a, b, c, d;
            std::cin >> a >> b >> c >> d;
            const Fraction result = SBT::lca(Fraction{a, b}, Fraction{c, d});
            std::cout << result.numerator << ' ' << result.denominator << '\n';
        }else if(type == "ANCESTOR"){
            std::uint64_t depth;
            long long numerator, denominator;
            std::cin >> depth >> numerator >> denominator;
            const auto result = SBT::ancestor(Fraction{numerator, denominator}, depth);
            if(result){
                std::cout << result->numerator << ' ' << result->denominator << '\n';
            }else{
                std::cout << -1 << '\n';
            }
        }else if(type == "RANGE"){
            long long numerator, denominator;
            std::cin >> numerator >> denominator;
            const auto result = SBT::range(Fraction{numerator, denominator});
            std::cout << result.lower.numerator << ' ' << result.lower.denominator << ' '
                      << result.upper.numerator << ' ' << result.upper.denominator << '\n';
        }else if(type == "DEPTH"){
            long long numerator, denominator;
            std::cin >> numerator >> denominator;
            std::cout << SBT::depth(Fraction{numerator, denominator}) << '\n';
        }else if(type == "DESCEND"){
            long long numerator, denominator;
            char direction;
            std::uint64_t steps;
            std::cin >> numerator >> denominator >> direction >> steps;
            const auto result = SBT::descend(
                Fraction{numerator, denominator}, parse_direction(direction), steps
            );
            std::cout << result.numerator << ' ' << result.denominator << '\n';
        }else if(type == "OPEN"){
            long long a, b, c, d;
            std::cin >> a >> b >> c >> d;
            const auto result = SBT::simplest_in_open_interval(Fraction{a, b}, Fraction{c, d});
            std::cout << result.numerator << ' ' << result.denominator << '\n';
        }else if(type == "SEARCH"){
            long long numerator, denominator, maximum;
            std::cin >> numerator >> denominator >> maximum;
            const auto result = SBT::search(maximum, [=](SBT::WideFraction value){
                return value.numerator * denominator <
                    static_cast<__int128_t>(numerator) * value.denominator;
            });
            std::cout << result.numerator << ' ' << result.denominator << '\n';
        }
    }
}

int main(){
    if(std::cin.peek() != std::char_traits<char>::eof()){
        answer_queries();
        return 0;
    }
    test_against_naive_tree();
    test_open_intervals();
    test_search();
    test_boundaries_and_exceptions();
}
