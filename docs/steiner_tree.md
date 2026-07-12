---
title: Steiner Tree
documentation_of: ../src/algorithm/graph/steiner_tree.hpp
---

無向重み付きグラフの最小シュタイナー木。

# 関数

```cpp
steiner_tree<T>(graph, terminals, inf)
```

`graph[v]` は `SteinerTreeEdge<T>{to, cost}` の列。無向グラフでは両方向に辺を入れる。

到達不能な場合は `inf` を返す。

## 時間計算量

端点数を $K$ として、

- $O(3^K N + 2^K M\log N)$
