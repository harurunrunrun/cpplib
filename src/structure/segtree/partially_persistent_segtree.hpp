#pragma once

#include <array>
#include <cstddef>
#include <utility>
#include <vector>
#include "persistent_segtree.hpp"

template<auto Monoid, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentSegtree{
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    PersistentSegtree<Monoid, MAX_SIZE, MAX_UPDATE> data;

public:
    PartiallyPersistentSegtree(const PartiallyPersistentSegtree&) = delete;
    PartiallyPersistentSegtree& operator=(const PartiallyPersistentSegtree&) = delete;
    PartiallyPersistentSegtree(PartiallyPersistentSegtree&&) = delete;
    PartiallyPersistentSegtree& operator=(PartiallyPersistentSegtree&&) = delete;

    explicit PartiallyPersistentSegtree(int n = MAX_SIZE): data(n){}

    explicit PartiallyPersistentSegtree(const std::vector<S>& v): data(v){}

    template<std::size_t N>
    explicit PartiallyPersistentSegtree(const std::array<S, N>& v): data(v){}

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
