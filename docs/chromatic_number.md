---
title: Chromatic Number
documentation_of: ../src/algorithm/graph/chromatic_number.hpp
---

無向グラフの彩色数を求める。

# 関数

```cpp
chromatic_number(graph)
```

`graph[v]` に隣接頂点を入れる。無向グラフである必要がある。

実装上 `N <= 24`。

## 時間計算量

指数時間。
