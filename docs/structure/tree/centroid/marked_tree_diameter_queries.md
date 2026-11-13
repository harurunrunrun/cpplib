---
title: Marked Tree Diameter Queries (マーク頂点直径クエリ) [QTREE4]
documentation_of: ../../../../src/structure/tree/centroid/marked_tree_diameter_queries.hpp
---

重み付き木でマークされた頂点の集合を動的に変更し、マークされた頂点間の最大距離（直径）を返す。
`DynamicMarkedTreeDiameter` の薄いラッパーである。

## API

### `MarkedTreeDiameterQueries<Weight, MAX_SIZE>(n)`

頂点数 `n` の辺のない木を作る。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。
時間計算量は固定長版で $O(\texttt{MAX_SIZE}\log \texttt{MAX_SIZE})$、可変長版で $O(n)$ である。

### `add_edge(left, right, weight)`

重み付きの無向辺を追加する。時間計算量は $O(1)$ である。

### `build(initially_marked = true)`

索引を構築し、全頂点の初期のマーク状態を指定する。時間計算量は `initially_marked` が `false` なら $O(n\log n)$、`true` なら $O(n\log^2 n)$ である。

### `toggle(vertex)`

マークを反転し、操作後にマークされていれば `true` を返す。時間計算量は $O(\log^2 n)$ である。

### `diameter()`

マークされた頂点間の距離の最大値を返す。同じ頂点を両端に選べるため、マークが 1 個以上あれば 0 以上である。マークがなければ `nullopt` を返す。時間計算量は $O(1)$ である。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、辺集合は重み付きの木でなければならない。辺の重みは負でもよい。
- `build` 前の操作、範囲外の頂点、木でない入力では `runtime_error` を送出する。
- 距離が `Weight` で表現できなければ `overflow_error` を送出する。
- 容量を指定した固定配列版（`MarkedTreeDiameterQueries<Weight, N>`）と可変長版（`MarkedTreeDiameterQueries<Weight>`）は同じヘッダで利用できる。
