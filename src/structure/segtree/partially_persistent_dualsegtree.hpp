#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "../other/partially_persistent_storage.hpp"
#include "../types/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_UPDATE>
struct PartiallyPersistentDualSegtree{
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
    static constexpr int lazy_capacity =
        static_cast<int>(static_cast<std::size_t>(MAX_UPDATE) * changes_per_update);

    int _n = 0;
    int version_count = 1;
    PartiallyPersistentStorage<S, sz, MAX_UPDATE> data;
    PartiallyPersistentStorage<T, 2 * sz, lazy_capacity> lazy;

    void initialize(int n, const S* values){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        _n = n;
        for(int k = 0; k < sz; k++){
            data.initialize(k, k < n && values != nullptr ? values[k] : Monoid_act.e());
        }
        for(int k = 0; k < 2 * sz; k++) lazy.initialize(k, Monoid_act.id());
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void write_lazy(int k, const T& x){ lazy.write(k, version_count, x); }
    void compose(int k, const T& f){
        write_lazy(k, Monoid_act.composition(f, lazy.current(k)));
    }
    void push(int k){
        compose(2 * k, lazy.current(k));
        compose(2 * k + 1, lazy.current(k));
        write_lazy(k, Monoid_act.id());
    }
    void apply_impl(int k, int l, int r, int ql, int qr, const T& f){
        if(ql <= l && r <= qr){ compose(k, f); return; }
        push(k);
        int mid = l + (r - l) / 2;
        if(ql < mid) apply_impl(2 * k, l, mid, ql, qr, f);
        if(mid < qr) apply_impl(2 * k + 1, mid, r, ql, qr, f);
    }
    void set_impl(int k, int l, int r, int p, const S& x){
        if(r - l == 1){
            data.write(l, version_count, x);
            write_lazy(k, Monoid_act.id());
            return;
        }
        push(k);
        int mid = l + (r - l) / 2;
        if(p < mid) set_impl(2 * k, l, mid, p, x);
        else set_impl(2 * k + 1, mid, r, p, x);
    }
    S get_impl(int version, int k, int l, int r, int p) const{
        if(r - l == 1){
            return Monoid_act.mapping(lazy.get(k, version), data.get(l, version));
        }
        int mid = l + (r - l) / 2;
        S value = p < mid
            ? get_impl(version, 2 * k, l, mid, p)
            : get_impl(version, 2 * k + 1, mid, r, p);
        return Monoid_act.mapping(lazy.get(k, version), value);
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
    PartiallyPersistentDualSegtree(const PartiallyPersistentDualSegtree&) = delete;
    PartiallyPersistentDualSegtree& operator=(const PartiallyPersistentDualSegtree&) = delete;
    PartiallyPersistentDualSegtree(PartiallyPersistentDualSegtree&&) = delete;
    PartiallyPersistentDualSegtree& operator=(PartiallyPersistentDualSegtree&&) = delete;

    explicit PartiallyPersistentDualSegtree(int n = MAX_SIZE){ initialize(n, nullptr); }
    explicit PartiallyPersistentDualSegtree(const std::vector<S>& v){
        initialize(static_cast<int>(v.size()), v.data());
    }
    template<std::size_t N>
    explicit PartiallyPersistentDualSegtree(const std::array<S, N>& v){
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
        return get_impl(version, 1, 0, sz, k);
    }
    S get(int k) const{ return get(latest_version(), k); }
};
