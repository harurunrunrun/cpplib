---
title: Dynamic Lazy Segment Tree (動的遅延セグメント木)
documentation_of: ../src/structure/segtree/dynamic_lazysegtree.hpp
---

巨大な添字範囲を疎に確保する遅延セグメント木。

## 時間計算量

- constructor: $O(1)$
- `set`, `get`, 1点`apply`, 区間`apply`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`: $O(1)$

更新で作られるノード数は $O(\log \mathtt{MAX\_SIZE})$。

## API

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

## 注意点

領域は `[0,MAX_SIZE)`、未生成点は `Monoid.e()`。点・区間違反では `runtime_error`、
node確保失敗では `bad_alloc`。境界探索のpredicateは単調で単位元に対してtrueが必要。
`Monoid` は作用付きモノイドの契約を満たす。保存領域は生成node数を $K$ として $O(K)$。
更新1回の追加nodeと各操作の再帰stackは $O(\log \mathtt{MAX\_SIZE})$、それ以外の
追加空間計算量は $O(1)$。
