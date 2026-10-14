---
title: Range-Parallel Component Product Sum (区間並列併合の成分重み積和) [range_parallel_unionfind]
documentation_of: ../../../../src/structure/dsu/range/range_parallel_component_product_sum.hpp
---

各Union-Find成分に頂点重みの総和を持ち、異なる2成分の併合ごとに両成分の
重み和の積を累積する区間並列Union-Find。

## テンプレート引数・コンストラクタ

```cpp
RangeParallelComponentProductSum<MAX_SIZE, T> dsu(vertex_weights)
```

`vertex_weights.size()` 個のsingleton成分を作る。`T` はcopy、`+=`、`*`を
提供する必要がある。

## 時間計算量

構築は $O(MAX\_SIZE\log MAX\_SIZE)$、本体の固定保存領域も
$O(MAX\_SIZE\log MAX\_SIZE)$。

## `merge_ranges`

```cpp
T merge_ranges(int l1, int r1, int l2, int r2)
```

同じ長さの半開区間を位置ごとに併合し、これまでに異なる成分を併合した全時点の
成分重み和の積の累積値を返す。

## 時間計算量

$Q$ 回の呼び出し全体で
$O((N\log N+Q)\alpha(N))$。1回では新しく展開した補助区間数に比例する。

## `product_sum`

```cpp
T product_sum() const
```

現在の累積値を返す。時間計算量 $O(1)$。

## `component_sum` / `same`

```cpp
T component_sum(int vertex)
bool same(int left, int right)
```

頂点が属する成分の重み和、2頂点が同じ成分かを返す。
時間計算量はamortized $O(\alpha(N))$。

## `size` / `groups`

```cpp
int size() const
int groups() const
```

頂点数と成分数を返す。時間計算量 $O(1)$。

## 注意点

`vertex_weights.size() <= MAX_SIZE` かつ頂点数を `int` で表せる必要がある。
構築時の違反は `length_error`。頂点・区間は範囲内で、2区間の長さは等しく
なければならず、違反時は `runtime_error`。`T` のoverflowと例外は型自身の
契約に従う。

## 可変長版

```cpp
using Dynamic = RangeParallelComponentProductSum<>;
```

容量の template 引数を省略すると、実行時の要素数・更新数に合わせて `std::vector` を確保する。頂点重みと各層の配列を実行時の n に合わせる。補助領域 O(n log(n + 1))、併合の計算量は RangeParallelUnionFind に従う。

容量を明示した固定長版も同じヘッダ・このページの code に含まれる。上記の固定上限に関する説明は容量を明示した場合に適用される。

型を指定した重みでも CTAD により容量を省略できる。重みの型は `weights` から推論する。

```cpp
RangeParallelComponentProductSum components(weights);
```
