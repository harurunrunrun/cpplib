// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../src/structure/tree/integer_set.hpp"

#if defined(INT_SET_GNU_X86_TARGET) || defined(INT_SET_TARGET_AVX512F) || defined(INT_SET_TARGET_AVX2)
#error integer_set internal macro leaked
#endif

template<class L>
void print_optional(const std::optional<L>& value){
    if(value){
        std::cout << *value << '\n';
    }else{
        std::cout << "NONE\n";
    }
}

template<class L>
void print_list(const std::vector<L>& values){
    std::cout << values.size();
    for(L value : values) std::cout << ' ' << value;
    std::cout << '\n';
}

template<class L, L MAX_SIZE>
void run(int q){
    using Set = Int_Set<L, MAX_SIZE>;
    using Signed = std::make_signed_t<L>;
    Set values;

    while(q--){
        std::string command;
        std::cin >> command;

        if(command == "INSERT" || command == "ERASE"){
            long long x;
            std::cin >> x;
            if(command == "INSERT") values.insert(static_cast<Signed>(x));
            else values.erase(static_cast<Signed>(x));
        }else if(command == "CONTAIN"){
            long long x;
            std::cin >> x;
            std::cout << values.contain(static_cast<Signed>(x)) << '\n';
        }else if(command == "RANGE"){
            long long l, r;
            std::cin >> l >> r;
            std::cout << values.range_sum(static_cast<Signed>(l), static_cast<Signed>(r)) << '\n';
        }else if(command == "LEAST" || command == "MORE" || command == "MOST" || command == "LESS"){
            long long x;
            std::cin >> x;
            Signed sx = static_cast<Signed>(x);
            if(command == "LEAST") print_optional(values.least(sx));
            else if(command == "MORE") print_optional(values.more(sx));
            else if(command == "MOST") print_optional(values.most(sx));
            else print_optional(values.less(sx));
        }else if(command == "KGE" || command == "KGT" || command == "KLE" || command == "KLT"){
            long long x, k;
            std::cin >> x >> k;
            Signed sx = static_cast<Signed>(x);
            Signed sk = static_cast<Signed>(k);
            if(command == "KGE") print_optional(values.kth_ge(sx, sk));
            else if(command == "KGT") print_optional(values.kth_gt(sx, sk));
            else if(command == "KLE") print_optional(values.kth_le(sx, sk));
            else print_optional(values.kth_lt(sx, sk));
        }else if(command == "UCONTAIN"){
            unsigned long long x;
            std::cin >> x;
            std::cout << values.contain(static_cast<L>(x)) << '\n';
        }else if(command == "URANGE"){
            unsigned long long l, r;
            std::cin >> l >> r;
            std::cout << values.range_sum(static_cast<L>(l), static_cast<L>(r)) << '\n';
        }else if(command == "ULEAST" || command == "UMORE" || command == "UMOST" || command == "ULESS"){
            unsigned long long x;
            std::cin >> x;
            L ux = static_cast<L>(x);
            if(command == "ULEAST") print_optional(values.least(ux));
            else if(command == "UMORE") print_optional(values.more(ux));
            else if(command == "UMOST") print_optional(values.most(ux));
            else print_optional(values.less(ux));
        }else if(command == "UKGE" || command == "UKGT" || command == "UKLE" || command == "UKLT"){
            unsigned long long x, k;
            std::cin >> x >> k;
            L ux = static_cast<L>(x);
            L uk = static_cast<L>(k);
            if(command == "UKGE") print_optional(values.kth_ge(ux, uk));
            else if(command == "UKGT") print_optional(values.kth_gt(ux, uk));
            else if(command == "UKLE") print_optional(values.kth_le(ux, uk));
            else print_optional(values.kth_lt(ux, uk));
        }else if(command == "EMPTY"){
            std::cout << values.empty() << '\n';
        }else if(command == "MIN"){
            print_optional(values.min());
        }else if(command == "MAX"){
            print_optional(values.max());
        }else if(command == "LIST"){
            print_list(values.list());
        }else if(command == "CLEAR"){
            values.clear();
        }else if(command == "TRY_INSERT" || command == "TRY_ERASE"){
            long long x;
            std::cin >> x;
            try{
                if(command == "TRY_INSERT") values.insert(static_cast<Signed>(x));
                else values.erase(static_cast<Signed>(x));
                std::cout << "OK\n";
            }catch(const std::runtime_error&){
                std::cout << "THROW\n";
            }
        }else if(command == "UTRY_INSERT" || command == "UTRY_ERASE"){
            unsigned long long x;
            std::cin >> x;
            try{
                if(command == "UTRY_INSERT") values.insert(static_cast<L>(x));
                else values.erase(static_cast<L>(x));
                std::cout << "OK\n";
            }catch(const std::runtime_error&){
                std::cout << "THROW\n";
            }
        }else if(command == "COPY"){
            long long x;
            std::cin >> x;
            Set copied(values);
            Set assigned;
            assigned = values;
            bool ok = copied.list() == values.list() && assigned.list() == values.list();
            Signed sx = static_cast<Signed>(x);
            bool was_present = values.contain(sx);
            if(was_present) copied.erase(sx);
            else copied.insert(sx);
            ok = ok && values.contain(sx) == was_present && copied.contain(sx) != was_present;
            assigned.clear();
            ok = ok && assigned.empty() && values.contain(sx) == was_present;
            std::cout << ok << '\n';
        }else if(command == "MOVE"){
            Set source(values);
            const auto expected = source.list();
            Set moved(std::move(source));
            bool ok = moved.list() == expected && source.empty();
            source.insert(static_cast<L>(0));
            ok = ok && source.contain(static_cast<L>(0));
            Set assigned;
            assigned = std::move(moved);
            ok = ok && assigned.list() == expected && moved.empty();
            moved.insert(static_cast<L>(MAX_SIZE - 1));
            ok = ok && moved.contain(static_cast<L>(MAX_SIZE - 1));
            std::cout << ok << '\n';
        }
    }
}

