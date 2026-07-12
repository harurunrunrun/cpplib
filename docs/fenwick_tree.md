---
title: Fenwick Tree
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

## 計算量

- 構築: $O(n \log n)$
- `add`, `prefix_sum`, `sum`, `get`, `set`: $O(\log n)$
