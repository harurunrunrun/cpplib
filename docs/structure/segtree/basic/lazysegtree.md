---
title: Lazy Segment Tree (遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/basic/lazysegtree.hpp
---

作用付きモノイドに対して、区間への作用と区間積を扱う遅延セグメント木である。

## API

```cpp
template<auto Monoid_act, int MAX_SIZE = -1>
struct LazySegtree;

explicit LazySegtree(int n = MAX_SIZE)
explicit LazySegtree(const vector<S>& values)
void set(int k, const S& value)
S get(int k)
S prod(int l, int r)
S all_prod() const
void apply(int k, const T& action)
void apply(int l, int r, const T& action)
int max_right(int l, auto predicate, auto&&... args)
int min_left(int r, auto predicate, auto&&... args)
```

`S` は値の型、`T` は作用の型（`Monoid_act::S`, `Monoid_act::T`）である。

- `set` / `get` は 1 点、`prod` は半開区間 `[l, r)`、`all_prod` は列全体を扱う。
- `apply` は 1 点または半開区間 `[l, r)` に `action` を作用させる。
- `max_right(l, predicate, args...)` は `predicate(prod(l, r), args...)` が `true` となる最大の `r` を、`min_left(r, predicate, args...)` は `predicate(prod(l, r), args...)` が `true` となる最小の `l` を返す。`predicate` は単調で、単位元に対して `true` でなければならない。

## 時間計算量

- `LazySegtree(int)`, `LazySegtree(vector)`: $O(\texttt{MAX_SIZE})$
- `set`, `get`, 1 点の `apply`, 区間の `apply`, `prod`: $O(\log \texttt{MAX_SIZE})$
- `max_right`, `min_left`: $O(\log \texttt{MAX_SIZE})$
- `all_prod`: $O(1)$

保存領域は $O(\texttt{MAX_SIZE})$、各更新・クエリの追加空間計算量は $O(1)$ である。

## 注意点

- `0 <= n <= MAX_SIZE`、点は `0 <= k < n`、区間は `0 <= l <= r <= n` を要求する。
- 容量・範囲の違反と、`predicate` が単位元に対して `false` を返す場合は `std::runtime_error` を送出する。
- `Monoid_act` は作用付きモノイドの契約を満たす必要がある。

## 容量を省略する場合

容量のテンプレート引数のデフォルトは `-1` である。`LazySegtree<Monoid_act> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使う。非負の容量を指定する固定長配列版も利用できる。容量以外のテンプレート引数（モノイドなど）は引き続き指定する。

可変長版では木の幅・高さを実行時の入力から求め、要素数に比例する記憶領域を確保する。更新・クエリのアルゴリズムは固定長配列版と同じであり、上記の計算量の $\texttt{MAX_SIZE}$ は実際の要素数 $n$ に置き換わる。添字に使う整数型の表現範囲と実メモリの容量が上限となる。
