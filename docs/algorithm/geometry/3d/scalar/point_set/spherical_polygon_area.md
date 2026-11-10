---
title: Spherical Polygon Area (球面多角形の面積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_set/spherical_polygon_area.hpp
---

## API

- `spherical_polygon_area(polygon, radius = 1)`: 頂点を順に短い方の大円弧で結んだ、半径 `radius` の球面上の多角形の面積（非負）を返す。頂点数が 3 未満なら $0$ を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(N)$。

## 注意点

- 単純な多角形が 1 つの開半球に収まり、入力順が境界に沿った順序であることを前提とする。先頭頂点からの扇形分割で各球面三角形の符号付き面積を足し合わせる。
- 頂点は有限な非零ベクトル、半径は有限な非負値でなければならず、違反時は `std::invalid_argument` を送出する。
- 対蹠点を結ぶ辺は許さない。扇形分割の三角形の境界が一意に定まらない場合は `std::domain_error` を送出する。
- 向きを反転しても同じ非負の面積を返す。面積が `long double` で表現できない場合は `std::overflow_error` を送出する。
