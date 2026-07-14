---
title: Subtree Label Order Statistics (部分木ラベル順序統計) [PT07J]
documentation_of: ../src/algorithm/tree/subtree_label_order_statistics.hpp
---

静的な根付き木の各部分木について、ラベルと頂点番号の辞書順で頂点を選ぶ。

## API

### `SubtreeLabelOrderStatistics<MAX_SIZE>(vertex_labels)`

頂点ラベルを保持する。時間計算量は $O(n)$。

### `size()`

頂点数を返す。時間計算量は $O(1)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root = 0)`

Euler順序と順序統計索引を構築する。時間計算量は $O(n\log n)$。

### `kth_smallest_vertex(subtree_root, order)`

部分木内の0-indexed `order` 番目の頂点番号を返す。時間計算量は $O(\log n)$。

## 注意点

- 木は連結で、頂点数は `MAX_SIZE` 以下でなければならない。
- 同じラベルでは頂点番号が小さいものを先に数える。
- `build` 前または部分木サイズ以上の `order` では `runtime_error`。
