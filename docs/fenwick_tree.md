---
title: Fenwick Tree (フェンウィック木) [ABC231_F] [DSL_2_B] [point_add_range_sum] [RRSCHED] [TRIPINV] [UPDATEIT] [vertex_add_path_sum]
documentation_of: ../src/structure/other/fenwick_tree.hpp
---

一点加算・区間和。

## 使い方

```cpp
FenwickTree<long long, 200000> fw(n);
fw.add(3, 10);
long long s = fw.sum(2, 5);
```

## メンバ関数

- `FenwickTree(int n = MAX_SIZE)`
    - 長さ `n` の列を `0` で初期化する。
- `FenwickTree(vector<T> v)`
    - `v` で初期化する。
- `int size() const`
    - 長さを返す。
- `bool empty() const`
    - 長さが `0` かを返す。
- `void add(int k, T x)`
    - `a[k] += x`。
- `T prefix_sum(int r) const`
    - `sum(a[0], ..., a[r - 1])` を返す。
- `T sum(int l, int r) const`
    - `sum(a[l], ..., a[r - 1])` を返す。
- `T get(int k) const`
    - `a[k]` を返す。
- `void set(int k, T x)`
    - `a[k] = x`。

## 時間計算量

- `FenwickTree(int)`: $O(\mathtt{MAX\_SIZE})$
- `FenwickTree(vector)`: $O(\mathtt{MAX\_SIZE}+n\log n)$
- `add`, `prefix_sum`, `sum`, `get`, `set`: $O(\log n)$
- `size`, `empty`: $O(1)$

## 空間計算量（API別の追加領域）

- 2種類のconstructor、`size`、`empty`、`add`、`prefix_sum`、`sum`、`get`、`set`: $O(1)$

本体の固定保存領域は $O(\mathtt{MAX\_SIZE})$ である。

## 注意点

- `0 <= n <= MAX_SIZE`。vector長超過を含め、constructorの範囲違反は例外。
- indexは`[0,n)`、prefix終端は`[0,n]`、区間は`0 <= l <= r <= n`。違反時は例外。
- `T`はdefault構築、加算、減算を使える必要がある。
- 保存領域は固定$O(MAX\_SIZE)$、各更新・queryの追加領域は$O(1)$。
