---
title: Spherical Polygon Containment (球面多角形の包含判定)
documentation_of: ../src/algorithm/geometry/3d/spherical_polygon_contains.hpp
---

## API

- `spherical_polygon_contains(polygon, query)`: 短い大円弧を辺とする球面多角形の内部または境界に`query`方向があれば`true`を返す。頂点が3未満なら`false`。

## API別の時間計算量・空間計算量

頂点数を$N$として時間$O(N)$、追加領域$O(N)$。

## 注意点

単純多角形が一つの開半球内にあり、頂点が境界順に並ぶことを前提とする。入力は有限な非零ベクトルでなければならない。反対な端点を持つ辺は一意でないため`std::domain_error`を送出する。境界を内部として扱う。
