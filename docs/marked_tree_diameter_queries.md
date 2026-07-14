---
title: Marked Tree Diameter Queries (マーク頂点直径クエリ) [QTREE4]
documentation_of: ../src/structure/tree/marked_tree_diameter_queries.hpp
---

重み付き木でmark頂点集合を動的に変更し、その直径を返す。

## API

### `MarkedTreeDiameterQueries<Weight, MAX_SIZE>(n)`

空の木を作る。時間計算量は $O(n)$。

### `add_edge(left, right, weight)`

重み付き辺を追加する。時間計算量は償却 $O(1)$。

### `build(initially_marked = true)`

索引を構築し、全頂点の初期mark状態を指定する。時間計算量は $O(n\log n)$。

### `toggle(vertex)`

markを反転する。時間計算量は $O(\log^2 n)$。

### `diameter()`

mark頂点間距離の最大値を返す。markがなければ `nullopt`。時間計算量は $O(1)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、辺集合は重み付き木でなければならない。
- `build` 前の操作、負辺、範囲外頂点では `runtime_error`。
