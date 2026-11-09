---
title: Weighted Blossom Internals (重み付きBlossom法の内部実装)
documentation_of: ../../../../src/algorithm/matching/general/minimum_weight_general_matching_internal.hpp
---

一般グラフの重み付きマッチングに用いる、主双対法による blossom 法の内部実装である。

## API

- `minimum_weight_general_matching_internal::Wide`: 内部の重みを保持する `__int128_t`。
- `WeightedEdge{from, to, weight}`: 内部の辺型。
- `WeightedBlossom(vertex_count, edges)`: blossom 探索の状態を構築する。
- `WeightedBlossom::solve(maximize_cardinality = true)`: 重みの和が最大のマッチングを求め、各頂点の相手（未マッチなら `-1`）を表す列を返す。`maximize_cardinality` が `true` なら、辺数が最大のマッチングの中で重みを最大化する。

## 時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

- 構築: 時間・領域 $O(N + M)$。
- `solve`: 時間 $O(N^3 + M)$、保持する領域と追加領域は $O(N^2 + M)$。

## 注意点

上位の `minimum_weight_general_matching` と `maximum_weight_general_matching` 専用の内部 API であり、互換性は保証しない。端点は $[0, N)$ に含まれ、内部の辺の重みと双対変数の値は `Wide` に収まる必要がある。
