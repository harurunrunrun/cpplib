#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "../other/partially_persistent_storage.hpp"
#include "../types/monoid.hpp"

template<auto Monoid, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentSegtree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= 0);

    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

    static constexpr int sz = []{
        int res = 1;
        while(res < MAX_SIZE) res <<= 1;
        return res;
    }();
    static constexpr int height = []{
        int res = 1, h = 0;
        while(res < MAX_SIZE){ res <<= 1; h++; }
        return h;
    }();

private:
    static_assert(
        static_cast<std::size_t>(MAX_UPDATE) <=
        static_cast<std::size_t>(std::numeric_limits<int>::max()) /
        static_cast<std::size_t>(height + 1)
    );
    static constexpr int change_capacity = MAX_UPDATE * (height + 1);

    int _n = 0;
    int version_count = 1;
    PartiallyPersistentStorage<S, 2 * sz, change_capacity> data;

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void initialize(int n, const S* values){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        _n = n;
        std::array<std::optional<S>, 2 * sz> initial;
        for(int k = 0; k < sz; k++){
            if(k < n && values != nullptr) initial[k + sz].emplace(values[k]);
            else initial[k + sz].emplace(Monoid.e());
        }
        for(int k = sz - 1; k > 0; k--){
            initial[k].emplace(Monoid.op(*initial[2 * k], *initial[2 * k + 1]));
        }
        initial[0].emplace(Monoid.e());
        for(int k = 0; k < 2 * sz; k++) data.initialize(k, *initial[k]);
    }

    S prod_impl(int version, int node, int l, int r, int ql, int qr) const{
        if(r <= ql || qr <= l) return Monoid.e();
        if(ql <= l && r <= qr) return data.get(node, version);
        int mid = l + (r - l) / 2;
        return Monoid.op(
            prod_impl(version, 2 * node, l, mid, ql, qr),
            prod_impl(version, 2 * node + 1, mid, r, ql, qr)
        );
    }

    template<class F, class... Args>
    int max_right_impl(
        int version, int node, int l, int r, int ql,
        S& sm, F& f, Args&... args
    ) const{
        if(r <= ql || _n <= l) return -1;
        if(ql <= l && r <= _n){
            S next = Monoid.op(sm, data.get(node, version));
            if(std::invoke(f, next, args...)){ sm = next; return -1; }
            if(r - l == 1) return l;
        }
        int mid = l + (r - l) / 2;
        int res = max_right_impl(version, 2 * node, l, mid, ql, sm, f, args...);
        if(res != -1) return res;
        return max_right_impl(version, 2 * node + 1, mid, r, ql, sm, f, args...);
    }

    template<class F, class... Args>
    int min_left_impl(
        int version, int node, int l, int r, int qr,
        S& sm, F& f, Args&... args
    ) const{
        if(qr <= l || _n <= l) return -1;
        if(r <= qr && r <= _n){
            S next = Monoid.op(data.get(node, version), sm);
            if(std::invoke(f, next, args...)){ sm = next; return -1; }
            if(r - l == 1) return r;
        }
        int mid = l + (r - l) / 2;
        int res = min_left_impl(version, 2 * node + 1, mid, r, qr, sm, f, args...);
        if(res != -1) return res;
        return min_left_impl(version, 2 * node, l, mid, qr, sm, f, args...);
    }

public:
    PartiallyPersistentSegtree(const PartiallyPersistentSegtree&) = delete;
    PartiallyPersistentSegtree& operator=(const PartiallyPersistentSegtree&) = delete;
    PartiallyPersistentSegtree(PartiallyPersistentSegtree&&) = delete;
    PartiallyPersistentSegtree& operator=(PartiallyPersistentSegtree&&) = delete;

    explicit PartiallyPersistentSegtree(int n = MAX_SIZE){ initialize(n, nullptr); }
    explicit PartiallyPersistentSegtree(const std::vector<S>& v){
        initialize(static_cast<int>(v.size()), v.data());
    }
    template<std::size_t N>
    explicit PartiallyPersistentSegtree(const std::array<S, N>& v){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        initialize(static_cast<int>(N), v.data());
    }

    int size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }

    int set(int k, const S& x){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (set).");
        }
        if(version_count > MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (set).");
        }
        int snapshot = data.changes();
        try{
            int node = k + sz;
            data.write(node, version_count, x);
            while(node >>= 1){
                data.write(
                    node, version_count,
                    Monoid.op(data.current(2 * node), data.current(2 * node + 1))
                );
            }
        }catch(...){
            data.rollback(snapshot);
            throw;
        }
        return version_count++;
    }

    S get(int version, int k) const{
        check_version(version, "library assertion fault: range violation (get).");
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return data.get(k + sz, version);
    }
    S get(int k) const{ return get(latest_version(), k); }

    S prod(int version, int l, int r) const{
        check_version(version, "library assertion fault: range violation (prod).");
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prod).");
        }
        return prod_impl(version, 1, 0, sz, l, r);
    }
    S prod(int l, int r) const{ return prod(latest_version(), l, r); }

    S all_prod(int version) const{
        check_version(version, "library assertion fault: range violation (all_prod).");
        return data.get(1, version);
    }
    S all_prod() const{ return all_prod(latest_version()); }

    int max_right(int version, int l, auto f, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (max_right).");
        if(l < 0 || _n < l)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (max_right).");
        }
        S sm = Monoid.e();
        if(!std::invoke(f, sm, args...))[[unlikely]]{
            throw std::runtime_error("library assertion fault: identity violation (max_right).");
        }
        if(l == _n) return _n;
        int res = max_right_impl(version, 1, 0, sz, l, sm, f, args...);
        return res == -1 ? _n : res;
    }
    int max_right(int l, auto f, auto&&... args) const{
        return max_right(latest_version(), l, f, std::forward<decltype(args)>(args)...);
    }

    int min_left(int version, int r, auto f, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (min_left).");
        if(r < 0 || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (min_left).");
        }
        S sm = Monoid.e();
        if(!std::invoke(f, sm, args...))[[unlikely]]{
            throw std::runtime_error("library assertion fault: identity violation (min_left).");
        }
        if(r == 0) return 0;
        int res = min_left_impl(version, 1, 0, sz, r, sm, f, args...);
        return res == -1 ? 0 : res;
    }
    int min_left(int r, auto f, auto&&... args) const{
        return min_left(latest_version(), r, f, std::forward<decltype(args)>(args)...);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_HPP_INCLUDED
