// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../src/structure/segtree/partially_persistent_lazysegtree.hpp"
#include "../../src/structure/segtree/persistent_lazysegtree.hpp"

constexpr std::int64_t mod = 998244353;

struct SumLen{
    std::int64_t sum;
    int len;
};

struct Affine{
    std::int64_t a;
    std::int64_t b;
};

SumLen sum_op(SumLen x, SumLen y){
    return {(x.sum + y.sum) % mod, x.len + y.len};
}

SumLen sum_e(){
    return {0, 0};
}

SumLen affine_mapping(Affine f, SumLen x){
    return {
        (f.a * x.sum + f.b * x.len) % mod,
        x.len
    };
}

Affine affine_composition(Affine f, Affine g){
    return {
        f.a * g.a % mod,
        (f.a * g.b + f.b) % mod
    };
}

Affine affine_id(){
    return {1, 0};
}

constexpr Monoid_Act<
    sum_op,
    sum_e,
    affine_mapping,
    affine_composition,
    affine_id
> affine_sum{};

SumLen add_mapping(std::int64_t f, SumLen x){
    return {x.sum + f * x.len, x.len};
}

std::int64_t add_composition(std::int64_t f, std::int64_t g){
    return f + g;
}

std::int64_t add_id(){
    return 0;
}

constexpr Monoid_Act<
    sum_op,
    sum_e,
    add_mapping,
    add_composition,
    add_id
> add_sum{};

std::int64_t naive_prod(
    const std::vector<std::int64_t>& a,
    int l,
    int r
){
    std::int64_t res = 0;
    for(int k = l; k < r; k++){
        res += a[static_cast<std::size_t>(k)];
        res %= mod;
    }
    return res;
}

void test_persistent_affine(){
    constexpr int n = 31;
    std::mt19937 rng(1234567);

    std::vector<SumLen> init(n);
    std::vector<std::int64_t> initial(n);
    for(int k = 0; k < n; k++){
        initial[static_cast<std::size_t>(k)] = rng() % 100;
        init[static_cast<std::size_t>(k)] = {
            initial[static_cast<std::size_t>(k)],
            1
        };
    }

    PersistentLazySegtree<affine_sum, 32, 1800> seg(init);
    std::vector<std::vector<std::int64_t>> versions = {initial};

    for(int step = 0; step < 1500; step++){
        int base = static_cast<int>(rng() % versions.size());
        std::vector<std::int64_t> next = versions[base];
        int type = static_cast<int>(rng() % 4);
        int version;

        if(type == 0){
            int k = static_cast<int>(rng() % n);
            std::int64_t x = rng() % 1000;
            next[static_cast<std::size_t>(k)] = x;
            version = seg.set(base, k, {x, 1});
        }else if(type == 1){
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r){
                std::swap(l, r);
            }
            Affine f{
                static_cast<std::int64_t>(rng() % 4),
                static_cast<std::int64_t>(rng() % 20)
            };
            for(int k = l; k < r; k++){
                auto& x = next[static_cast<std::size_t>(k)];
                x = (f.a * x + f.b) % mod;
            }
            version = seg.apply(base, l, r, f);
        }else if(type == 2){
            int k = static_cast<int>(rng() % n);
            Affine f{
                static_cast<std::int64_t>(rng() % 4),
                static_cast<std::int64_t>(rng() % 20)
            };
            auto& x = next[static_cast<std::size_t>(k)];
            x = (f.a * x + f.b) % mod;
            version = seg.apply(base, k, f);
        }else{
            version = seg.fork(base);
        }

        assert(version == static_cast<int>(versions.size()));
        versions.push_back(next);

        for(int query = 0; query < 5; query++){
            int v = static_cast<int>(rng() % versions.size());
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r){
                std::swap(l, r);
            }
            assert(seg.prod(v, l, r).sum == naive_prod(versions[v], l, r));

            int k = static_cast<int>(rng() % n);
            assert(
                seg.get(v, k).sum ==
                versions[v][static_cast<std::size_t>(k)]
            );
            assert(seg.all_prod(v).sum == naive_prod(versions[v], 0, n));
        }
    }

    assert(seg.size() == n);
    assert(seg.versions() == static_cast<int>(versions.size()));
    assert(seg.latest_version() == static_cast<int>(versions.size()) - 1);
}

