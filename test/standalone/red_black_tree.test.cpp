// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include "../../src/structure/bbst/red_black_tree.hpp"

std::optional<int> kth_naive(const std::set<int>& s, int k){
    if(k < 0 || static_cast<int>(s.size()) <= k) return std::nullopt;
    auto it = s.begin();
    std::advance(it, k);
    return *it;
}

std::optional<int> lower_bound_naive(const std::set<int>& s, int x){
    auto it = s.lower_bound(x);
    if(it == s.end()) return std::nullopt;
    return *it;
}

std::optional<int> upper_bound_naive(const std::set<int>& s, int x){
    auto it = s.upper_bound(x);
    if(it == s.end()) return std::nullopt;
    return *it;
}

std::optional<int> max_leq_naive(const std::set<int>& s, int x){
    auto it = s.upper_bound(x);
    if(it == s.begin()) return std::nullopt;
    --it;
    return *it;
}

std::optional<int> max_less_naive(const std::set<int>& s, int x){
    auto it = s.lower_bound(x);
    if(it == s.begin()) return std::nullopt;
    --it;
    return *it;
}

void check_all(const RedBlackSet<int, 256>& tree, const std::set<int>& s){
    assert(tree.size() == static_cast<int>(s.size()));
    assert(tree.empty() == s.empty());
    for(int x = -20; x <= 220; x++){
        assert(tree.contains(x) == (s.find(x) != s.end()));
        assert(tree.order_of_key(x) == static_cast<int>(std::distance(s.begin(), s.lower_bound(x))));
        assert(tree.order_of_key_upper(x) == static_cast<int>(std::distance(s.begin(), s.upper_bound(x))));
        assert(tree.lower_bound(x) == lower_bound_naive(s, x));
        assert(tree.upper_bound(x) == upper_bound_naive(s, x));
        assert(tree.max_leq(x) == max_leq_naive(s, x));
        assert(tree.max_less(x) == max_less_naive(s, x));
    }
    for(int k = -2; k <= static_cast<int>(s.size()) + 2; k++){
        assert(tree.kth(k) == kth_naive(s, k));
    }
}

int main(){
    int input_q;
    if(std::cin >> input_q){
        RedBlackSet<int, 512> tree;
        auto print_optional = [](std::optional<int> value){
            if(value.has_value()){
                std::cout << *value << '\n';
            }else{
                std::cout << "NONE\n";
            }
        };
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "INSERT"){
                int value;
                std::cin >> value;
                std::cout << tree.insert(value) << '\n';
            }else if(type == "ERASE"){
                int value;
                std::cin >> value;
                std::cout << tree.erase(value) << '\n';
            }else if(type == "CONTAINS"){
                int value;
                std::cin >> value;
                std::cout << tree.contains(value) << '\n';
            }else if(type == "KTH"){
                int k;
                std::cin >> k;
                print_optional(tree.kth(k));
            }else if(type == "ORDER"){
                int value;
                std::cin >> value;
                std::cout << tree.order_of_key(value) << '\n';
            }else if(type == "ORDERU"){
                int value;
                std::cin >> value;
                std::cout << tree.order_of_key_upper(value) << '\n';
            }else if(type == "LOWER"){
                int value;
                std::cin >> value;
                print_optional(tree.lower_bound(value));
            }else if(type == "UPPER"){
                int value;
                std::cin >> value;
                print_optional(tree.upper_bound(value));
            }else if(type == "LEQ"){
                int value;
                std::cin >> value;
                print_optional(tree.max_leq(value));
            }else if(type == "LESS"){
                int value;
                std::cin >> value;
                print_optional(tree.max_less(value));
            }else if(type == "SIZE"){
                std::cout << tree.size() << '\n';
            }else if(type == "CLEAR"){
                tree.clear();
                std::cout << tree.size() << '\n';
            }
        }
        return 0;
    }
    {
        RedBlackSet<int, 3> tree;
        assert(tree.insert(1));
        assert(tree.insert(2));
        assert(tree.insert(3));
        assert(tree.erase(2));
        assert(tree.insert(4));
        bool thrown = false;
        try{
            tree.insert(5);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    RedBlackSet<int, 256> tree;
    std::set<int> naive;
    std::mt19937 rng(260713);
    for(int step = 0; step < 10000; step++){
        int x = static_cast<int>(rng() % 201);
        int type = static_cast<int>(rng() % 8);
        if(type < 3){
            bool expected = naive.insert(x).second;
            assert(tree.insert(x) == expected);
        }else if(type < 5){
            bool expected = naive.erase(x) != 0;
            assert(tree.erase(x) == expected);
        }else if(type == 5){
            auto k = static_cast<int>(rng() % 240) - 20;
            assert(tree.kth(k) == kth_naive(naive, k));
        }else if(type == 6){
            check_all(tree, naive);
        }else{
            if(!naive.empty()){
                int k = static_cast<int>(rng() % naive.size());
                auto val = kth_naive(naive, k);
                assert(val.has_value());
                assert(tree.erase(*val));
                naive.erase(*val);
            }
        }
    }
    check_all(tree, naive);

    tree.clear();
    naive.clear();
    check_all(tree, naive);
    assert(tree.insert(42));
    naive.insert(42);
    check_all(tree, naive);
}
