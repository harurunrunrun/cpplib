---
title: Tree Path K-th Value (木上パスK番目値) [COT]
documentation_of: ../src/algorithm/tree/tree_path_kth_value.hpp
---

静的な頂点値付き木のパス上でk番目に小さい値を返す。

## API

### `TreePathKthValue<Value, MAX_SIZE>(n)`

空の木を作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は $O(1)$。

### `build(values, root = 0)`

頂点値と永続順序統計索引を構築する。時間計算量は $O(n\log n)$。

### `kth_smallest(left, right, order)`

パス上の0-indexed `order` 番目に小さい値を返す。時間計算量は $O(\log n)$。

## 注意点

- `values.size()==n<=MAX_SIZE` で、辺集合は連結な木でなければならない。
- `build` 前、範囲外頂点、パス頂点数以上の `order` では `runtime_error`。
