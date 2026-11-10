---
title: Maximum Clique (最大クリーク) [ABC002_4]
documentation_of: ../../../../../src/algorithm/graph/constraints/selection/maximum_clique.hpp
---

無向グラフの最大クリークを 1 つ求める。貪欲彩色による上界で枝刈りする分枝限定法を用いる。

## 関数

```cpp
std::vector<int> maximum_clique(const std::vector<std::vector<int>>& graph)
```

`graph[v]` には頂点 `v` に隣接する頂点を入れる。グラフは無向でなければならない（`u` が `graph[v]` に含まれるなら `v` も `graph[u]` に含まれる）。自己ループは無視し、重複した要素は 1 本の辺として扱う。

戻り値は最大クリークの頂点を昇順に並べた列である。

## 時間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数とする。

- 隣接行列の構築と無向性の検査: $O(N^2 + M)$
- 探索: 最悪 $O(2^N N^2)$

分枝限定法により実際の探索量は入力に依存するが、最悪計算量は指数時間である。

## 空間計算量

隣接行列と再帰中の候補列を含めて $O(N^2)$。

## 注意点

- 隣接リストの行き先が $[0, N)$ の範囲外なら `runtime_error` を送出する。
- グラフが無向でない（隣接関係が対称でない）場合も `runtime_error` を送出する。
