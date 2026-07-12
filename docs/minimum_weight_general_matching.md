---
title: Minimum Weight General Matching
documentation_of: ../src/algorithm/matching/minimum_weight_general_matching.hpp
---

一般グラフの最小重み最大マッチング。

まずマッチング数を最大化し、その中で重み和を最小化する。

# 関数

```cpp
minimum_weight_general_matching<T>(n, edges, inf)
```

`edges` は `MinimumWeightGeneralMatchingEdge<T>{from, to, cost}` の列。多重辺がある場合は最小コストの辺を使う。

`MinimumWeightGeneralMatchingResult<T>` は次を持つ。

```cpp
int size;
T cost;
vector<int> match;
```

実装上 `N <= 22`。

## 時間計算量

- $O(2^N N^2)$
