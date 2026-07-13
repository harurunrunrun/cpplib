---
title: Unicyclic Matching
documentation_of: ../src/algorithm/matching/unicyclic_matching.hpp
---

各連結成分が木または $N$ 頂点 $N$ 辺の無向グラフである場合の最大マッチング。

# 関数

```cpp
unicyclic_matching(n, edges)
```

`edges` は `UnicyclicMatchingEdge{from, to}` の列。

`UnicyclicMatchingResult` は次を持つ。

```cpp
int size;
vector<int> match;
```

自己ループは扱わない。多重辺は扱える。

## 時間計算量

- `unicyclic_matching`: $O(N+M)$

各連結成分の検査、木DP・cycle DP、長さ $N$ の `match` 出力を含む。

## 空間計算量

- 戻り値と作業領域を含めて $O(N+M)$
