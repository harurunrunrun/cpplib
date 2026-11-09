---
title: Polygon3 Ray Intersection (三次元多角形と半直線の交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polygon_ray/polygon3_ray_intersection.hpp
---

三次元空間内の平面多角形と半直線の共通部分を求める。

## API

- `polygon3_ray_intersection(polygon, ray)`: 半直線 $\lbrace \mathrm{origin} + t(\mathrm{through} - \mathrm{origin}) \mid t \ge 0 \rbrace$ と平面多角形の共通区間・孤立点をすべて `Polygon3LinearIntersection3` で返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。半直線の支持直線が多角形の平面を横切る場合は時間 $O(N)$、平面上にある場合は最悪時間 $O(N \log N)$。追加空間はいずれも $O(N)$。戻り値の `empty()` は $O(1)$。

## 注意点

- 非凸多角形との共通部分が複数の区間になる場合も、すべて保持する。半直線の始点も共通部分に含める。
- 退化した半直線（`origin` と `through` の距離が `GEOMETRY3D_EPS` 以下）や、頂点数 3 未満または頂点が共面でない多角形には `std::invalid_argument` を送出する。
