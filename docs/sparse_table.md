---
title: Sparse Table (スパーステーブル) [staticrmq]
documentation_of: ../src/structure/other/sparse_table.hpp
---

静的な列に対する冪等な演算の区間取得。

演算は結合的かつ冪等である必要がある。最小値、最大値、gcd などに使える。

## 使い方

```cpp
constexpr MinMonoid<int> min_monoid{};
SparseTable<min_monoid, 200000> st(a);
int x = st.prod(l, r);
```

## メンバ関数

- `SparseTable(int n = MAX_SIZE)`
    - 長さ `n` の列を単位元で初期化する。
- `SparseTable(vector<S> v)`
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

## 時間計算量

- `SparseTable(int)`: $O(\mathtt{MAX\_SIZE}\log \mathtt{MAX\_SIZE})$
- `SparseTable(vector)`: $O(\mathtt{MAX\_SIZE}\log \mathtt{MAX\_SIZE}+n\log n)$
- `get`: $O(1)$
- `prod`, `all_prod`: $O(1)$
- `size`, `empty`: $O(1)$

## 空間計算量（API別の追加空間計算量）

- 2種類のconstructor、`size`、`empty`、`get`、`prod`、`all_prod`: $O(1)$

構築済みtableの保存領域は $O(\mathtt{MAX\_SIZE}\log\mathtt{MAX\_SIZE})$ であり、constructorはそのobject内領域へ直接構築する。

## 注意点

- `0 <= n <= MAX_SIZE`。vector長超過を含むconstructor違反は例外。
- `get(k)`は`0 <= k < n`、`prod(l,r)`は`0 <= l <= r <= n`。違反時は例外。
- 空区間の`prod`と空列の`all_prod`は単位元を返す。
- `Semilattice.op`は結合的かつ冪等で、`e`は単位元である必要がある。
