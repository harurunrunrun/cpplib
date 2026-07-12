---
title: Maximum Independent Set
documentation_of: ../src/algorithm/graph/maximum_independent_set.hpp
---

無向グラフの最大独立集合を1つ求める。

# 関数

```cpp
maximum_independent_set(graph)
```

`graph[v]` に隣接頂点を入れる。無向グラフである必要がある。

戻り値は最大独立集合の頂点列。

## 時間計算量

指数時間。
