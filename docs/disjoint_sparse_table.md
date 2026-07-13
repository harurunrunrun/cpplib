---
title: Disjoint Sparse Table
documentation_of: ../src/structure/other/disjoint_sparse_table.hpp
---

静的な列に対するモノイドの区間取得。

演算は結合的である必要がある。冪等でなくてもよい。

## 使い方

```cpp
constexpr AddMonoid<long long> add_monoid{};
DisjointSparseTable<add_monoid, 200000> dst(a);
long long x = dst.prod(l, r);
```

## メンバ関数

- `DisjointSparseTable(int n = MAX_SIZE)`
    - 長さ `n` の列を単位元で初期化する。
- `DisjointSparseTable(vector<S> v)`
    - `v` で初期化する。
- `int size() const`
    - 長さを返す。
- `bool empty() const`
    - 長さが `0` かを返す。
- `S get(int k) const`
    - `a[k]` を返す。
- `S prod(int l, int r) const`
    - 区間 `[l, r)` の演算結果を返す。
- `S all_prod() const`
    - 区間 `[0, n)` の演算結果を返す。

## 計算量

- `DisjointSparseTable(int)`: $O(\mathtt{MAX\_SIZE}\log \mathtt{MAX\_SIZE})$
- `DisjointSparseTable(vector)`: $O(\mathtt{MAX\_SIZE}\log \mathtt{MAX\_SIZE}+n\log n)$
- `get`: $O(1)$
- `prod`, `all_prod`: $O(1)$
- `size`, `empty`: $O(1)$
