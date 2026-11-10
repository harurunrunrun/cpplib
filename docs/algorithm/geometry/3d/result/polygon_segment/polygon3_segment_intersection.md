---
title: Polygon3 Segment Intersection (三次元多角形と線分の交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polygon_segment/polygon3_segment_intersection.hpp
---

三次元空間内の平面多角形と線分の共通部分を求める。

## API

- `polygon3_segment_intersection(polygon, segment)`: 線分と平面多角形の共通区間・孤立点をすべて `Polygon3LinearIntersection3` で返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

- 線分の支持直線が多角形の平面を横切る場合: 時間 $O(N)$。
- 線分が多角形の平面上にある場合: 最悪時間 $O(N \log N)$。
- 退化した線分を一点として調べる場合: 時間 $O(N)$。

追加空間はいずれも $O(N)$。戻り値の `empty()` は $O(1)$。

## 注意点

- 線分の両端点も共通部分に含める。
- 両端点が完全に一致する退化線分は一点として扱う。一方、両端点が異なっていても長さが `GEOMETRY3D_EPS` 以下の線分には `std::invalid_argument` を送出する。
- 頂点数 3 未満または頂点が共面でない多角形には `std::invalid_argument` を送出する。
