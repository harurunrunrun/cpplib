---
title: Minimum Diameter Spanning Tree (最小直径全域木) [minimum_diameter_spanning_tree]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/minimum_diameter_spanning_tree.hpp
---

非負整数重みの無向グラフで、木の上での距離の最大値（直径）が最小になる全域木を求める。自己ループと多重辺を扱える。

## API

`minimum_diameter_spanning_tree(n, edges)` に `DiameterSpanningTreeEdge{from, to, weight}` の配列を渡す。戻り値のメンバ `exists` はグラフが連結かどうか、`diameter` は最適な直径、`edge_indices` は採用した辺の元の配列での添字を表す。

中心が頂点上にあるなら `center_vertex` にその番号を返し、`center_edge == -1` とする。辺の内部にあるなら `center_edge` に元の辺の添字を返し、`center_offset_twice` にその辺の `from` 側の端点から中心までの距離の 2 倍を返す。これにより、中心が半整数の位置にあっても正確に表現できる。

全点対最短路と各頂点からの距離の順序を求め、各辺について中心の候補の包絡線を走査する。最適な中心からの最短路木を構成し、中心が辺の内部にある場合は、中心の辺で 2 つの木を接続する。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。$O(N(N + M)\log(N + M) + N^2 \log N + NM)$。

## 空間計算量

$O(N^2 + M)$

## 注意点

- $n \ge 1$ で、辺の端点は $[0, n)$ の範囲内、辺の重みは非負でなければならない。違反時は `invalid_argument` を送出する。
- 重みの総和は `LLONG_MAX / 8` 未満でなければならない。超える場合は `overflow_error` を送出する。
- 非連結なら `exists == false` を返す。

中心の探索は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/graph/minimum_diameter_spanning_tree/sol/correct.cpp) を基にした。Apache-2.0 ライセンスの全文はヘッダ末尾に記載している。
