---
title: Rotating Calipers Header (回転キャリパ法ヘッダ)
documentation_of: ../src/algorithm/geometry/rotating_calipers.hpp
---

凸多角形向け回転キャリパ法APIをまとめてincludeするheader。

## API

- `convex_polygon_diameter`, `convex_polygon_farthest_vertices`
- `convex_polygon_maximum_area_triangle`, `convex_polygon_support_vertex`, `convex_polygon_tangents`
- 各結果型と共通の正規化関数は、それぞれの個別documentationを参照する。

## API別の時間計算量・空間計算量

- include自体: 実行時の時間・領域 $O(1)$。各関数の計算量は個別documentationに従う。

## 注意点

このheader自身は追加の関数や型を定義しない。入力の凸性、頂点順、退化入力の条件は各APIごとに異なる。
