---
title: 3D Distance (三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/distance.hpp
---

## API

いずれも戻り値は `long double` である。

- `distance(Point3, Point3)`: 2 点間の距離。
- `distance(Line3, Point3)`, `distance(Ray3, Point3)`, `distance(Segment3, Point3)`, `distance(Plane3, Point3)`: 図形と点の距離。
- `distance(Line3, Line3)`, `distance(Segment3, Segment3)`: 同種の図形同士の距離。
- `distance(Triangle3, Point3)`: 閉三角形と点の距離。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した線分には対応する。
- 直線・半直線・平面・三角形の退化に関する条件に違反する入力と、非有限な入力には `std::invalid_argument` を送出する。
- 点の差を局所スケールに正規化し、最近点 API と同じ候補から距離を安全に復元する。直線間の距離は最近点の座標を構築せず、単位方向ベクトルの外積から直接求める。
- 真の距離、または必要な局所正規化差分を `long double` で表現できない場合は `std::overflow_error` を送出する。
