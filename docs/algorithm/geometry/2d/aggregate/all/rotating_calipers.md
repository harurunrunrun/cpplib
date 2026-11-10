---
title: Rotating Calipers Header (回転キャリパ法ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/rotating_calipers.hpp
---

凸多角形に対する回転キャリパー法の API をまとめて include する集約ヘッダである。

## API

- `convex_polygon_diameter`, `convex_polygon_farthest_vertices`
- `convex_polygon_maximum_area_triangle`, `convex_polygon_support_vertex`, `convex_polygon_tangents`
- 各結果型と共通の正規化関数については、それぞれのドキュメントを参照すること。

## 時間計算量・空間計算量

include 自体は実行時の時間・空間 $O(1)$ である。各関数の計算量はそれぞれのドキュメントに従う。

## 注意点

このヘッダ自身は、内部実装（`convex_calipers_detail.hpp`）以外に関数や型を追加で定義しない。入力の凸性、頂点の順序、退化した入力の条件は API ごとに異なる。
