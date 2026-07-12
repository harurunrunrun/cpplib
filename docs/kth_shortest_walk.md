---
title: K-th Shortest Walk
documentation_of: ../src/algorithm/graph/kth_shortest_walk.hpp
---

非負重み有向グラフで、閉路を許す walk の距離を小さい順に高々 `k` 個求める。

# 関数

```cpp
kth_shortest_walks<T>(graph, source, target, k)
```

`graph[v]` は `KthShortestWalkEdge<T>{to, cost}` の列。

戻り値は `source` から `target` への walk の距離列。`source == target` のときは空 walk の距離 `0` も含む。

walk の距離と辺コストの加算結果は `T` で表現できる必要がある。負辺を含む場合は例外を送出する。

## 時間計算量

- $O(KM\log(KN))$
