#pragma once

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
#include "../types/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentLazySegtree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= 0);
    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;

    static constexpr int sz = []{
        int res = 1; while(res < MAX_SIZE) res <<= 1; return res;
    }();
    static constexpr int height = []{
        int res = 1, h = 0;
        while(res < MAX_SIZE){ res <<= 1; h++; }
        return h;
    }();

private:
    static constexpr std::size_t changes_per_update =
        static_cast<std::size_t>(8 * height) + 4;
    static_assert(
        static_cast<std::size_t>(MAX_UPDATE) <=
        static_cast<std::size_t>(std::numeric_limits<int>::max()) /
        changes_per_update
    );
    static constexpr int history_capacity =
        static_cast<int>(static_cast<std::size_t>(MAX_UPDATE) * changes_per_update);

    int _n = 0;
    int version_count = 1;
    PartiallyPersistentStorage<S, 2 * sz, history_capacity> data;
    PartiallyPersistentStorage<T, sz, history_capacity> lazy;

    void initialize(int n, const S* values){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        _n = n;
        std::array<std::optional<S>, 2 * sz> initial;
        for(int k = 0; k < sz; k++){
            if(k < n && values != nullptr) initial[k + sz].emplace(values[k]);
            else initial[k + sz].emplace(Monoid_act.e());
        }
        for(int k = sz - 1; k > 0; k--){
            initial[k].emplace(Monoid_act.op(*initial[2 * k], *initial[2 * k + 1]));
        }
        initial[0].emplace(Monoid_act.e());
        for(int k = 0; k < 2 * sz; k++) data.initialize(k, *initial[k]);
        for(int k = 0; k < sz; k++) lazy.initialize(k, Monoid_act.id());
    }
    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }
    void write_data(int k, const S& x){ data.write(k, version_count, x); }
    void write_lazy(int k, const T& x){ lazy.write(k, version_count, x); }
    void update(int k){
        write_data(k, Monoid_act.op(data.current(2 * k), data.current(2 * k + 1)));
    }
    void all_apply(int k, const T& f){
        write_data(k, Monoid_act.mapping(f, data.current(k)));
        if(k < sz){
            write_lazy(k, Monoid_act.composition(f, lazy.current(k)));
        }
    }
    void push(int k){
        T action = lazy.current(k);
        all_apply(2 * k, action);
        all_apply(2 * k + 1, action);
        write_lazy(k, Monoid_act.id());
    }
    void set_impl(int k, int l, int r, int p, const S& x){
        if(r - l == 1){ write_data(k, x); return; }
        push(k);
        int mid = l + (r - l) / 2;
        if(p < mid) set_impl(2 * k, l, mid, p, x);
        else set_impl(2 * k + 1, mid, r, p, x);
        update(k);
    }
    void apply_impl(int k, int l, int r, int ql, int qr, const T& f){
        if(ql <= l && r <= qr){ all_apply(k, f); return; }
        push(k);
        int mid = l + (r - l) / 2;
        if(ql < mid) apply_impl(2 * k, l, mid, ql, qr, f);
        if(mid < qr) apply_impl(2 * k + 1, mid, r, ql, qr, f);
        update(k);
    }
    S prod_impl(
        int version, int k, int l, int r, int ql, int qr, const T& carry
    ) const{
        if(r <= ql || qr <= l) return Monoid_act.e();
        if(ql <= l && r <= qr){
            return Monoid_act.mapping(carry, data.get(k, version));
        }
        T next = k < sz
            ? Monoid_act.composition(carry, lazy.get(k, version)) : carry;
        int mid = l + (r - l) / 2;
        return Monoid_act.op(
            prod_impl(version, 2 * k, l, mid, ql, qr, next),
            prod_impl(version, 2 * k + 1, mid, r, ql, qr, next)
        );
    }
    template<class F, class... Args>
    int max_right_impl(
        int version, int k, int l, int r, int ql, const T& carry,
        S& sm, F& f, Args&... args
    ) const{
        if(r <= ql || _n <= l) return -1;
        if(ql <= l && r <= _n){
            S value = Monoid_act.mapping(carry, data.get(k, version));
            S next_value = Monoid_act.op(sm, value);
            if(std::invoke(f, next_value, args...)){ sm = next_value; return -1; }
            if(r - l == 1) return l;
        }
        T next = k < sz
            ? Monoid_act.composition(carry, lazy.get(k, version)) : carry;
        int mid = l + (r - l) / 2;
        int res = max_right_impl(
            version, 2 * k, l, mid, ql, next, sm, f, args...
        );
        if(res != -1) return res;
        return max_right_impl(
            version, 2 * k + 1, mid, r, ql, next, sm, f, args...
        );
    }
    template<class F, class... Args>
    int min_left_impl(
        int version, int k, int l, int r, int qr, const T& carry,
        S& sm, F& f, Args&... args
    ) const{
        if(qr <= l || _n <= l) return -1;
        if(r <= qr && r <= _n){
            S value = Monoid_act.mapping(carry, data.get(k, version));
            S next_value = Monoid_act.op(value, sm);
            if(std::invoke(f, next_value, args...)){ sm = next_value; return -1; }
            if(r - l == 1) return r;
        }
        T next = k < sz
            ? Monoid_act.composition(carry, lazy.get(k, version)) : carry;
        int mid = l + (r - l) / 2;
        int res = min_left_impl(
            version, 2 * k + 1, mid, r, qr, next, sm, f, args...
        );
        if(res != -1) return res;
        return min_left_impl(
            version, 2 * k, l, mid, qr, next, sm, f, args...
        );
    }
    template<class F>
    int perform_update(F&& function){
        if(version_count > MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (update).");
        }
        int data_snapshot = data.changes();
        int lazy_snapshot = lazy.changes();
        try{ std::forward<F>(function)(); }
        catch(...){
            data.rollback(data_snapshot);
            lazy.rollback(lazy_snapshot);
            throw;
        }
        return version_count++;
    }

