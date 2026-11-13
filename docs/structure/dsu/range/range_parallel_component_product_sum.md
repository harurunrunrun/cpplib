---
title: Range-Parallel Component Product Sum (区間並列併合の成分重み積和) [range_parallel_unionfind]
documentation_of: ../../../../src/structure/dsu/range/range_parallel_component_product_sum.hpp
---

各 Union-Find 成分に頂点重みの総和を持ち、異なる 2 成分を併合するたびに両成分の重みの和の積を累積する、区間並列 Union-Find。内部では `RangeParallelUnionFind` を使う。

## テンプレート引数・コンストラクタ

```cpp
RangeParallelComponentProductSum<MAX_SIZE = -1, T = long long> dsu(vertex_weights)
RangeParallelComponentProductSum dsu(weights)  // CTAD: 可変長版、T は weights から推論
```

`vertex_weights.size()` 個の単独の成分を作る。`T` はコピー、`+=`、`*` を提供する必要がある。
`MAX_SIZE` を省略（`-1`）すると、実行時の頂点数に合わせて領域を確保する可変長版になる。

## merge_ranges

```cpp
T merge_ranges(int l1, int r1, int l2, int r2)
```

同じ長さの半開区間 $[l_1, r_1)$ と $[l_2, r_2)$ を位置ごとに併合し、これまでに異なる成分を併合したすべての時点での「両成分の重みの和の積」の累積値を返す。

## product_sum

```cpp
T product_sum() const
```

現在の累積値を返す。

## component_sum / same

```cpp
T component_sum(int vertex)
bool same(int left, int right)
```

頂点が属する成分の重みの和と、2 頂点が同じ成分に属するかを返す。

## size / groups

```cpp
int size() const
int groups() const
```

頂点数と成分数を返す。

## 時間計算量

$N$ を頂点数とする。

- コンストラクタ: $O(N\log N)$（固定長版では $O(\texttt{MAX_SIZE}\log \texttt{MAX_SIZE})$）
- `merge_ranges`: $Q$ 回の呼び出し全体で $O((N\log N+Q)\alpha(N))$。1 回では新たに展開した補助区間の数に比例する
- `product_sum`、`size`、`groups`: $O(1)$
- `component_sum`、`same`: 償却 $O(\alpha(N))$

## 空間計算量

$O(N \log N)$。固定長版では $O(\texttt{MAX_SIZE}\log \texttt{MAX_SIZE})$ の領域を持つ。

## 注意点

- `vertex_weights.size()` $\le$ `MAX_SIZE` で、かつ頂点数を `int` で表せる必要がある。構築時に違反すると `std::length_error` を送出する。
- 頂点・区間は範囲内で、2 区間の長さは等しくなければならない。違反すると `std::runtime_error` を送出する。
- `T` のオーバーフローや例外は型自身の仕様に従う。
