// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "../../src/structure/segtree/partially_persistent_segtree.hpp"
#include "../../src/structure/segtree/rollback_segtree.hpp"

using ll = long long;

ll sum_op(ll a, ll b){
    return a + b;
}

ll sum_e(){
    return 0;
}

constexpr Monoid<sum_op, sum_e> sum_monoid{};

bool sum_leq(ll value, ll limit){
    return value <= limit;
}

std::string concat_op(std::string a, std::string b){
    return a + b;
}

std::string concat_e(){
    return {};
}

constexpr Monoid<concat_op, concat_e> concat_monoid{};

using Persistent = PersistentSegtree<sum_monoid, 32, 3000>;
using Partial = PartiallyPersistentSegtree<sum_monoid, 32, 2000>;
using Rollback = RollbackSegtree<sum_monoid, 32, 2000>;

static_assert(!std::is_copy_constructible_v<Persistent>);
static_assert(!std::is_move_constructible_v<Persistent>);
static_assert(!std::is_copy_constructible_v<Partial>);
static_assert(!std::is_move_constructible_v<Partial>);
static_assert(!std::is_copy_constructible_v<Rollback>);
static_assert(!std::is_move_constructible_v<Rollback>);

int naive_max_right(const std::vector<ll>& a, int l, ll limit){
    ll sum = 0;
    while(l < (int)a.size() && sum + a[l] <= limit){
        sum += a[l++];
    }
    return l;
}

int naive_min_left(const std::vector<ll>& a, int r, ll limit){
    ll sum = 0;
    while(0 < r && a[r - 1] + sum <= limit){
        sum += a[--r];
    }
    return r;
}

int main(){
    constexpr int N = 29;
    std::mt19937_64 rng(0x2718281828459045ULL);

    std::vector<ll> initial(N);
    for(ll& x: initial){
        x = (ll)(rng() % 21);
    }

    Persistent persistent(initial);
    std::vector<std::vector<ll>> versions(1, initial);

    for(int q = 0; q < 3000; q++){
        int base = (int)(rng() % versions.size());
        int version;
        if(rng() % 4 != 0){
            int k = (int)(rng() % N);
            ll x = (ll)(rng() % 21);
            auto next = versions[base];
            next[k] = x;
            version = persistent.set(base, k, x);
            versions.push_back(std::move(next));
        }else{
            version = persistent.fork(base);
            versions.push_back(versions[base]);
        }

        assert(version == q + 1);
        assert(persistent.versions() == q + 2);
        assert(persistent.latest_version() == q + 1);

        int v = (int)(rng() % versions.size());
        int l = (int)(rng() % (N + 1));
        int r = l + (int)(rng() % (N - l + 1));
        ll sum = std::accumulate(
            versions[v].begin() + l,
            versions[v].begin() + r,
            0LL
        );
        assert(persistent.prod(v, l, r) == sum);
        assert(
            persistent.all_prod(v) ==
            std::accumulate(versions[v].begin(), versions[v].end(), 0LL)
        );

        int k = (int)(rng() % N);
        assert(persistent.get(v, k) == versions[v][k]);

        ll limit = (ll)(rng() % 151);
        assert(
            persistent.max_right(v, l, sum_leq, limit) ==
            naive_max_right(versions[v], l, limit)
        );
        assert(
            persistent.min_left(v, r, sum_leq, limit) ==
            naive_min_left(versions[v], r, limit)
        );
    }

    bool thrown = false;
    try{
        persistent.fork(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    std::array<std::string, 4> words{"a", "b", "c", "d"};
    PersistentSegtree<concat_monoid, 4, 3> persistent_string(words);
    int changed = persistent_string.set(0, 1, std::string("X"));
    assert(persistent_string.prod(0, 0, 4) == "abcd");
    assert(persistent_string.prod(changed, 0, 4) == "aXcd");

    Partial partial(initial);
    std::vector<std::vector<ll>> partial_versions(1, initial);
    for(int q = 0; q < 2000; q++){
        int k = (int)(rng() % N);
        ll x = (ll)(rng() % 21);
        auto next = partial_versions.back();
        next[k] = x;
        int version = partial.set(k, x);
        partial_versions.push_back(std::move(next));
        assert(version == q + 1);

        int v = (int)(rng() % partial_versions.size());
        int l = (int)(rng() % (N + 1));
        int r = l + (int)(rng() % (N - l + 1));
        ll sum = std::accumulate(
            partial_versions[v].begin() + l,
            partial_versions[v].begin() + r,
            0LL
        );
        assert(partial.prod(v, l, r) == sum);
        assert(partial.get(k) == partial_versions.back()[k]);
    }

    Rollback rollback(initial);
    std::vector<std::vector<ll>> states(1, initial);
    for(int q = 0; q < 10000; q++){
        int operation = (int)(rng() % 4);
        if(operation < 2 && rollback.history_size() < 2000){
            int k = (int)(rng() % N);
            ll x = (ll)(rng() % 21);
            auto next = states.back();
            next[k] = x;
            rollback.set(k, x);
            states.push_back(std::move(next));
        }else if(operation == 2 && rollback.can_undo()){
            rollback.undo();
            states.pop_back();
        }else{
            int snapshot = (int)(rng() % states.size());
            rollback.rollback(snapshot);
            states.resize(snapshot + 1);
        }

        assert(rollback.snapshot() == (int)states.size() - 1);
        int l = (int)(rng() % (N + 1));
        int r = l + (int)(rng() % (N - l + 1));
        ll sum = std::accumulate(
            states.back().begin() + l,
            states.back().begin() + r,
            0LL
        );
        assert(rollback.prod(l, r) == sum);
        assert(
            rollback.all_prod() ==
            std::accumulate(states.back().begin(), states.back().end(), 0LL)
        );

        ll limit = (ll)(rng() % 151);
        assert(
            rollback.max_right(l, sum_leq, limit) ==
            naive_max_right(states.back(), l, limit)
        );
        assert(
            rollback.min_left(r, sum_leq, limit) ==
            naive_min_left(states.back(), r, limit)
        );
    }

    RollbackSegtree<concat_monoid, 4, 2> rollback_string(words);
    auto snapshot = rollback_string.snapshot();
    rollback_string.set(2, std::string("Y"));
    assert(rollback_string.prod(0, 4) == "abYd");
    rollback_string.rollback(snapshot);
    assert(rollback_string.prod(0, 4) == "abcd");
}
