---
title: Dynamic Lazy Segment Tree (動的遅延セグメント木)
documentation_of: ../src/structure/segtree/dynamic_lazysegtree.hpp
---

巨大な添字範囲を疎に確保する遅延セグメント木。

# 計算量

- constructor: $O(1)$
- `set`, `get`, 1点`apply`, 区間`apply`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`: $O(1)$

更新で作られるノード数は $O(\log \mathtt{MAX\_SIZE})$。

# API

```cpp
DynamicLazySegtree<Monoid, MAX_SIZE>()
void set(long long k, const S& value)
S get(long long k) const
S prod(long long l, long long r) const
S all_prod() const
void apply(long long k, const F& action)
void apply(long long l, long long r, const F& action)
long long max_right(long long l, auto predicate, auto&&... args) const
long long min_left(long long r, auto predicate, auto&&... args) const
```

領域は `[0,MAX_SIZE)`、未生成点は単位元。`set/get` は1点、`prod/apply` は
半開区間を扱う。境界探索のpredicateは単調かつ単位元に対してtrueが必要。
範囲・predicate違反は `runtime_error`、確保失敗は `bad_alloc`。copyは禁止。
各APIの計算量は上表の通り。
