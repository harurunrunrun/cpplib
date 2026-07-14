---
title: LCA Subtree Distinct Colors (LCA部分木色数) [ADAORANG]
documentation_of: ../src/algorithm/tree/lca_subtree_distinct_colors.hpp
---

2頂点のLCAを根とする部分木に現れる異なる色数を返す静的構造である。

## API

### `LcaSubtreeDistinctColors<Color, MAX_SIZE>(vertex_colors)`

頂点色を保持する。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root)`

LCA・Euler順序・区間異色数索引を構築する。時間計算量は $O(n\log n)$。

### `count_at_lca_subtree(left, right)`

両頂点のLCAの部分木にある異なる色数を返す。時間計算量は $O(\log n)$。

## 注意点

- 辺集合は連結な木で、頂点数は `MAX_SIZE` 以下でなければならない。
- `build` 前のqueryや範囲外頂点では `runtime_error`。
