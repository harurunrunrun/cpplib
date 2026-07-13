---
title: Minimum Weight Unicyclic Matching
documentation_of: ../src/algorithm/matching/minimum_weight_unicyclic_matching.hpp
---

各連結成分が木または $N$ 頂点 $N$ 辺の無向グラフである場合の最小重み最大マッチング。

まずマッチング数を最大化し、その中で重み和を最小化する。

# 関数

```cpp
minimum_weight_unicyclic_matching<T>(n, edges)
```

`edges` は `MinimumWeightUnicyclicMatchingEdge<T>{from, to, cost}` の列。

`MinimumWeightUnicyclicMatchingResult<T>` は次を持つ。

```cpp
int size;
T cost;
vector<int> match;
```

自己ループは扱わない。多重辺は扱える。

## 時間計算量

- `minimum_weight_unicyclic_matching`: $O(N+M)$

各連結成分の検査、木DP・cycle DP、長さ $N$ の `match` 出力を含む。

## 空間計算量

- 戻り値と作業領域を含めて $O(N+M)$
