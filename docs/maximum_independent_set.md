---
title: Maximum Independent Set (最大独立集合) [maximum_independent_set]
documentation_of: ../src/algorithm/graph/constraints/maximum_independent_set.hpp
---

無向グラフの最大独立集合を1つ求める。

## 関数

```cpp
maximum_independent_set(graph)
```

`graph[v]` に隣接頂点を入れる。無向グラフである必要がある。

戻り値は最大独立集合の頂点列。

## 時間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数、$I$ を返す独立集合の大きさとする。

- 補グラフ構築: $O(N^2+M)$
- 最大クリーク探索を含む `maximum_independent_set`: worst-case $O(2^N N^3+N^2+M)$、出力構築 $O(I)$

## 空間計算量

- 元・補グラフの隣接表現と探索領域を含めて $O(N^2)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
