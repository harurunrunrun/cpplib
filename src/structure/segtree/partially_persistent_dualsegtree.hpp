#pragma once

#include <array>
#include <cstddef>
#include <vector>
#include "persistent_dualsegtree.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentDualSegtree{
    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;

private:
    PersistentDualSegtree<Monoid_act, MAX_SIZE, MAX_UPDATE> data;

public:
    explicit PartiallyPersistentDualSegtree(
        int n = MAX_SIZE
    ): data(n){}

    explicit PartiallyPersistentDualSegtree(
        const std::vector<S>& v
    ): data(v){}

    template<std::size_t N>
    explicit PartiallyPersistentDualSegtree(
        const std::array<S, N>& v
    ): data(v){}

    int size() const{
        return data.size();
    }

    int versions() const{
        return data.versions();
    }

    int latest_version() const{
        return data.latest_version();
    }

    int set(int k, const S& x){
        return data.set(data.latest_version(), k, x);
    }

    int apply(int l, int r, const T& f){
        return data.apply(data.latest_version(), l, r, f);
    }

    S get(int version, int k) const{
        return data.get(version, k);
    }

    S get(int k) const{
        return data.get(data.latest_version(), k);
    }
};
