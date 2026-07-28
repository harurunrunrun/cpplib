---
title: 3D Point Deduplication by Relative Range Tree (相対範囲木による三次元点重複除去)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/point_set/deduplicate_points_3d_relative_range_tree.hpp
---

## API

- `deduplicate_points_3d_relative_range_tree(points, relative_tolerance, absolute_tolerance)`: 相対許容誤差が正の場合に、座標圧縮した三次元範囲木で既存代表点の有無を判定し、既定法と同じ代表点列を返す。

## API別の時間計算量・空間計算量

点数を $N$ として座標圧縮・構築・全queryを含め最悪時間 $O(N\log^3 N)$、追加領域 $O(N\log^2 N)$。

## 注意点

`relative_tolerance` は正の有限値、`absolute_tolerance` は非負の有限値でなければならない。境界は実際の `relative_equal` を単調二分探索して決める。
