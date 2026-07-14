---
title: QTree Edge Maximum (動的木上辺最大値) [QTREE]
documentation_of: ../src/structure/tree/qtree_edge_maximum.hpp
---

静的な木の辺値を点更新し、パス上の最大辺値を返す。

## API

### `QTreeEdgeMaximum<Weight, MAX_SIZE>(n)`

頂点数 `n` の空の構造を作る。時間計算量は $O(n)$。

### `size()`

頂点数を返す。時間計算量は $O(1)$。

### `add_edge(left, right, weight)`

辺を追加し、0-indexed辺番号を返す。時間計算量は償却 $O(1)$。

### `build(root = 0)`

HL分解と辺値索引を構築する。時間計算量は $O(n\log n)$。

### `change_edge(edge, weight)`

指定辺の値を代入する。時間計算量は $O(\log n)$。

### `path_maximum(left, right)`

パス上の最大辺値を返す。時間計算量は $O(\log^2 n)$。同じ頂点では `Weight{}` を返す。

## 注意点

- 頂点数は `MAX_SIZE` 以下、追加辺は連結な木でなければならない。
- `build` 前の更新・queryと範囲外番号では `runtime_error`。
- `Weight` は `MaxMonoid` で比較できる必要がある。
