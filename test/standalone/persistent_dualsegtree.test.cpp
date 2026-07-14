// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "../../src/structure/segtree/partially_persistent_dualsegtree.hpp"
#include "../../src/structure/segtree/persistent_dualsegtree.hpp"

constexpr std::int64_t mod = 998244353;

struct Affine{
    std::int64_t a;
    std::int64_t b;
};

std::int64_t op(std::int64_t, std::int64_t){
    return 0;
}

std::int64_t e(){
    return 0;
}

std::int64_t mapping(Affine f, std::int64_t x){
    return (f.a * x + f.b) % mod;
}

Affine composition(Affine f, Affine g){
    return {
        f.a * g.a % mod,
        (f.a * g.b + f.b) % mod
    };
}

Affine id(){
    return {1, 0};
}

constexpr Monoid_Act<op, e, mapping, composition, id> affine{};

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::vector<std::int64_t> initial(static_cast<std::size_t>(input_n));
        for(auto& value: initial) std::cin >> value;
        auto seg = std::make_unique<PersistentDualSegtree<affine, 128, 2048>>(initial);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "APPLY"){
                int version, l, r;
                Affine f;
                std::cin >> version >> l >> r >> f.a >> f.b;
                std::cout << seg->apply(version, l, r, f) << '\n';
            }else if(type == "SET"){
                int version, k;
                std::int64_t value;
                std::cin >> version >> k >> value;
                std::cout << seg->set(version, k, value) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << seg->fork(version) << '\n';
            }else if(type == "GET"){
                int version, k;
                std::cin >> version >> k;
                std::cout << seg->get(version, k) << '\n';
            }
        }
        return 0;
    }

    constexpr int n = 41;
    std::mt19937 rng(1618033);
    std::vector<std::int64_t> initial(n);
    for(auto& x: initial){
        x = rng() % 100;
    }

    PersistentDualSegtree<affine, 64, 1800> seg(initial);
    std::vector<std::vector<std::int64_t>> versions = {initial};

    for(int step = 0; step < 1500; step++){
        int base = static_cast<int>(rng() % versions.size());
        auto next = versions[base];
        int type = static_cast<int>(rng() % 3);
        int version;
        if(type == 0){
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r){
                std::swap(l, r);
            }
            Affine f{
                static_cast<std::int64_t>(rng() % 5),
                static_cast<std::int64_t>(rng() % 20)
            };
            for(int k = l; k < r; k++){
                auto& x = next[static_cast<std::size_t>(k)];
                x = mapping(f, x);
            }
            version = seg.apply(base, l, r, f);
        }else if(type == 1){
            int k = static_cast<int>(rng() % n);
            std::int64_t x = rng() % 1000;
            next[static_cast<std::size_t>(k)] = x;
            version = seg.set(base, k, x);
        }else{
            version = seg.fork(base);
        }
        assert(version == static_cast<int>(versions.size()));
        versions.push_back(next);

        for(int query = 0; query < 20; query++){
            int v = static_cast<int>(rng() % versions.size());
            int k = static_cast<int>(rng() % n);
            assert(
                seg.get(v, k) ==
                versions[v][static_cast<std::size_t>(k)]
            );
        }
    }

    PartiallyPersistentDualSegtree<affine, 64, 1000> partial(initial);
    versions = {initial};
    for(int step = 0; step < 900; step++){
        auto next = versions.back();
        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r){
            std::swap(l, r);
        }
        Affine f{
            static_cast<std::int64_t>(rng() % 5),
            static_cast<std::int64_t>(rng() % 20)
        };
        for(int k = l; k < r; k++){
            auto& x = next[static_cast<std::size_t>(k)];
            x = mapping(f, x);
        }
        partial.apply(l, r, f);
        versions.push_back(next);

        int v = static_cast<int>(rng() % versions.size());
        int k = static_cast<int>(rng() % n);
        assert(
            partial.get(v, k) ==
            versions[v][static_cast<std::size_t>(k)]
        );
        assert(
            partial.get(k) ==
            versions.back()[static_cast<std::size_t>(k)]
        );
    }
    assert(partial.size() == n);
    assert(partial.versions() == static_cast<int>(versions.size()));
    assert(partial.latest_version() == partial.versions() - 1);
    assert(seg.size() == n);
    assert(seg.versions() == 1501 && seg.latest_version() == 1500);
}
