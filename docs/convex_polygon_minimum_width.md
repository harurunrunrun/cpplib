---
title: Convex Polygon Minimum Width (凸多角形の最小幅)
documentation_of: ../src/algorithm/geometry/2d/convex_polygon_minimum_width.hpp
---

## API

- `convex_polygon_minimum_width(polygon)`: 平行な2支持直線で挟む幅の最小値を返す。

## 引数

`polygon` は `NormalizedConvexPolygon` または弱凸な周回列である。

## 戻り値

最小幅を `long double` で返す。

## API別の時間計算量・空間計算量

- 正規化済みoverload: 時間 $O(N)$、追加領域 $O(1)$。
- 頂点列overload: 時間・領域 $O(N)$。

## 注意点

点・線分へ退化した凸集合の幅は0。非凸入力には `std::invalid_argument` を送出する。
