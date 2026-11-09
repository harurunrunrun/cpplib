---
title: 3D Point Deduplication by Relative Range Tree (相対範囲木による三次元点重複除去)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/point_set/deduplicate_points_3d_relative_range_tree.hpp
---

## API

- `deduplicate_points_3d_relative_range_tree(points, relative_tolerance, absolute_tolerance = GEOMETRY3D_EPS)`: 相対許容誤差が正の場合に、座標圧縮した三次元の範囲木（三重の Fenwick 木）で既存の代表点の有無を判定し、既定法 `deduplicate_points_3d` と同じ代表点列を返す。

## API 別の時間計算量・空間計算量

点数を $N$ とする。座標圧縮・構築・全クエリを含めて最悪時間 $O(N \log^3 N)$、追加領域 $O(N \log^2 N)$。

## 注意点

- `relative_tolerance` は正の有限値、`absolute_tolerance` は非負の有限値でなければならない。全座標も有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 各座標で一致とみなす範囲の境界は、実際の `relative_equal` の判定に対する単調二分探索で決める。
