---
title: Sparse Table (スパーステーブル) [staticrmq]
documentation_of: ../../../../src/structure/range_query/aggregation/sparse_table.hpp
---

静的な列に対して、冪等な演算の区間積を求める。

演算は結合的かつ冪等である必要がある。最小値、最大値、gcd などに使える。

## 使い方

```cpp
constexpr MinMonoid<int> min_monoid{};
SparseTable<min_monoid, 200000> st(a);
int x = st.prod(l, r);
```

```cpp
template<auto Semilattice, int MAX_SIZE = -1>
struct SparseTable;
```

`MAX_SIZE` は列の長さの上限である。省略時（`-1`）は上限を設けず、実行時の長さに合わせて `std::vector` で領域を確保する。

## メンバ関数

- `explicit SparseTable(int n = MAX_SIZE)`
    - 長さ `n` の列を単位元で初期化する（`MAX_SIZE` 省略時のデフォルトは `n = 0`）。
- `explicit SparseTable(const vector<S>& v)`
    - `v` で初期化する。
- `int size() const`
    - 列の長さを返す。
- `bool empty() const`
    - 長さが `0` かどうかを返す。
- `S get(int k) const`
    - `a[k]` を返す。
- `S prod(int l, int r) const`
    - 区間 `[l, r)` の演算結果を返す。
- `S all_prod() const`
    - 区間 `[0, n)` の演算結果を返す。

## 時間計算量

- `SparseTable(int)`: $O(\texttt{MAX_SIZE}\log \texttt{MAX_SIZE})$
- `SparseTable(vector)`: $O(\texttt{MAX_SIZE}\log \texttt{MAX_SIZE}+n\log n)$
- `get`: $O(1)$
- `prod`, `all_prod`: $O(1)$
- `size`, `empty`: $O(1)$

`MAX_SIZE` を省略した場合、構築は $O(n\log(n+1))$ である。

## 空間計算量（API 別の追加空間計算量）

- 2 種類のコンストラクタ、`size`、`empty`、`get`、`prod`、`all_prod`: $O(1)$

構築済みのテーブルの保存領域は $O(\texttt{MAX_SIZE}\log\texttt{MAX_SIZE})$（省略時は $O(n\log(n+1))$）であり、コンストラクタはオブジェクト内の領域へ直接構築する。

## 注意点

- `0 <= n <= MAX_SIZE` を要求する。`vector` の長さが上限を超える場合を含め、コンストラクタの引数が不正なら `std::runtime_error` を送出する。
- `get(k)` は `0 <= k < n`、`prod(l, r)` は `0 <= l <= r <= n` を要求し、違反時は `std::runtime_error` を送出する。
- 空区間の `prod` と空列の `all_prod` は単位元を返す。
- `Semilattice.op` は結合的かつ冪等で、`e` は単位元である必要がある。
