// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/structure/bbst/aa_tree_monoid_map.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr AddMonoid<long long> sum_monoid{};

constexpr std::uint64_t modulus = 1'000'000'007ULL;

struct Matrix{
    std::uint64_t a00 = 1;
    std::uint64_t a01 = 0;
    std::uint64_t a10 = 0;
    std::uint64_t a11 = 1;

    friend bool operator==(const Matrix&, const Matrix&) = default;
};

struct MatrixMonoid{
    using S = Matrix;

    constexpr S op(const S& left, const S& right) const{
        return {
            (left.a00 * right.a00 + left.a01 * right.a10) % modulus,
            (left.a00 * right.a01 + left.a01 * right.a11) % modulus,
            (left.a10 * right.a00 + left.a11 * right.a10) % modulus,
            (left.a10 * right.a01 + left.a11 * right.a11) % modulus
        };
    }

    constexpr S e() const{ return {}; }
};

constexpr MatrixMonoid matrix_monoid{};

Matrix random_matrix(std::mt19937_64& engine){
    return {
        engine() % modulus,
        engine() % modulus,
        engine() % modulus,
        engine() % modulus
    };
}

Matrix reference_product(
    const std::map<int, Matrix>& reference,
    int lower,
    int upper
){
    Matrix result = matrix_monoid.e();
    for(auto iterator = reference.lower_bound(lower);
        iterator != reference.end() && iterator->first < upper;
        ++iterator){
        result = matrix_monoid.op(result, iterator->second);
    }
    return result;
}

void verify_random_state(
    const AATreeMonoidMap<int, matrix_monoid, 512>& tree,
    const std::map<int, Matrix>& reference,
    std::mt19937_64& engine
){
    assert(tree.size() == static_cast<int>(reference.size()));
    assert(tree.empty() == reference.empty());

    Matrix total = matrix_monoid.e();
    int index = 0;
    for(const auto& [key, value]: reference){
        assert(tree.contains(key));
        assert(tree.find(key) != nullptr);
        assert(tree.at(key) == value);
        assert(tree.kth(index) == std::make_pair(key, value));
        assert(tree.order_of_key(key) == index);
        total = matrix_monoid.op(total, value);
        ++index;
    }
    assert(tree.all_prod() == total);

    for(int repetition = 0; repetition < 30; ++repetition){
        int lower = static_cast<int>(engine() % 281) - 140;
        int upper = static_cast<int>(engine() % 281) - 140;
        if(lower > upper) std::swap(lower, upper);
        assert(tree.prod(lower, upper) ==
            reference_product(reference, lower, upper));
        assert(tree.prefix_prod(upper) ==
            reference_product(reference, -1000, upper));

        const auto expected_lower = reference.lower_bound(lower);
        const auto expected_upper = reference.upper_bound(lower);
        const auto* actual_lower = tree.lower_bound(lower);
        const auto* actual_upper = tree.upper_bound(lower);
        assert((actual_lower == nullptr) ==
            (expected_lower == reference.end()));
        assert((actual_upper == nullptr) ==
            (expected_upper == reference.end()));
        if(actual_lower != nullptr) assert(*actual_lower == *expected_lower);
        if(actual_upper != nullptr) assert(*actual_upper == *expected_upper);
        assert(tree.order_of_key(lower) ==
            std::distance(reference.begin(), expected_lower));
        assert(tree.order_of_key_inclusive(lower) ==
            std::distance(reference.begin(), expected_upper));
    }
}

void test_random_noncommutative(){
    std::mt19937_64 engine(2026071701ULL);
    for(int iteration = 0; iteration < 80; ++iteration){
        AATreeMonoidMap<int, matrix_monoid, 512> tree;
        std::map<int, Matrix> reference;
        for(int step = 0; step < 2500; ++step){
            const int key = static_cast<int>(engine() % 241) - 120;
            const Matrix value = random_matrix(engine);
            const int operation = static_cast<int>(engine() % 6);
            if(operation == 0){
                const bool expected = reference.emplace(key, value).second;
                assert(tree.insert(key, value) == expected);
            }else if(operation == 1){
                const bool expected = reference.find(key) == reference.end();
                reference[key] = value;
                assert(tree.insert_or_assign(key, value) == expected);
            }else if(operation == 2){
                const auto iterator = reference.find(key);
                const bool expected = iterator != reference.end();
                if(expected) iterator->second = value;
                assert(tree.set(key, value) == expected);
            }else if(operation == 3){
                assert(tree.erase(key) == (reference.erase(key) != 0));
            }else if(operation == 4){
                const auto iterator = reference.find(key);
                assert((tree.find(key) == nullptr) ==
                    (iterator == reference.end()));
                if(iterator != reference.end()){
                    assert(*tree.find(key) == iterator->second);
                }
            }else{
                int lower = static_cast<int>(engine() % 281) - 140;
                int upper = static_cast<int>(engine() % 281) - 140;
                if(lower > upper) std::swap(lower, upper);
                assert(tree.prod(lower, upper) ==
                    reference_product(reference, lower, upper));
            }
            if(step % 79 == 0){
                verify_random_state(tree, reference, engine);
            }
        }
        verify_random_state(tree, reference, engine);
    }
}

