// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "../../src/algorithm/range/interval_set.hpp"

struct ConcatenationMonoid{
    using S = std::string;

    S op(S left, const S& right) const{
        left += right;
        return left;
    }

    S e() const{
        return {};
    }
};

inline constexpr ConcatenationMonoid concatenation_monoid{};
using TestSet = WeightedIntervalSet<concatenation_monoid, int>;

void validate(const TestSet& set){
    bool first = true;
    int previous_right = 0;
    std::string previous_value;
    for(const auto& [left, interval] : set.intervals()){
        assert(left < interval.right);
        if(!first){
            assert(previous_right <= left);
            if(previous_right == left){
                assert(previous_value != interval.value);
            }
        }
        first = false;
        previous_right = interval.right;
        previous_value = interval.value;
    }
}

std::string naive_prod(
    const std::vector<std::optional<char>>& values,
    int left,
    int right
){
    std::string result;
    bool in_interval = false;
    char previous = 0;
    for(int x = left; x < right; ++x){
        if(!values[static_cast<std::size_t>(x)]){
            in_interval = false;
            continue;
        }
        const char value = *values[static_cast<std::size_t>(x)];
        if(!in_interval || value != previous){
            result.push_back(value);
        }
        in_interval = true;
        previous = value;
    }
    return result;
}

void compare_naive(
    const TestSet& set,
    const std::vector<std::optional<char>>& values
){
    validate(set);
    std::size_t expected_size = 0;
    bool in_interval = false;
    char previous = 0;
    int expected_left = 0;
    auto current = set.intervals().begin();

    for(int x = 0; x <= static_cast<int>(values.size()); ++x){
        const bool present =
            x < static_cast<int>(values.size())
            && values[static_cast<std::size_t>(x)].has_value();
        const char value = present
            ? *values[static_cast<std::size_t>(x)]
            : 0;
        if(present && (!in_interval || value != previous)){
            if(in_interval){
                assert(current != set.intervals().end());
                assert(current->first == expected_left);
                assert(current->second.right == x);
                assert(current->second.value == std::string(1, previous));
                ++current;
                ++expected_size;
            }
            expected_left = x;
        }else if(!present && in_interval){
            assert(current != set.intervals().end());
            assert(current->first == expected_left);
            assert(current->second.right == x);
            assert(current->second.value == std::string(1, previous));
            ++current;
            ++expected_size;
        }
        in_interval = present;
        if(present) previous = value;
    }
    assert(current == set.intervals().end());
    assert(set.size() == static_cast<int>(expected_size));

    for(int x = 0; x < static_cast<int>(values.size()); ++x){
        assert(set.contains(x) == values[static_cast<std::size_t>(x)].has_value());
        const auto value = set.get(x);
        assert(value.has_value() == values[static_cast<std::size_t>(x)].has_value());
        if(value){
            assert(*value == std::string(
                1,
                *values[static_cast<std::size_t>(x)]
            ));
        }
    }

    assert(set.all_prod() == naive_prod(
        values,
        0,
        static_cast<int>(values.size())
    ));
}

void self_test(){
    TestSet set;
    set.set(0, 10, "A");
    set.set(3, 7, "B");
    assert(set.size() == 3);
    assert(set.prod(1, 9) == "ABA");

    const auto middle = set.interval(5);
    assert(middle);
    assert(middle->left == 3);
    assert(middle->right == 7);
    assert(middle->value == "B");

    set.set(3, 7, "A");
    assert(set.size() == 1);
    assert(set.interval(5)->left == 0);
    assert(set.interval(5)->right == 10);

    set.set(2, 8, "B");
    set.erase(4, 6);
    assert(set.prod(0, 10) == "ABBA");
    assert(!set.covered(0, 10));
    set.assign(4, 6, "B");
    assert(set.prod(0, 10) == "ABA");
    assert(set.covered(0, 10));
    assert(set.mex(0) == 10);

    set.clear();
    set.set(0, 2, "A");
    set.set(2, 4, "B");
    set.set(4, 6, "C");
    assert(set.prod(1, 5) == "ABC");

    std::mt19937 rng(20261017);
    for(int round = 0; round < 100; ++round){
        TestSet random_set;
        std::vector<std::optional<char>> values(32);
        for(int step = 0; step < 300; ++step){
            int left = static_cast<int>(rng() % 33);
            int right = static_cast<int>(rng() % 33);
            if(left > right) std::swap(left, right);
            if(rng() % 3 != 0){
                const char value = static_cast<char>('A' + rng() % 4);
                random_set.set(left, right, std::string(1, value));
                for(int x = left; x < right; ++x){
                    values[static_cast<std::size_t>(x)] = value;
                }
            }else{
                random_set.erase(left, right);
                for(int x = left; x < right; ++x){
                    values[static_cast<std::size_t>(x)] = std::nullopt;
                }
            }
            compare_naive(random_set, values);

            int query_left = static_cast<int>(rng() % 33);
            int query_right = static_cast<int>(rng() % 33);
            if(query_left > query_right){
                std::swap(query_left, query_right);
            }
            assert(
                random_set.prod(query_left, query_right)
                == naive_prod(values, query_left, query_right)
            );
        }
    }
}

void print_entries(const std::vector<TestSet::Entry>& entries){
    std::cout << entries.size();
    for(const auto& entry : entries){
        std::cout
            << ' ' << entry.left
            << ' ' << entry.right
            << ' ' << entry.value;
    }
    std::cout << '\n';
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }

    TestSet set;
    while(query_count--){
        std::string type;
        std::cin >> type;

        if(type == "SET" || type == "ASSIGN"){
            int left, right;
            std::string value;
            std::cin >> left >> right >> value;
            if(type == "SET"){
                set.set(left, right, value);
            }else{
                set.assign(left, right, value);
            }
            validate(set);
        }else if(type == "ERASE"){
            int left, right;
            std::cin >> left >> right;
            set.erase(left, right);
            validate(set);
        }else if(type == "GET"){
            int x;
            std::cin >> x;
            const auto value = set.get(x);
            std::cout << (value ? *value : "-") << '\n';
        }else if(type == "INTERVAL"){
            int x;
            std::cin >> x;
            const auto interval = set.interval(x);
            if(interval){
                std::cout
                    << interval->left << ' '
                    << interval->right << ' '
                    << interval->value << '\n';
            }else{
                std::cout << "-\n";
            }
        }else if(type == "ENUM"){
            int left, right;
            std::cin >> left >> right;
            print_entries(set.enumerate(left, right));
        }else if(type == "ENUMALL"){
            print_entries(set.enumerate());
        }else if(type == "PROD"){
            int left, right;
            std::cin >> left >> right;
            std::cout << '[' << set.prod(left, right) << "]\n";
        }else if(type == "ALLPROD"){
            std::cout << '[' << set.all_prod() << "]\n";
        }else if(type == "CONTAINS"){
            int x;
            std::cin >> x;
            std::cout << set.contains(x) << '\n';
        }else if(type == "COVERED"){
            int left, right;
            std::cin >> left >> right;
            std::cout << set.covered(left, right) << '\n';
        }else if(type == "MEX"){
            int x;
            std::cin >> x;
            std::cout << set.mex(x) << '\n';
        }else if(type == "SIZE"){
            std::cout << set.size() << '\n';
        }else if(type == "EMPTY"){
            std::cout << set.empty() << '\n';
        }else if(type == "CLEAR"){
            set.clear();
            validate(set);
        }
    }
}
