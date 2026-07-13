---
title: Best-Bin-First KD-tree
documentation_of: ../src/approximate/nearest_neighbor/best_bin_first_kd_tree.hpp
---

# Best-Bin-First KD-tree

部分木の軸平行包囲箱と問い合わせ点の距離が小さい順に探索する静的KD-tree。調べる点数を制限すると近似最近傍探索、制限しなければ厳密最近傍探索になる。

## `BestBinFirstKdTree(points)` / `reset(points)`

`std::array<T, Dimension>` の点群から平衡木を構築する。同一座標の点も保持する。

- 時間計算量: 平均・実装上の構築 $O(N\log N)$
- 空間計算量: $O(ND)$

## `size()` / `empty()` / `point(index)`

点数、空判定、元の添字に対応する点を返す。

- 時間計算量: $O(1)$

## `nearest(query, max_checks)`

`KdTreeNeighbor { index, squared_distance, checked_points }` を返す。空なら `std::nullopt`。`max_checks` は実際に距離を評価する点数の上限で、正でなければならない。

- 時間計算量: 最悪 $O(\min(N,C)\log N)$、通常は $O(\log N+C\log N)$ 程度（$C$ は `max_checks`）
- 追加空間計算量: 最悪 $O(N)$
- 近似: `max_checks >= N` なら厳密。小さい値では近似比の保証はない。包囲箱下界が現在値を超えた部分木は安全に除く。

非有限座標と距離二乗のoverflowは例外とする。距離が等しい場合は元の添字が小さい点を返す。
