---
title: Matroid Union (マトロイド和)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/matroid_union.hpp
---

1つのマトロイドの独立集合 `copy_count` 個の和として表せる最大部分集合と、その分割を求める。

## `matroid_union(ground_set_size, copy_count, independent)`

同じoracleを複製して `matroid_partition` を呼ぶ。返却型は `MatroidPartitionResult`。$k=copy_count$ とした時間・空間計算量は `matroid_partition` と同じ。

## 注意点

同一要素は高々1 groupに入る。`copy_count == 0` では全要素が `uncovered`。

## 時間計算量

The bound is stated in the API section above.
