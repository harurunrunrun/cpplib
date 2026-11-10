---
title: Exact Point in Polygon (厳密点多角形内外判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/enumeration/point_set_point/point_in_polygon.hpp
---

点を多角形の外部・境界・内部に分類する。

## API

- `PointLocation point_in_polygon(const std::vector<Point>& polygon, const Point& point)`: 点の位置を `PointLocation::outside`、`PointLocation::boundary`、`PointLocation::inside` のいずれかで返す。多角形は頂点を巡回順に並べたものとし、いずれかの辺上にあれば `boundary`、そうでなければ回転数が $0$ でないとき `inside` を返す。空の多角形では `outside` を返す。

## API別の時間計算量・空間計算量

- `point_in_polygon(polygon, point)`: 時間 $O(n)$、追加領域 $O(1)$（$n$ は頂点数）

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
