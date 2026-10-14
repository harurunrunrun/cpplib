---
title: Minimum Diameter Spanning Tree (最小直径全域木) [minimum_diameter_spanning_tree]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/minimum_diameter_spanning_tree.hpp
---

非負整数重みの無向グラフで、木上の距離の最大値が最小になる全域木を復元する。自己ループと多重辺を扱える。

## API

`minimum_diameter_spanning_tree(n, edges)` に `DiameterSpanningTreeEdge{from, to, weight}` の配列を渡す。戻り値の `exists` は連結性、`diameter` は最適直径、`edge_indices` は元の配列の採用辺番号を表す。

中心が頂点なら `center_vertex` に番号を返し `center_edge == -1`。辺の内部なら `center_edge` に元の辺番号を返し、`center_offset_twice` はその辺の `from` 側からの中心までの距離の2倍を返す。中心は半整数の位置でも正確に表現する。

全点対最短路と各頂点からの距離順序を作り、各辺について中心候補の包絡線を走査する。最適中心からの最短路木を構成し、辺内中心の場合は中心辺で2本の木を接続する。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。$O(N(N+M)\log(N+M)+N^2\log N+NM)$。

## 空間計算量

$O(N^2+M)$。

## 注意点

`n >= 1`、端点が頂点範囲内、辺重みが非負であることが必要。重みの和は `LLONG_MAX / 8` 未満とする。入力条件に違反すると例外を送出する。非連結なら `exists == false` を返す。

中心の探索は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/graph/minimum_diameter_spanning_tree/sol/correct.cpp) を基にした。Apache-2.0 の全文はヘッダ末尾に記載している。