void test_partially_persistent(){
    constexpr int n = 23;
    std::mt19937 rng(987654);
    std::vector<SumLen> init(n, {0, 1});
    PartiallyPersistentLazySegtree<affine_sum, 32, 1000> seg(init);
    std::vector<std::vector<std::int64_t>> versions(1, {std::vector<std::int64_t>(n)});

    for(int step = 0; step < 900; step++){
        std::vector<std::int64_t> next = versions.back();
        if(rng() & 1){
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r){
                std::swap(l, r);
            }
            Affine f{
                static_cast<std::int64_t>(rng() % 3),
                static_cast<std::int64_t>(rng() % 10)
            };
            for(int k = l; k < r; k++){
                auto& x = next[static_cast<std::size_t>(k)];
                x = (f.a * x + f.b) % mod;
            }
            seg.apply(l, r, f);
        }else{
            int k = static_cast<int>(rng() % n);
            std::int64_t x = rng() % 1000;
            next[static_cast<std::size_t>(k)] = x;
            seg.set(k, {x, 1});
        }
        versions.push_back(next);

        int old = static_cast<int>(rng() % versions.size());
        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r){
            std::swap(l, r);
        }
        assert(
            seg.prod(old, l, r).sum ==
            naive_prod(versions[old], l, r)
        );
        assert(
            seg.prod(l, r).sum ==
            naive_prod(versions.back(), l, r)
        );
    }
    assert(seg.size() == n);
    assert(seg.versions() == static_cast<int>(versions.size()));
    assert(seg.latest_version() == seg.versions() - 1);
}

void test_binary_search(){
    constexpr int n = 29;
    std::mt19937 rng(314159);
    std::vector<SumLen> init(n);
    std::vector<std::int64_t> a(n);
    for(int k = 0; k < n; k++){
        a[static_cast<std::size_t>(k)] = rng() % 10;
        init[static_cast<std::size_t>(k)] = {
            a[static_cast<std::size_t>(k)],
            1
        };
    }

    PersistentLazySegtree<add_sum, 32, 500> seg(init);
    int version = 0;
    for(int step = 0; step < 400; step++){
        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r){
            std::swap(l, r);
        }
        std::int64_t add = rng() % 5;
        version = seg.apply(version, l, r, add);
        for(int k = l; k < r; k++){
            a[static_cast<std::size_t>(k)] += add;
        }

        int left = static_cast<int>(rng() % (n + 1));
        std::int64_t limit = rng() % 500;
        int expected_right = left;
        std::int64_t sum = 0;
        while(
            expected_right < n &&
            sum + a[static_cast<std::size_t>(expected_right)] <= limit
        ){
            sum += a[static_cast<std::size_t>(expected_right++)];
        }
        assert(seg.max_right(
            version,
            left,
            [](SumLen x, std::int64_t bound){
                return x.sum <= bound;
            },
            limit
        ) == expected_right);

        int right = static_cast<int>(rng() % (n + 1));
        int expected_left = right;
        sum = 0;
        while(
            expected_left > 0 &&
            sum + a[static_cast<std::size_t>(expected_left - 1)] <= limit
        ){
            sum += a[static_cast<std::size_t>(--expected_left)];
        }
        assert(seg.min_left(
            version,
            right,
            [](SumLen x, std::int64_t bound){
                return x.sum <= bound;
            },
            limit
        ) == expected_left);
    }
}

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::vector<SumLen> input(static_cast<std::size_t>(input_n));
        for(auto& value: input){
            std::cin >> value.sum;
            value.len = 1;
        }
        PersistentLazySegtree<add_sum, 128, 700> seg(input);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int version, k;
                std::int64_t value;
                std::cin >> version >> k >> value;
                std::cout << seg.set(version, k, {value, 1}) << '\n';
            }else if(type == "ADD"){
                int version, l, r;
                std::int64_t value;
                std::cin >> version >> l >> r >> value;
                std::cout << seg.apply(version, l, r, value) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << seg.fork(version) << '\n';
            }else if(type == "GET"){
                int version, k;
                std::cin >> version >> k;
                std::cout << seg.get(version, k).sum << '\n';
            }else if(type == "SUM"){
                int version, l, r;
                std::cin >> version >> l >> r;
                std::cout << seg.prod(version, l, r).sum << '\n';
            }else if(type == "ALL"){
                int version;
                std::cin >> version;
                std::cout << seg.all_prod(version).sum << '\n';
            }else if(type == "MR"){
                int version, l;
                std::int64_t limit;
                std::cin >> version >> l >> limit;
                std::cout << seg.max_right(
                    version,
                    l,
                    [](SumLen value, std::int64_t bound){
                        return value.sum <= bound;
                    },
                    limit
                ) << '\n';
            }else if(type == "ML"){
                int version, r;
                std::int64_t limit;
                std::cin >> version >> r >> limit;
                std::cout << seg.min_left(
                    version,
                    r,
                    [](SumLen value, std::int64_t bound){
                        return value.sum <= bound;
                    },
                    limit
                ) << '\n';
            }
        }
        return 0;
    }
    test_persistent_affine();
    test_partially_persistent();
    test_binary_search();
}
