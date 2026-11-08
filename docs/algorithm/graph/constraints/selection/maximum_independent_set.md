---
title: Maximum Independent Set (最大独立集合) [maximum_independent_set]
documentation_of: ../../../../../src/algorithm/graph/constraints/selection/maximum_independent_set.hpp
---

無向グラフの最大独立集合を 1 つ求める。補グラフを作り、`maximum_clique` で最大クリークを求める。

## 関数

```cpp
std::vector<int> maximum_independent_set(const std::vector<std::vector<int>>& graph)
```

`graph[v]` には頂点 `v` に隣接する頂点を入れる。グラフは無向でなければならない。自己ループは無視する。

戻り値は最大独立集合の頂点を昇順に並べた列である。

## 時間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数とする。

- 隣接行列・補グラフの構築と無向性の検査: $O(N^2 + M)$
- 補グラフ上の最大クリーク探索: 最悪 $O(2^N N^2)$

## 空間計算量

元のグラフと補グラフの隣接表現、探索領域を含めて $O(N^2)$。

## 注意点

- 隣接リストの行き先が $[0, N)$ の範囲外なら `runtime_error` を送出する。
- グラフが無向でない（隣接関係が対称でない）場合も `runtime_error` を送出する。
