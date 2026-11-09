---
title: Bipartite Edge Coloring (二部グラフの辺彩色) [bipartite_edge_coloring]
documentation_of: ../../../../../src/algorithm/graph/constraints/coloring/bipartite_edge_coloring.hpp
---

無向二部多重グラフを、辺彩色数に等しい最小の色数で辺彩色（隣接する辺どうしが異なる色になる彩色）する。Kőnig の辺彩色定理により、必要な色数は最大次数 $\Delta$ に等しい。

## 辺

```cpp
struct BipartiteEdgeColoringEdge {
    int left;
    int right;
};
```

`left` は左側の頂点番号、`right` は右側の頂点番号。入力の異なる要素は、端点が同じでも別の辺として扱う。

## 結果

```cpp
struct BipartiteEdgeColoringResult {
    int color_count;
    vector<int> color;
};
```

- `color_count` は最大次数 $\Delta$。辺がなければ 0。
- `color[i]` は入力の `edges[i]` に対応する色。入力順を保ち、辺が存在すれば $0 \le$ `color[i]` $\lt \Delta$ を満たす。
- 同じ頂点に接続する 2 辺の色は常に異なる。

## 二部グラフの辺彩色

```cpp
BipartiteEdgeColoringResult bipartite_edge_coloring(
    int left_size,
    int right_size,
    const vector<BipartiteEdgeColoringEdge>& edges
)
```

孤立頂点を含んでよく、辺がなければ `left_size == 0` や `right_size == 0` も許す。二部グラフには自己ループという概念はなく、左右で同じ整数番号は別の頂点を表す。多重辺を許す。

次数の和が $\Delta$ 以下になるように同じ側の頂点を縮約し、左右の不足する次数をダミー辺で補って $\Delta$-正則な二部多重グラフを作る。次数が偶数ならオイラー閉路を交互に 2 分割し、奇数なら Hopcroft--Karp 法で完全マッチングを 1 つ取り除く。これを再帰的に繰り返し、ダミー辺の色は結果から捨てる。

## 時間計算量

$M$ を辺数、$\Delta$ を最大次数、$L$ を `left_size`、$R$ を `right_size`、縮約後の片側の頂点数を $S$ とする。$S = O(M / \Delta + 1)$ であり、正則化した後の辺数も $O(M)$ である。

- 頂点の縮約と正則化: $O(L + R + M)$
- オイラー閉路による分割の合計: $O(M \log (\Delta + 1))$
- 完全マッチングの合計: $O(M \sqrt{S} \log (\Delta + 1))$
- 全体: $O(L + R + M \sqrt{M / \Delta + 1} \log (\Delta + 1))$

$M = 0$ の場合は $O(L + R)$。

## 空間計算量

- $O(L + R + M)$

## 注意点

- `left_size`、`right_size` は非負でなければならない。全辺について `0 <= left < left_size` かつ `0 <= right < right_size` を要求する。違反時は `std::runtime_error` を送出する。
- 正則化後の辺数 $S \Delta$ が `int` の最大値を超える場合も `std::runtime_error` を送出する。
