---
title: Cycle Detection
documentation_of: ../src/algorithm/graph/cycle_detection.hpp
---

閉路検出。

# 関数

```cpp
directed_cycle(graph)
undirected_cycle(n, edges)
```

`directed_cycle` は有向グラフの閉路を1つ返す。

`undirected_cycle` は無向グラフの閉路を1つ返す。多重辺を扱える。

閉路がない場合は空配列を返す。

## 時間計算量

- $O(N+M)$
