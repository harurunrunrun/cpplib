---
title: Rerooting DP
documentation_of: ../src/algorithm/tree/rerooting_dp.hpp
---

全方位木 DP。

隣接する部分木からの寄与を monoid でまとめ、頂点を足す処理と辺を渡る処理を与える。

## 使い方

- `RerootingDP<Monoid, AddVertex, AddEdge> dp(n, add_vertex, add_edge)`
- `dp.add_edge(u, v)`
- `dp.solve(root = 0)`

`add_vertex(S aggregate, int v) -> S` は、隣接部分木からの寄与をまとめた値に頂点 `v` を足す。

`add_edge(S value, int from, int to, int edge_id) -> S` は、`from` 側の値を辺 `edge_id` 越しに `to` への寄与へ変換する。

## 計算量

- $O(n)$ 回の monoid 演算と変換
