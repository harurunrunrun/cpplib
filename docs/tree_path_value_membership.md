---
title: Tree Path Value Membership (木上パス値存在判定) [GOT]
documentation_of: ../src/algorithm/tree/tree_path_value_membership.hpp
---

静的な頂点値付き木で、パス上に指定値が存在するかを返す。

## API

### `TreePathValueMembership<Value, MAX_SIZE>(n)`

空の木を作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は $O(1)$。

### `build(values, root = 0)`

頂点値と順序統計索引を構築する。時間計算量は $O(n\log n)$。

### `contains(left, right, value)`

両端を含むパスに `value` があれば `true`。時間計算量は $O(\log n)$。

## 注意点

- `values.size()==n<=MAX_SIZE` で、辺集合は連結な木でなければならない。
- `build` 前や範囲外頂点では `runtime_error`。
