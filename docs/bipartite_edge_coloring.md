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

$M$ を辺数、$\Delta$ を最大次数とする。

- 端点の座標圧縮: $O(M\log(M+1))$
- 交互パスによる彩色と長さ $M$ の出力構築: $O(M^2)$
- `bipartite_edge_coloring` 全体: $O(M^2+M\log(M+1))$

色の入れ替えは2色からなる交互パスだけを直接たどり、孤立頂点数には依存しない。

## 空間計算量

- 圧縮後の色表と戻り値を含めて $O(M\Delta+M)$

## API契約・前提・例外

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
