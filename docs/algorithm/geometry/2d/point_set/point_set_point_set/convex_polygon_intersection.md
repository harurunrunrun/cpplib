---
title: Convex Polygon Intersection (凸多角形の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_point_set/convex_polygon_intersection.hpp
---

## API

- `std::vector<Point> convex_polygon_intersection(first, second)`: 2 つの閉凸集合の共通部分を、`normalize_convex_polygon` で正規化した頂点列として返す。

共通部分が空なら空列、1 点で接するならその 1 点、線分なら両端の 2 点、面積を持つなら反時計回りの凸多角形を返す。

## API別の時間計算量・空間計算量

- `convex_polygon_intersection`: 頂点数を $N, M$ として、時間 $O(N + M)$、追加領域 $O(N + M)$。

正規化した 2 つの多角形の辺を方向順に線形マージし、角度順に並んだ半平面を両端キュー（`std::deque`）で一度ずつ処理する。点や線分に退化した入力も同じ上界に収まる。

## 注意点

- 入力は時計回りまたは反時計回りの弱凸な周回列でなければならない。左折と右折が混在する列、自己交差する星形、同じ輪郭を複数周する列には `std::invalid_argument` を送出する。
- 座標と交点計算の中間値は有限な `long double` の範囲に収まる必要がある。
- 境界・平行・重複の判定には `GEOMETRY_EPS` と機械丸め誤差を用いる。
