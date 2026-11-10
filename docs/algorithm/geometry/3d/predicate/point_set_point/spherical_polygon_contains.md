---
title: Spherical Polygon Containment (球面多角形の包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point_set_point/spherical_polygon_contains.hpp
---

## API

- `spherical_polygon_contains(polygon, query)`: 隣接する頂点を短い大円弧で結んだ球面多角形 `polygon` の内部または境界に方向 `query` があれば `true` を返す。頂点数が 3 未満なら `false` を返す。

## API別の時間計算量・空間計算量

頂点数を $N$ として時間 $O(N)$、追加領域 $O(N)$。

## 注意点

- 多角形は単純で、ある開半球内に収まり、頂点が境界に沿った順に並んでいることを前提とする。
- 頂点と `query` は有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を送出する。ベクトルの長さは結果に影響しない。
- 両端点が対蹠点である辺は短い大円弧が一意に定まらないため、`std::domain_error` を送出する。符号付き面積が定まらない配置でも `std::domain_error` を送出する。
- 境界上の方向は内部として扱う。
