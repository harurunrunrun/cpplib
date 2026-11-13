---
title: Wide Segtree (ワイドセグメント木) [point_add_range_sum]
documentation_of: ../../../../src/structure/segtree/basic/widesegtree.hpp
---

分岐数の大きい動的セグメント木である。値は `long long`、演算は和である。

## テンプレート引数

```cpp
template<long long MAX_SIZE = -1>
struct Wide_Segtree;
```

添字の範囲 $[0, \texttt{MAX_SIZE})$ を扱う。ノードは必要な部分だけ動的に確保する。

## 1 点更新

```cpp
void set(long long idx, long long x)
void add(long long idx, long long x)
long long get(long long idx)
```

`set(idx, x)` は 1 点を `x` に置き換え、`add(idx, x)` は 1 点に `x` を加え、`get(idx)` は 1 点の値を返す。

時間計算量: `set`, `add`, `get` は $O(\log_B \texttt{MAX_SIZE})$

## 区間和

```cpp
long long prod(long long l, long long r)
long long all_prod()
```

`prod(l, r)` は半開区間 `[l, r)` の和を、`all_prod` は全体の和を返す。

時間計算量: `prod` は $O(\log_B \texttt{MAX_SIZE})$、`all_prod` は $O(1)$

## 構築とコピー

- デフォルトコンストラクタ: $O(1)$
- コピーコンストラクタ、コピー代入: $O(U)$

$B$ は実行環境に応じて選ばれる分岐数、$U$ は確保済みのノード数である。1 点更新 1 回で新しく確保するノード数は $O(\log_B \texttt{MAX_SIZE})$ である。

## 注意点

- 点・区間が $[0, \texttt{MAX_SIZE})$ の外なら `std::runtime_error` を送出する。メモリの確保に失敗すると `std::bad_alloc` を送出する。

## 容量を省略する場合

`MAX_SIZE` のデフォルトは `-1` である。`Wide_Segtree<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使い、添字の範囲は $[0, n)$ になる（上記の計算量の $\texttt{MAX_SIZE}$ は $n$ に置き換わる）。このコンストラクタは `explicit Wide_Segtree(long long n = 0)` で、`n` が負または大きすぎる場合は `std::length_error` を送出する。
