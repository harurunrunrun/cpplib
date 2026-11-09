---
title: General Polygon Intersection Predicate (一般多角形の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_set_point_set/intersect_polygons.hpp
---

## API

- `bool intersect_polygons(const std::vector<Point>& first, const std::vector<Point>& second)`: 凸性を仮定しない 2 つの単純多角形の閉領域が共有点を持つかを返す。

## API別の時間計算量・空間計算量

頂点数を $N, M$ とする。

- AABB 階層の構築: $O(N \log N + M \log M)$
- 辺の候補探索: 最悪 $O(NM)$
- 追加空間計算量: $O(N + M)$

## 注意点

- 空の入力には `false` を返す。
- 有限座標の単純多角形を、境界に沿った頂点順で与える。非有限な座標には `std::invalid_argument` を送出する。
- 接触と包含も交差に含める（境界間の最小距離が `GEOMETRY_EPS` 以下、または一方が他方の頂点を含む場合）。
- 自己交差する多角形は対象外とする。
