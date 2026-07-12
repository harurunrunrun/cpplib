---
title: General Matching
documentation_of: ../src/algorithm/matching/general_matching.hpp
---

一般グラフの最大マッチング。

# コンストラクタ

```cpp
GeneralMatching graph(n)
```

# 関数

```cpp
graph.add_edge(u, v)
graph.solve()
```

`GeneralMatchingResult` は次を持つ。

```cpp
int size;
vector<int> match;
```

## 時間計算量

- $O(N^3)$
