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

- `topological_sort`: $O(N+M)$

全辺の入次数計算と走査、最大 $N$ 頂点の `order` 出力を含む。

## 空間計算量

- 戻り値、入次数、queueを含めて $O(N)$
