---
title: Minimum Weight Bipartite Matching
documentation_of: ../src/algorithm/matching/minimum_weight_bipartite_matching.hpp
---

二部グラフの最小重み最大マッチング。

まずマッチング数を最大化し、その中で重み和を最小化する。

# 関数

```cpp
minimum_weight_bipartite_matching<T>(left_size, right_size, edges, inf)
```

`edges` は `MinimumWeightBipartiteMatchingEdge<T>{left, right, cost}` の列。

`MinimumWeightBipartiteMatchingResult<T>` は次を持つ。

```cpp
int size;
T cost;
vector<int> left_match;
vector<int> right_match;
```

## 時間計算量

最短増加路法。

- $O(FVE)$
