---
title: Fenwick Tree (フェンウィック木) [DSL_2_B] [point_add_range_sum]
documentation_of: ../../../src/structure/fenwick_tree/fenwick_tree.hpp
---

一点加算と区間和を扱う Fenwick Tree（Binary Indexed Tree）。

## 使い方

```cpp
FenwickTree<long long, 200000> fw(n);  // 固定長版
FenwickTree<long long> bit(n);         // 可変長版
fw.add(3, 10);
long long s = fw.sum(2, 5);
```

テンプレート引数は `FenwickTree<T, MAX_SIZE = -1>`。`MAX_SIZE` を省略（`-1`）すると、実行時の要素数に合わせて `std::vector` を確保する可変長版になる。

## メンバ関数

- `explicit FenwickTree(int n = MAX_SIZE)`
    - 長さ $n$ の列を `T{}` で初期化する。可変長版では `n` のデフォルトは $0$ である。
- `explicit FenwickTree(const vector<T>& v)`
    - `v` で初期化する。
- `int size() const`
    - 長さを返す。
- `bool empty() const`
    - 長さが $0$ かどうかを返す。
- `void add(int k, T x)`
    - $a_k \gets a_k + x$ とする。
- `T prefix_sum(int r) const`
    - $a_0 + \dots + a_{r-1}$ を返す。
- `T sum(int l, int r) const`
    - $a_l + \dots + a_{r-1}$ を返す。
- `T get(int k) const`
    - $a_k$ を返す。
- `void set(int k, T x)`
    - $a_k \gets x$ とする。

## 時間計算量

- `FenwickTree(int)`: $O(n)$。固定長版では配列全体の初期化に $O(\texttt{MAX_SIZE})$ かかる
- `FenwickTree(vector)`: 上記に加えて $O(n)$（各ノードの値を親へ足し込む線形時間の構築）
- `add`、`prefix_sum`、`sum`、`get`、`set`: $O(\log n)$
- `size`、`empty`: $O(1)$

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE})$、可変長版: $O(n)$
- 各関数の追加領域: $O(1)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ でなければならない。`vector` の長さが上限を超える場合を含め、コンストラクタの範囲違反は `std::runtime_error` を送出する。
- 添字は $[0, n)$、`prefix_sum` の終端は $[0, n]$、区間は $0 \le l \le r \le n$ でなければならない。違反すると `std::runtime_error` を送出する。
- `T` はデフォルト構築、加算、減算ができる必要がある。
- `vector` からの構築は `add` を繰り返す場合と加算の順序が異なる。浮動小数点型では丸め誤差の出方が変わりうる。
