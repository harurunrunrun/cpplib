---
title: Dual Segment Tree (双対セグメント木)
documentation_of: ../../../../src/structure/segtree/basic/dualsegtree.hpp
---

作用付きモノイドに対して、区間への作用と 1 点の取得を行う。

## API

```cpp
template<auto Monoid_act, int MAX_SIZE = -1>
struct DualSegtree;

explicit DualSegtree(int n = MAX_SIZE)
explicit DualSegtree(const vector<S>& values)
void set(int k, const S& value)
S get(int k)
void apply(int l, int r, const T& action)
```

`S` は値の型、`T` は作用の型（`Monoid_act::S`, `Monoid_act::T`）である。

- コンストラクタは長さ `n` の単位元の列、または `values` の列を構築する。
- `set` は `k` 番目の要素を `value` に置き換える。
- `get` は遅延している作用を反映した `k` 番目の要素を返す。
- `apply` は半開区間 `[l, r)` の各要素に `action` を作用させる。1 点への作用は `apply(k, k + 1, action)` で行う。

## 時間計算量

- `DualSegtree(int)`, `DualSegtree(vector)`: $O(\texttt{MAX_SIZE})$
- `set`, `get`, `apply`: $O(\log \texttt{MAX_SIZE})$

保存領域は $O(\texttt{MAX_SIZE})$、各更新・クエリの追加空間計算量は $O(1)$ である。

## 注意点

- `0 <= n <= MAX_SIZE`、点は `0 <= k < n`、区間は `0 <= l <= r <= n` を要求する。範囲外または容量超過では `std::runtime_error` を送出する。
- `Monoid_act` は作用の合成順序と `mapping` の契約を満たす必要がある。

## 容量を省略する場合

容量のテンプレート引数のデフォルトは `-1` である。`DualSegtree<Monoid_act> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使う。非負の容量を指定する固定長配列版も利用できる。容量以外のテンプレート引数（モノイドなど）は引き続き指定する。

可変長版では木の幅・高さを実行時の入力から求め、要素数に比例する記憶領域を確保する。更新・クエリのアルゴリズムは固定長配列版と同じであり、上記の計算量の $\texttt{MAX_SIZE}$ は実際の要素数 $n$ に置き換わる。添字に使う整数型の表現範囲と実メモリの容量が上限となる。
