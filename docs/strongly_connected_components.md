---
title: Strongly Connected Components
documentation_of: ../src/algorithm/graph/strongly_connected_components.hpp
---

有向グラフの強連結成分分解。

# 関数

```cpp
strongly_connected_components(graph)
```

`StronglyConnectedComponentsResult` は次を持つ。

```cpp
int count;
vector<int> id;
vector<vector<int>> groups;
```

## 時間計算量

- $O(N+M)$
