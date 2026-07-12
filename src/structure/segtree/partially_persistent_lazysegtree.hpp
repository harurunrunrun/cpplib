#pragma once

#include <array>
#include <cstddef>
#include <utility>
#include <vector>
#include "persistent_lazysegtree.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentLazySegtree{
    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;

private:
    PersistentLazySegtree<Monoid_act, MAX_SIZE, MAX_UPDATE> data;

public:
    PartiallyPersistentLazySegtree(
        const PartiallyPersistentLazySegtree&
    ) = delete;
    PartiallyPersistentLazySegtree& operator=(
        const PartiallyPersistentLazySegtree&
    ) = delete;
    PartiallyPersistentLazySegtree(
        PartiallyPersistentLazySegtree&&
    ) = delete;
    PartiallyPersistentLazySegtree& operator=(
        PartiallyPersistentLazySegtree&&
    ) = delete;

    explicit PartiallyPersistentLazySegtree(
        int n = MAX_SIZE
    ): data(n){}

    explicit PartiallyPersistentLazySegtree(
        const std::vector<S>& v
    ): data(v){}

    template<std::size_t N>
    explicit PartiallyPersistentLazySegtree(
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

    int apply(int k, const T& f){
        return data.apply(data.latest_version(), k, f);
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

    S prod(int version, int l, int r) const{
        return data.prod(version, l, r);
    }

    S prod(int l, int r) const{
        return data.prod(data.latest_version(), l, r);
    }

    S all_prod(int version) const{
        return data.all_prod(version);
    }

    S all_prod() const{
        return data.all_prod(data.latest_version());
    }

    int max_right(int version, int l, auto f, auto&&... args) const{
        return data.max_right(
            version, l, f, std::forward<decltype(args)>(args)...
        );
    }

    int max_right(int l, auto f, auto&&... args) const{
        return data.max_right(
            data.latest_version(), l, f,
            std::forward<decltype(args)>(args)...
        );
    }

    int min_left(int version, int r, auto f, auto&&... args) const{
        return data.min_left(
            version, r, f, std::forward<decltype(args)>(args)...
        );
    }

    int min_left(int r, auto f, auto&&... args) const{
        return data.min_left(
            data.latest_version(), r, f,
            std::forward<decltype(args)>(args)...
        );
    }
};
