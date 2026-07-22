---
title: Weighted Blossom Internals (重み付きBlossom法の内部実装)
documentation_of: ../src/algorithm/matching/general/minimum_weight_general_matching_internal.hpp
---

一般グラフの重み付き最大matchingに用いる内部primal-dual実装。

## API

- `minimum_weight_general_matching_internal::Wide`: 内部重みを保持する `__int128`。
- `WeightedEdge{from, to, weight}`: 内部辺型。
- `WeightedBlossom(vertex_count, edges)`: blossom探索状態を構築する。
- `WeightedBlossom::solve()`: 各頂点の相手、未matchingなら-1を表す列を返す。

## API別の時間計算量・空間計算量

- 構築: 頂点数 $N$、辺数 $M$ として時間・領域 $O(N+M)$。
- `solve`: 時間 $O(N^3+M)$、保持・追加領域 $O(N^2+M)$。

## 注意点

上位の `minimum_weight_general_matching` 専用の内部APIで、互換性を保証しない。端点は $[0,N)$、内部辺重みと双対値は `Wide` に収まる必要がある。
