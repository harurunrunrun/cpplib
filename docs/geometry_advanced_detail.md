---
title: Advanced Geometry Internals (高度幾何の内部関数)
documentation_of: ../src/algorithm/geometry/2d/detail/advanced_convex_geometry_detail.hpp
---

高度凸幾何で用いる誤差制御と退化処理の内部関数群。

## API

- `length`, `scaled_tolerance`, `scaled_sign`, `cross_sign`: 長さとscale付き符号判定。
- `point_tolerance`, `point_equal`: 座標規模を考慮した点の等値判定。
- `side_value`, `side_sign`: 有向直線の左半平面に対する符号判定。
- `parallel_direction`, `same_direction`, `direction_less`, `direction_angle`: 方向の分類と角度順比較。
- `line_intersection_unchecked`: 非平行な2直線の交点。
- `canonicalize_start`, `rotate_to_lowest`: 頂点列の開始位置を正規化する。
- `on_segment_scaled`, `segment_intersection_set`: scale付き線分所属判定と共通部分列挙。
- `point_in_normalized_convex_polygon`: 正規化済み凸多角形への所属判定。
- `clip_segment_by_convex_polygon`: 線分を正規化済み凸多角形で切り取る。

## API別の時間計算量・空間計算量

- 点・方向・直線・線分の各定数個演算: 時間・追加領域 $O(1)$。
- `canonicalize_start`, `rotate_to_lowest`, `point_in_normalized_convex_polygon`, `clip_segment_by_convex_polygon`: 頂点数を $N$ として時間 $O(N)$、出力を除く追加領域 $O(1)$。

## 注意点

`advanced_geometry_detail` 名前空間の実装内部APIであり、互換性は保証しない。`line_intersection_unchecked` は非退化かつ非平行な直線を前提とする。
`scaled_tolerance(scale)` と `scaled_sign(value, scale)` は `abs(scale)` に比例する許容幅と丸め誤差を用い、単位が異なる量へ固定の絶対許容幅を加えない。
`side_sign` は境界方向を正規化して符号付き垂直距離を判定する。
