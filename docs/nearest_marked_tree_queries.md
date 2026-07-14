---
title: Nearest Marked Tree Queries (動的最近傍マーク頂点) [QTREE5]
documentation_of: ../src/structure/tree/nearest_marked_tree_queries.hpp
---

重み付き木上のmark集合を反転し、指定頂点から最近いmarkまでの距離を返す。

## API

### `NearestMarkedTreeQueries<Weight, MAX_SIZE>(n)`

全頂点未markの空の木を作る。時間計算量は $O(n)$。

### `add_edge(left, right, weight = Weight{1})`

重み付き辺を追加する。時間計算量は償却 $O(1)$。

### `build()`

centroid分解索引を構築する。時間計算量は $O(n\log n)$。

### `toggle(vertex)`

markを反転する。時間計算量は $O(\log^2 n)$。

### `nearest_distance(vertex)`

最近のmarkまでの距離を返し、markがなければ `nullopt`。時間計算量は $O(\log^2 n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下、辺集合は非負重みの木でなければならない。
- `build` 前の操作や範囲外頂点では `runtime_error`。
