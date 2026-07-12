// competitive-verifier: STANDALONE

#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/bbst/persistent_splay_tree.hpp"

using Tree = PersistentSplaySet<int, 200000, 5000>;

template<class Set>
std::optional<int> kth_naive(const Set& set, int k){
    if(k < 0 || static_cast<int>(set.size()) <= k) return std::nullopt;
    auto it = set.begin();
    std::advance(it, k);
    return *it;
}

template<class Iterator>
std::optional<int> iterator_value(Iterator it, Iterator last){
    if(it == last) return std::nullopt;
    return *it;
}

template<class TreeType, class Set>
void check_version(const TreeType& tree, int version, const Set& set){
    assert(tree.size(version) == static_cast<int>(set.size()));
    assert(tree.empty(version) == set.empty());
    assert(tree.to_vector(version) == std::vector<int>(set.begin(), set.end()));

    for(int x = -45; x <= 85; x++){
        assert(tree.contains(x, version) == set.contains(x));
        assert(
            tree.order_of_key(x, version) ==
            static_cast<int>(std::distance(set.begin(), set.lower_bound(x)))
        );
        assert(
            tree.order_of_key_upper(x, version) ==
            static_cast<int>(std::distance(set.begin(), set.upper_bound(x)))
        );
        assert(
            tree.lower_bound(x, version) ==
            iterator_value(set.lower_bound(x), set.end())
        );
        assert(
            tree.upper_bound(x, version) ==
            iterator_value(set.upper_bound(x), set.end())
        );

        std::optional<int> expected_max_leq;
        auto leq = set.upper_bound(x);
        if(leq != set.begin()){
            --leq;
            expected_max_leq = *leq;
        }
        assert(tree.max_leq(x, version) == expected_max_leq);

        std::optional<int> expected_max_less;
        auto less = set.lower_bound(x);
        if(less != set.begin()){
            --less;
            expected_max_less = *less;
        }
        assert(tree.max_less(x, version) == expected_max_less);
    }

    for(int k = -2; k <= static_cast<int>(set.size()) + 2; k++){
        assert(tree.kth(k, version) == kth_naive(set, k));
    }
}

void branching_test(){
    using BranchTree = PersistentSplaySet<int, 5000, 64>;
    auto tree = std::make_unique<BranchTree>();
    std::vector<std::set<int>> expected(1);

    auto insert = [&](int base, int key){
        int version = tree->insert(key, base);
        auto next = expected[static_cast<std::size_t>(base)];
        next.insert(key);
        assert(version == static_cast<int>(expected.size()));
        expected.push_back(std::move(next));
        return version;
    };
    auto erase = [&](int base, int key){
        int version = tree->erase(key, base);
        auto next = expected[static_cast<std::size_t>(base)];
        next.erase(key);
        assert(version == static_cast<int>(expected.size()));
        expected.push_back(std::move(next));
        return version;
    };

    int v1 = insert(0, 40);
    int v2 = insert(v1, 10);
    int v3 = insert(v1, 70);
    int v4 = insert(v2, 30);
    int v5 = erase(v3, 40);
    int v6 = insert(v3, 55);
    int v7 = erase(v4, 10);
    int v8 = insert(v2, 10);
    int v9 = erase(v6, 999);

    assert(v1 == 1 && v2 == 2 && v3 == 3 && v4 == 4);
    assert(v5 == 5 && v6 == 6 && v7 == 7 && v8 == 8 && v9 == 9);
    for(int version = 0; version < static_cast<int>(expected.size()); version++){
        check_version(*tree, version, expected[static_cast<std::size_t>(version)]);
    }
    assert(tree->versions() == static_cast<int>(expected.size()));
}

template<class Compare>
void randomized_test(unsigned seed){
    using RandomTree = PersistentSplaySet<int, 120000, 1200, Compare>;
    auto tree = std::make_unique<RandomTree>(Compare{});
    std::vector<std::set<int, Compare>> expected(1);
    std::mt19937 rng(seed);

    for(int step = 0; step < 650; step++){
        int base = static_cast<int>(rng() % expected.size());
        int key = static_cast<int>(rng() % 121) - 40;
        auto next = expected[static_cast<std::size_t>(base)];
        int version;
        if((rng() & 1U) != 0U){
            version = tree->insert(key, base);
            next.insert(key);
        }else{
            version = tree->erase(key, base);
            next.erase(key);
        }
        assert(version == static_cast<int>(expected.size()));
        expected.push_back(std::move(next));

        check_version(*tree, base, expected[static_cast<std::size_t>(base)]);
        check_version(*tree, version, expected[static_cast<std::size_t>(version)]);
        if(step % 23 == 0){
            for(int repeat = 0; repeat < 4; repeat++){
                int old = static_cast<int>(rng() % expected.size());
                check_version(*tree, old, expected[static_cast<std::size_t>(old)]);
            }
        }
    }
    assert(tree->versions() == static_cast<int>(expected.size()));
}

