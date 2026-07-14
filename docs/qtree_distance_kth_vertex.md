---
title: QTree Distance and K-th Vertex (木上距離・K番目頂点) [QTREE2]
documentation_of: ../src/algorithm/tree/qtree_distance_kth_vertex.hpp
---

重み付き静的木の距離と、指定方向のパス上頂点を返す。

## API

### `QTreeDistanceKthVertex<Weight, MAX_SIZE>(n)`

空の構造を作る。時間計算量は $O(n\log n)$。

### `size()`

頂点数を返す。時間計算量は $O(1)$。

### `add_edge(left, right, weight)`

重み付き無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root = 0)`

二分lifting索引を構築する。時間計算量は $O(n\log n)$。

### `distance(left, right)`

パス重み和を返す。時間計算量は $O(\log n)$。

### `kth_vertex(left, right, order)`

`left` から始まる0-indexed `order` 番目の頂点を返す。時間計算量は $O(\log n)$。

## 注意点

- 辺集合は連結な木で、頂点数は `MAX_SIZE` 以下でなければならない。
- `Weight` の加算は結合的で0を単位元とし、結果は型の範囲に収まる必要がある。
- `build` 前やパス長以上の `order` では `runtime_error`。
