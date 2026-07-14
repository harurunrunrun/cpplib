---
title: Rerooting DP (全方位木DP)
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

## 時間計算量

- `RerootingDP(n, add_vertex, add_edge)`: $O(N)$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `solve`: $O(N)$ 回のmonoid演算、`add_vertex`、`add_edge` 呼出しと、長さ $N$ の出力構築

上記はmonoid演算と各callbackを $O(1)$ とした計算量。callbackが重い場合はその呼出しコストを掛ける。

## 空間計算量

- 保存graph、前後prefix、戻り値を含めて $O(N)$

## 注意点

`add_edge(u,v)` は0-indexedの辺idを返す。`solve(root)[v]` は `v` を根とした木全体のDP値。callbackの型は前節の `S` に従う。非連結木・閉路・辺数不一致で `runtime_error`。

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
