---
title: Weighted Tree Distance and K-th Vertex (重み付き木の距離・K番目頂点) [QTREE2]
documentation_of: ../src/algorithm/tree/weighted_tree_distance_kth_vertex.hpp
---

辺に重みを持つ静的な木に対し、パスの重み和とパス上の頂点を求める。
二分累積を使うため、クエリは根の選び方に依存しない。

## API

### `WeightedTreeDistanceKthVertex<Weight, MAX_SIZE>(vertex_count)`

頂点数 `vertex_count` の空の構造を作る。
頂点番号は `[0, vertex_count)`。

### `size()`

頂点数を返す。

### `add_edge(left, right, weight)`

重み `weight` の無向辺を追加する。
`build` より前に呼ぶ。

### `build(root = 0)`

追加済みの辺が成す木を `root` を根として前処理する。

### `distance(left, right)`

`left` から `right` への単純パスに含まれる辺重みの総和を返す。
同じ頂点同士では加法単位元を返す。

### `kth_vertex(left, right, order)`

`left` を0番目として、`left` から `right` への単純パス上の
`order` 番目の頂点を返す。端点も数える。

## 時間計算量

`N = vertex_count` とする。

- 構築: $O(N \log N)$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `build`: $O(N \log N)$
- `distance`: $O(\log N)$
- `kth_vertex`: $O(\log N)$

使用メモリは $O(N \log N)$。

## 注意点

- `MAX_SIZE > 0` かつ `0 <= vertex_count <= MAX_SIZE` が必要。
- `Weight` は `AddMonoid<Weight>` で扱え、重み和が `Weight` に収まる必要がある。
- `vertex_count > 0` では、辺数が `vertex_count - 1` の連結な無向木を渡す。
- `order` は `[0, パスの辺数]` でなければならない。
- 頂点・`order` の範囲違反、木でない入力、構築前のクエリ、
  構築後の辺追加では `runtime_error` を送出する。
