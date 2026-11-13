---
title: Dynamic Lazy Segment Tree (動的遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/dynamic/dynamic_lazysegtree.hpp
---

大きな添字範囲を扱い、ノードを必要な部分だけ疎に確保する遅延セグメント木である。

## API

```cpp
template<auto Monoid, long long MAX_SIZE = -1>
struct DynamicLazySegtree;

explicit DynamicLazySegtree(long long n = MAX_SIZE)
void set(long long k, const S& value)
S get(long long k) const
S prod(long long l, long long r) const
S all_prod() const
void apply(long long k, const T& action)
void apply(long long l, long long r, const T& action)
```

`S` は値の型、`T` は作用の型である。

- 添字の範囲は $[0, n)$（$n$ はコンストラクタの引数）で、まだ生成されていない点の値は単位元 `Monoid.e()` である。
- `set` / `get` は 1 点を、`prod` は半開区間 `[l, r)` の積を、`all_prod` は全体の積を扱う。
- `apply` は 1 点または半開区間 `[l, r)` に `action` を作用させる。

## 時間計算量

- コンストラクタ: $O(1)$
- `set`, `get`, 1 点の `apply`, 区間の `apply`, `prod`: $O(\log n)$
- `all_prod`: $O(1)$

更新 1 回で作られるノード数は $O(\log n)$ である。保存領域は生成したノード数を $K$ として $O(K)$ で、各操作の再帰スタックは $O(\log n)$、それ以外の追加の空間計算量は $O(1)$ である。

## 注意点

- `0 <= n <= MAX_SIZE` を要求する。点・区間の範囲違反では `std::runtime_error` を、ノードの確保に失敗すると `std::bad_alloc` を送出する。
- `Monoid` は作用付きモノイドの契約を満たす必要がある。
- `max_right` / `min_left` は提供しない。
- コピーはできない。

## 容量を省略する場合

`MAX_SIZE` のデフォルトは `-1` である。`DynamicLazySegtree<Monoid> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使う（コンストラクタのデフォルトは `n = 0`）。ノードは使用量に応じて確保し、更新・クエリのアルゴリズムは固定長版と同じである。