public:
    PartiallyPersistentLazySegtree(const PartiallyPersistentLazySegtree&) = delete;
    PartiallyPersistentLazySegtree& operator=(const PartiallyPersistentLazySegtree&) = delete;
    PartiallyPersistentLazySegtree(PartiallyPersistentLazySegtree&&) = delete;
    PartiallyPersistentLazySegtree& operator=(PartiallyPersistentLazySegtree&&) = delete;

    explicit PartiallyPersistentLazySegtree(int n = MAX_SIZE){ initialize(n, nullptr); }
    explicit PartiallyPersistentLazySegtree(const std::vector<S>& v){
        initialize(static_cast<int>(v.size()), v.data());
    }
    template<std::size_t N>
    explicit PartiallyPersistentLazySegtree(const std::array<S, N>& v){
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
        return perform_update([&]{ set_impl(1, 0, sz, k, x); });
    }
    int apply(int k, const T& f){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (apply).");
        }
        return perform_update([&]{ apply_impl(1, 0, sz, k, k + 1, f); });
    }
    int apply(int l, int r, const T& f){
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (apply).");
        }
        return perform_update([&]{ if(l != r) apply_impl(1, 0, sz, l, r, f); });
    }
    S get(int version, int k) const{
        check_version(version, "library assertion fault: range violation (get).");
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return prod_impl(version, 1, 0, sz, k, k + 1, Monoid_act.id());
    }
    S get(int k) const{ return get(latest_version(), k); }
    S prod(int version, int l, int r) const{
        check_version(version, "library assertion fault: range violation (prod).");
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prod).");
        }
        return prod_impl(version, 1, 0, sz, l, r, Monoid_act.id());
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
        S sm = Monoid_act.e();
        if(!std::invoke(f, sm, args...))[[unlikely]]{
            throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        }
        if(l == _n) return _n;
        int res = max_right_impl(
            version, 1, 0, sz, l, Monoid_act.id(), sm, f, args...
        );
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
        S sm = Monoid_act.e();
        if(!std::invoke(f, sm, args...))[[unlikely]]{
            throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        }
        if(r == 0) return 0;
        int res = min_left_impl(
            version, 1, 0, sz, r, Monoid_act.id(), sm, f, args...
        );
        return res == -1 ? 0 : res;
    }
    int min_left(int r, auto f, auto&&... args) const{
        return min_left(latest_version(), r, f, std::forward<decltype(args)>(args)...);
    }
};
