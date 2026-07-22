---
title: Convex Polygon Intersection (凸多角形の共通部分)
documentation_of: ../src/algorithm/geometry/2d/advanced/convex_polygon_intersection.hpp
---

## API

- `convex_polygon_intersection(first, second)`: 2閉凸集合の共通部分を正規化済み頂点列で返す。

空なら空列、点接触なら1点、線分なら両端、面積を持てば反時計回りの凸多角形を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N,M$ として、時間 $O(N+M)$、追加領域 $O(N+M)$。

正規化後の2本の辺方向列を線形マージし、角度順になった半平面を
dequeで一度ずつ処理する。点・線分へ退化した入力も同じ上界に含む。

## 注意点

入力は時計回りまたは反時計回りの弱凸な周回列でなければならない。
左右の折れ曲がりが混在する列、自己交差する星形、同じ輪郭を複数周する列には
`std::invalid_argument` を送出する。

座標と交点計算の中間値は有限な `long double` の範囲に収まる必要がある。
境界・平行・重複の判定には `GEOMETRY_EPS` と機械丸め誤差を用いる。
