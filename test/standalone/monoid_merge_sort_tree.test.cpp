// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/structure/other/monoid_merge_sort_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"
#include "../../src/structure/types/monoid.hpp"

constexpr AddMonoid<long long> add_monoid{};

constexpr int MOD = 1000000007;
constexpr int multiply_mod(int left, int right){
    return static_cast<int>(
        static_cast<long long>(left) * static_cast<long long>(right) % MOD
    );
}
constexpr int one_mod(){ return 1; }
constexpr Monoid<multiply_mod, one_mod> product_monoid{};

struct NoDefaultKey{
    int value;

    NoDefaultKey() = delete;
    explicit constexpr NoDefaultKey(int value_): value(value_){}

    friend constexpr bool operator<(
        const NoDefaultKey& left,
        const NoDefaultKey& right
    ){
        return left.value < right.value;
    }
};

struct NoDefaultWeight{
    int value;

    NoDefaultWeight() = delete;
    explicit constexpr NoDefaultWeight(int value_): value(value_){}
};

struct NoDefaultAddMonoid{
    using S = NoDefaultWeight;

    constexpr S op(const S& left, const S& right) const{
        return S(left.value + right.value);
    }

    constexpr S e() const{ return S(0); }
};

constexpr NoDefaultAddMonoid no_default_add_monoid{};

template<class Function>
void expect_runtime_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_boundaries_and_duplicates(){
    const std::vector<int> keys{2, -1, 2, 3, 2, 0};
    const std::vector<long long> weights{5, 7, -2, 11, 13, 17};
    MonoidMergeSortTree<int, add_monoid, 8> tree(keys, weights);

    assert(tree.size() == 6);
    assert(!tree.empty());
    assert(tree.key(1) == -1);
    assert(tree.weight(4) == 13);
    assert(tree.fold(0, 6) == 51);
    assert(tree.fold(1, 5) == 29);
    assert(tree.fold_less(0, 6, -1) == 0);
    assert(tree.fold_less_equal(0, 6, -1) == 7);
    assert(tree.fold_less(0, 6, 2) == 24);
    assert(tree.fold_less_equal(0, 6, 2) == 40);
    assert(tree.fold_less_equal(2, 5, 2) == 11);
    assert(tree.fold_less(3, 3, 100) == 0);
    assert(tree.fold(6, 6) == 0);
}

void test_product(){
    const std::vector<int> values{2, 3, 5, 2, 7};
    MonoidMergeSortTree<int, product_monoid, 5> tree(values, values);

    assert(tree.fold_less_equal(0, 5, 2) == 4);
    assert(tree.fold_less_equal(1, 4, 4) == 6);
    assert(tree.fold_less(0, 5, 2) == 1);
    assert(tree.fold_less_equal(2, 2, 100) == 1);
    assert(tree.fold_less_equal(0, 5, 100) == 420);
}

void test_non_default_constructible_types(){
    const std::vector<NoDefaultKey> keys{
        NoDefaultKey(3), NoDefaultKey(1), NoDefaultKey(2), NoDefaultKey(1)
    };
    const std::vector<NoDefaultWeight> weights{
        NoDefaultWeight(5), NoDefaultWeight(7),
        NoDefaultWeight(11), NoDefaultWeight(13)
    };
    MonoidMergeSortTree<
        NoDefaultKey, no_default_add_monoid, 4
    > tree(keys, weights);

    assert(tree.fold(0, 4).value == 36);
    assert(tree.fold_less(0, 4, NoDefaultKey(2)).value == 20);
    assert(tree.fold_less_equal(1, 4, NoDefaultKey(2)).value == 31);
}

void test_array_and_empty(){
    const std::array<int, 4> keys{4, 1, 4, 2};
    const std::array<long long, 4> weights{10, 20, 30, 40};
    MonoidMergeSortTree<int, add_monoid, 4> tree(keys, weights);
    assert(tree.fold_less_equal(0, 4, 2) == 60);

    MonoidMergeSortTree<int, add_monoid, 0> empty(
        std::vector<int>{}, std::vector<long long>{}
    );
    assert(empty.size() == 0);
    assert(empty.empty());
    assert(empty.fold(0, 0) == 0);
    assert(empty.fold_less(0, 0, 0) == 0);
    assert(empty.fold_less_equal(0, 0, 0) == 0);
}