template<class L, L MAX_SIZE>
void self_test(unsigned long long seed){
    using Set = Int_Set<L, MAX_SIZE>;
    Set values;
    std::set<L> oracle;
    std::mt19937_64 rng(seed);

    auto as_vector = [&]{ return std::vector<L>(oracle.begin(), oracle.end()); };
    auto least = [&](L x) -> std::optional<L>{
        auto it = oracle.lower_bound(x);
        return it == oracle.end() ? std::nullopt : std::optional<L>(*it);
    };
    auto more = [&](L x) -> std::optional<L>{
        auto it = oracle.upper_bound(x);
        return it == oracle.end() ? std::nullopt : std::optional<L>(*it);
    };
    auto most = [&](L x) -> std::optional<L>{
        auto it = oracle.upper_bound(x);
        if(it == oracle.begin()) return std::nullopt;
        return *std::prev(it);
    };
    auto less = [&](L x) -> std::optional<L>{
        auto it = oracle.lower_bound(x);
        if(it == oracle.begin()) return std::nullopt;
        return *std::prev(it);
    };

    for(int step = 0; step < 12000; step++){
        L x = static_cast<L>(rng() % MAX_SIZE);
        int kind = static_cast<int>(rng() % 12);
        if(kind < 4){
            values.insert(x);
            oracle.insert(x);
        }else if(kind < 6){
            values.erase(x);
            oracle.erase(x);
        }else if(kind == 6 && step % 997 == 0){
            values.clear();
            oracle.clear();
        }

        L y = static_cast<L>(rng() % (static_cast<unsigned long long>(MAX_SIZE) + 20));
        L l = static_cast<L>(rng() % (static_cast<unsigned long long>(MAX_SIZE) + 20));
        L r = static_cast<L>(rng() % (static_cast<unsigned long long>(MAX_SIZE) + 20));
        assert(values.contain(y) == oracle.contains(y));
        assert(values.least(y) == (y < MAX_SIZE ? least(y) : std::nullopt));
        assert(values.more(y) == (y < MAX_SIZE - 1 ? more(y) : std::nullopt));
        assert(values.most(y) == most(std::min(y, static_cast<L>(MAX_SIZE - 1))));
        assert(values.less(y) == less(y));
        L expected_count = 0;
        if(l < r){
            expected_count = static_cast<L>(std::distance(oracle.lower_bound(l), oracle.lower_bound(std::min(r, MAX_SIZE))));
        }
        assert(values.range_sum(l, r) == expected_count);

        L k = static_cast<L>(rng() % (oracle.size() + 3));
        auto check_kth = [&](auto begin, auto end, L skip) -> std::optional<L>{
            while(begin != end && skip > 0){
                ++begin;
                --skip;
            }
            return begin == end ? std::nullopt : std::optional<L>(*begin);
        };
        assert(values.kth_ge(y, k) == (y < MAX_SIZE ? check_kth(oracle.lower_bound(y), oracle.end(), k) : std::nullopt));
        assert(values.kth_gt(y, k) == (y < MAX_SIZE - 1 ? check_kth(oracle.upper_bound(y), oracle.end(), k) : std::nullopt));
        auto reverse_begin_le = std::make_reverse_iterator(oracle.upper_bound(std::min(y, static_cast<L>(MAX_SIZE - 1))));
        auto reverse_begin_lt = std::make_reverse_iterator(oracle.lower_bound(std::min(y, MAX_SIZE)));
        assert(values.kth_le(y, k) == check_kth(reverse_begin_le, oracle.rend(), k));
        assert(values.kth_lt(y, k) == (y == 0 ? std::nullopt : check_kth(reverse_begin_lt, oracle.rend(), k)));

        if(step % 251 == 0){
            assert(values.list() == as_vector());
            Set copied(values);
            Set assigned;
            assigned = values;
            assert(copied.list() == as_vector());
            assert(assigned.list() == as_vector());
            Set moved(std::move(copied));
            assert(copied.empty());
            assert(moved.list() == as_vector());
            copied = std::move(moved);
            assert(moved.empty());
            assert(copied.list() == as_vector());
        }
    }
    assert(values.list() == as_vector());
}
int main(){
    int type, q;
    if(!(std::cin >> type >> q)){
        self_test<unsigned int, 257u>(20260713);
        self_test<unsigned long long, 1000003ULL>(20260714);
        return 0;
    }
    if(type == 0) run<unsigned int, 257u>(q);
    else run<unsigned long long, 1000003ULL>(q);
}
