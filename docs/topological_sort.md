---
title: Topological Sort
documentation_of: ../src/algorithm/graph/topological_sort.hpp
---

有向グラフをトポロジカルソートする。

# 関数

```cpp
topological_sort(graph)
```

`TopologicalSortResult` は次を持つ。

```cpp
vector<int> order;
bool is_dag;
```

閉路がある場合 `is_dag` は `false`。

## 時間計算量

- $O(N+M)$