void test_exceptions(){
    expect_runtime_error([]{
        MonoidMergeSortTree<int, add_monoid, 3> tree(
            std::vector<int>{1}, std::vector<long long>{}
        );
        (void)tree;
    });
    expect_runtime_error([]{
        MonoidMergeSortTree<int, add_monoid, 3> tree(
            std::vector<int>{1, 2, 3, 4},
            std::vector<long long>{1, 2, 3, 4}
        );
        (void)tree;
    });

    MonoidMergeSortTree<int, add_monoid, 4> tree(
        std::vector<int>{1, 2, 3}, std::vector<long long>{4, 5, 6}
    );
    expect_runtime_error([&]{ (void)tree.key(-1); });
    expect_runtime_error([&]{ (void)tree.key(3); });
    expect_runtime_error([&]{ (void)tree.weight(-1); });
    expect_runtime_error([&]{ (void)tree.weight(3); });
    expect_runtime_error([&]{ (void)tree.fold(-1, 2); });
    expect_runtime_error([&]{ (void)tree.fold(2, 1); });
    expect_runtime_error([&]{ (void)tree.fold(0, 4); });
    expect_runtime_error([&]{ (void)tree.fold_less(-1, 0, 1); });
    expect_runtime_error([&]{ (void)tree.fold_less_equal(0, 4, 1); });
}

void test_random(){
    std::mt19937_64 random(20260714);
    for(int trial = 0; trial < 300; trial++){
        const int size = static_cast<int>(random() % 65U);
        std::vector<int> keys(static_cast<std::size_t>(size));
        std::vector<long long> weights(static_cast<std::size_t>(size));
        for(int index = 0; index < size; index++){
            keys[static_cast<std::size_t>(index)] =
                static_cast<int>(random() % 31U) - 15;
            weights[static_cast<std::size_t>(index)] =
                static_cast<long long>(random() % 101U) - 50;
        }
        MonoidMergeSortTree<int, add_monoid, 64> tree(keys, weights);

        for(int query = 0; query < 200; query++){
            int left = static_cast<int>(
                random() % static_cast<unsigned int>(size + 1)
            );
            int right = static_cast<int>(
                random() % static_cast<unsigned int>(size + 1)
            );
            if(right < left) std::swap(left, right);
            const int upper = static_cast<int>(random() % 35U) - 17;

            long long all = 0;
            long long less = 0;
            long long less_equal = 0;
            for(int index = left; index < right; index++){
                const long long weight = weights[static_cast<std::size_t>(index)];
                const int key = keys[static_cast<std::size_t>(index)];
                all += weight;
                if(key < upper) less += weight;
                if(key <= upper) less_equal += weight;
            }
            assert(tree.fold(left, right) == all);
            assert(tree.fold_less(left, right, upper) == less);
            assert(tree.fold_less_equal(left, right, upper) == less_equal);
        }
    }
}

void self_test(){
    test_boundaries_and_duplicates();
    test_product();
    test_non_default_constructible_types();
    test_array_and_empty();
    test_exceptions();
    test_random();
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int query_count;
    if(!(std::cin >> size >> query_count)){
        self_test();
        return 0;
    }

    std::vector<int> keys(static_cast<std::size_t>(size));
    std::vector<long long> weights(static_cast<std::size_t>(size));
    for(int& key : keys) std::cin >> key;
    for(long long& weight : weights) std::cin >> weight;

    MonoidMergeSortTree<int, add_monoid, 300000> tree(keys, weights);
    while(query_count-- != 0){
        std::string type;
        int left;
        int right;
        int upper;
        std::cin >> type >> left >> right >> upper;
        if(type == "ALL"){
            std::cout << tree.fold(left, right) << '\n';
        }else if(type == "LESS"){
            std::cout << tree.fold_less(left, right, upper) << '\n';
        }else{
            std::cout << tree.fold_less_equal(left, right, upper) << '\n';
        }
    }
}
