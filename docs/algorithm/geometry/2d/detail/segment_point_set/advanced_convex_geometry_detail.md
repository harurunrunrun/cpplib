---
title: Advanced Geometry Internals (高度幾何の内部関数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/segment_point_set/advanced_convex_geometry_detail.hpp
---

高度な凸幾何で用いる、誤差制御と退化処理のための内部関数群。すべて名前空間 `advanced_geometry_detail` に属する。

## API

- `length`, `scaled_tolerance`, `scaled_sign`, `cross_sign`: 長さ（`hypot`）とスケール付きの符号判定。`cross_sign(u, v)` は 2 ベクトルを単位ベクトルに正規化した外積を `GEOMETRY_EPS` と比較し、どちらかの長さが `GEOMETRY_EPS` 以下なら 0 を返す。
- `point_tolerance`, `point_equal`: 座標の大きさを考慮した点の等値判定。
- `side_value`, `side_sign`: 有向直線の左側の半平面に対する値と符号の判定。
- `parallel_direction`, `same_direction`, `direction_less`, `direction_angle`: 方向の分類と偏角順の比較。`direction_angle` は $[0, 2\pi)$ の偏角を返す。
- `line_intersection_unchecked`: 平行でない 2 直線の交点。
- `canonicalize_start`, `rotate_to_lowest`: 頂点列の開始位置を正規化する。前者は辞書順最小の頂点、後者は $y$ 座標最小（同じなら $x$ 最小）の頂点を先頭にする。
- `on_segment_scaled`, `segment_intersection_set`: スケール付きの線分上判定と、2 線分の共通部分の列挙（0〜2 点。2 点なら重なり区間の両端）。
- `point_in_normalized_convex_polygon`: 正規化済み凸多角形に点が含まれるか（境界を含む）。
- `clip_segment_by_convex_polygon`: 線分を正規化済み凸多角形で切り取り、0〜2 点を返す。

## API別の時間計算量・空間計算量

- 点・方向・直線・線分に対する定数個の演算: 時間・追加領域とも $O(1)$。
- `canonicalize_start`, `rotate_to_lowest`, `point_in_normalized_convex_polygon`, `clip_segment_by_convex_polygon`: 頂点数を $N$ として時間 $O(N)$、出力を除く追加領域 $O(1)$。

## 注意点

- 実装内部の API であり、互換性は保証しない。
- `line_intersection_unchecked` は、非退化かつ平行でない直線を前提とする。
- `scaled_tolerance(scale)` と `scaled_sign(value, scale)` は、$\lvert \mathrm{scale} \rvert$ に比例する許容幅と丸め誤差を用い、単位の異なる量に固定の絶対許容幅を加えない。
- `side_sign` は境界の方向ベクトルを正規化し、符号付き垂直距離を判定する。
