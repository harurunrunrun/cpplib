---
title: Bipartite Edge Coloring
documentation_of: ../src/algorithm/graph/bipartite_edge_coloring.hpp
---

無向二部多重グラフを、辺彩色数と等しい最小色数でproper edge coloringする。Kőnigのline coloring theoremにより、必要な色数は最大次数 `Delta` である。

# 辺

```cpp
struct BipartiteEdgeColoringEdge {
    int left;
    int right;
};
```

`left` は左側頂点番号、`right` は右側頂点番号。異なる入力要素は、端点が同じでも別の辺として扱う。

# 結果

```cpp
struct BipartiteEdgeColoringResult {
    int color_count;
    vector<int> color;
};
```

- `color_count` は最大次数 `Delta`。辺が空なら0。
- `color[i]` は入力の `edges[i]` に対応する色。入力順を保ち、辺が存在すれば `0 <= color[i] < color_count`。
- 同じ頂点に接続する2辺の色は常に異なる。

# 二部グラフの辺彩色

```cpp
BipartiteEdgeColoringResult bipartite_edge_coloring(
    int left_size,
    int right_size,
    const vector<BipartiteEdgeColoringEdge>& edges
)
```

孤立頂点を含めてよく、`left_size == 0` または `right_size == 0` も辺が空なら許す。自己loopという概念は二部グラフにはなく、左右の同じ整数番号は別の頂点である。多重辺を許す。

次数の和が `Delta` 以下になるよう同じ側の頂点を縮約し、左右の不足次数をdummy edgeで補って `Delta`-regular bipartite multigraphを作る。偶数次数はEuler circuitを交互に分割し、奇数次数はHopcroft--Karpでperfect matchingを1つ取り除く。dummy edgeの色は結果から捨てる。

## 時間計算量

`M = edges.size()`、`Delta` を最大次数、縮約後の片側頂点数を `S` とする。`S = O(M / Delta + 1)` であり、regularization後の辺数も `O(M)`。

- 頂点縮約とregularization: `O(left_size + right_size + M)`
- Euler分割全体: `O(M log(Delta + 1))`
- perfect matching全体: `O(M sqrt(S) log(Delta + 1))`
- API全体: `O(left_size + right_size + M sqrt(M / Delta + 1) log(Delta + 1))`

`M == 0` の場合は `O(left_size + right_size)`。

## 空間計算量

- `O(left_size + right_size + M)`

# 前提・例外

`left_size`、`right_size` は非負でなければならない。全辺について `0 <= left < left_size` および `0 <= right < right_size` を要求する。違反時は `runtime_error` を送出する。
