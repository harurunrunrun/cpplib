---
title: Bipartite Edge Coloring
documentation_of: ../src/algorithm/graph/bipartite_edge_coloring.hpp
---

二部グラフの辺彩色。多重辺を扱える。

最大次数を $\Delta$ として、$\Delta$ 色で塗る。

# 関数

```cpp
bipartite_edge_coloring(left_size, right_size, edges)
```

`edges` は `BipartiteEdgeColoringEdge{left, right}` の列。

`BipartiteEdgeColoringResult` は次を持つ。

```cpp
int color_count;
vector<int> color;
```

各辺の色は `0 <= color[i] < color_count`。

## 時間計算量

- $O(M^2)$

色の入れ替えは2色からなる交互パスだけを直接たどり、孤立頂点数には依存しない。
端点は辺に現れる頂点だけを座標圧縮し、色表のメモリは $O(M\Delta)$。
