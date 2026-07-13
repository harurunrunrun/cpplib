---
title: Convex Polygon Intersection (凸多角形の共通部分)
documentation_of: ../src/algorithm/geometry/2d/advanced/convex_polygon_intersection.hpp
---

## API

- `convex_polygon_intersection(first, second)`: 2閉凸集合の共通部分を正規化済み頂点列で返す。

空なら空列、点接触なら1点、線分なら両端、面積を持てば反時計回りの凸多角形を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N,M$ として、通常は時間 $O((N+M)\log(N+M))$、追加領域 $O(N+M)$。
- どちらかが点または線分なら時間 $O(N+M)$。

## 注意点

入力は弱凸な周回列でなければならない。無効な周回列には `std::invalid_argument` を送出する。