void node_capacity_rollback_test(){
    using TinyTree = PersistentSplaySet<int, 4, 8>;
    auto tree = std::make_unique<TinyTree>();
    int v1 = tree->insert(10, 0);
    int v2 = tree->insert(5, v1);
    const std::vector<int> before0 = tree->to_vector(0);
    const std::vector<int> before1 = tree->to_vector(v1);
    const std::vector<int> before2 = tree->to_vector(v2);
    int version_count = tree->versions();

    auto expect_capacity_error = [&](auto operation){
        bool threw = false;
        try{
            operation();
        }catch(const std::runtime_error&){
            threw = true;
        }
        assert(threw);
        assert(tree->versions() == version_count);
        assert(tree->to_vector(0) == before0);
        assert(tree->to_vector(v1) == before1);
        assert(tree->to_vector(v2) == before2);
    };

    for(int repeat = 0; repeat < 2; repeat++){
        expect_capacity_error([&]{ (void)tree->insert(15, v2); });
        expect_capacity_error([&]{ (void)tree->erase(10, v2); });
    }

    int v3 = tree->insert(5, v2);
    assert(tree->to_vector(v3) == before2);
    assert(tree->versions() == version_count + 1);
}

void version_capacity_rollback_test(){
    using TinyTree = PersistentSplaySet<int, 32, 3>;
    auto tree = std::make_unique<TinyTree>();
    int v1 = tree->insert(1, 0);
    int v2 = tree->insert(2, v1);
    const auto before0 = tree->to_vector(0);
    const auto before1 = tree->to_vector(v1);
    const auto before2 = tree->to_vector(v2);

    bool threw = false;
    try{
        (void)tree->erase(1, v2);
    }catch(const std::runtime_error&){
        threw = true;
    }
    assert(threw);
    assert(tree->versions() == 3);
    assert(tree->to_vector(0) == before0);
    assert(tree->to_vector(v1) == before1);
    assert(tree->to_vector(v2) == before2);
}

void self_test(){
    branching_test();
    randomized_test<std::less<int>>(20260921);
    randomized_test<std::greater<int>>(20260922);
    node_capacity_rollback_test();
    version_capacity_rollback_test();
}

void print_optional(const std::optional<int>& value){
    std::cout << (value ? *value : -1) << '\n';
}

int main(){
    self_test();

    int q;
    if(!(std::cin >> q)) return 0;
    auto tree = std::make_unique<Tree>();
    while(q--){
        std::string type;
        int version, x;
        std::cin >> type >> version >> x;
        if(type == "INSERT"){
            std::cout << tree->insert(x, version) << '\n';
        }else if(type == "ERASE"){
            std::cout << tree->erase(x, version) << '\n';
        }else if(type == "CONTAINS"){
            std::cout << tree->contains(x, version) << '\n';
        }else if(type == "KTH"){
            print_optional(tree->kth(x, version));
        }else if(type == "ORDER"){
            std::cout << tree->order_of_key(x, version) << '\n';
        }else if(type == "ORDER_UPPER"){
            std::cout << tree->order_of_key_upper(x, version) << '\n';
        }else if(type == "LOWER"){
            print_optional(tree->lower_bound(x, version));
        }else if(type == "UPPER"){
            print_optional(tree->upper_bound(x, version));
        }else if(type == "MAX_LEQ"){
            print_optional(tree->max_leq(x, version));
        }else if(type == "MAX_LESS"){
            print_optional(tree->max_less(x, version));
        }else if(type == "SIZE"){
            std::cout << tree->size(version) << '\n';
        }else if(type == "DUMP"){
            auto values = tree->to_vector(version);
            for(std::size_t i = 0; i < values.size(); i++){
                if(i != 0) std::cout << ' ';
                std::cout << values[i];
            }
            std::cout << '\n';
        }else{
            assert(false);
        }
    }
}