void test_descending_and_errors(){
    AATreeMonoidMap<
        int,
        matrix_monoid,
        512,
        std::greater<int>
    > descending;
    const Matrix first{1, 1, 0, 1};
    const Matrix second{2, 0, 1, 1};
    const Matrix third{0, 1, 1, 0};
    assert(descending.insert(9, first));
    assert(descending.insert(5, second));
    assert(descending.insert(2, third));
    const int descending_lower = 9;
    const int descending_upper = 2;
    const int descending_prefix_end = 5;
    assert(descending.prod(descending_lower, descending_upper) == matrix_monoid.op(first, second));
    assert(descending.prefix_prod(descending_prefix_end) == first);
    assert(descending.kth(0).first == 9);
    assert(descending.kth(2).first == 2);
    assert(descending.order_of_key(5) == 1);

    AATreeMonoidMap<int, sum_monoid, 3> small;
    assert((AATreeMonoidMap<int, sum_monoid, 3>::capacity() == 3));
    assert(small.insert(1, 10));
    assert(small.insert(2, 20));
    assert(small.insert(3, 30));
    assert(!small.insert(2, 99));
    assert(small.at(2) == 20);
    bool thrown = false;
    try{
        (void)small.insert(4, 40);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)small.at(8);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)small.kth(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(small.erase(2));
    assert(small.insert(4, 40));
    assert(small.all_prod() == 80);
    small.clear();
    assert(small.empty());
    assert(small.all_prod() == 0);

    AATreeMonoidMap<int, sum_monoid, 0> zero;
    thrown = false;
    try{
        (void)zero.insert(0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_large(){
    constexpr int count = 200000;
    auto tree = std::make_unique<
        AATreeMonoidMap<int, sum_monoid, count>
    >();
    std::vector<int> keys(static_cast<std::size_t>(count));
    for(int key = 0; key < count; ++key){
        keys[static_cast<std::size_t>(key)] = key;
    }
    std::mt19937 engine(20260717U);
    std::shuffle(keys.begin(), keys.end(), engine);
    for(const int key: keys) assert(tree->insert(key, 1));
    assert(tree->size() == count);
    assert(tree->all_prod() == count);
    assert(tree->prod(12345, 178901) == 178901 - 12345);
    for(int key = 0; key < count; key += 3) assert(tree->erase(key));
    const long long erased = (count + 2LL) / 3LL;
    assert(tree->all_prod() == count - erased);
    for(int index = 0; index < tree->size(); index += 997){
        const int key = tree->kth(index).first;
        assert(key % 3 != 0);
        assert(tree->order_of_key(key) == index);
    }
}

void run_self_tests(){
    test_random_noncommutative();
    test_descending_and_errors();
    test_large();
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)){
        run_self_tests();
        return 0;
    }

    auto tree = std::make_unique<
        AATreeMonoidMap<long long, sum_monoid, 500000>
    >();
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            long long key;
            long long value;
            std::cin >> key >> value;
            tree->insert_or_assign(key, value);
        }else if(type == 1){
            long long key;
            std::cin >> key;
            tree->erase(key);
        }else if(type == 2){
            long long key;
            std::cin >> key;
            const long long* value = tree->find(key);
            std::cout << (value == nullptr ? -1 : *value) << '\n';
        }else if(type == 3){
            long long key;
            std::cin >> key;
            std::cout << tree->order_of_key_inclusive(key) << '\n';
        }else if(type == 4){
            int index;
            std::cin >> index;
            if(index < 0 || index >= tree->size()){
                std::cout << -1 << '\n';
            }else{
                const auto& [key, value] = tree->kth(index);
                std::cout << key << ' ' << value << '\n';
            }
        }else if(type == 5){
            long long lower;
            long long upper;
            std::cin >> lower >> upper;
            std::cout << tree->prod(lower, upper) << '\n';
        }else{
            throw std::runtime_error("unknown ordered associative query");
        }
    }
}
