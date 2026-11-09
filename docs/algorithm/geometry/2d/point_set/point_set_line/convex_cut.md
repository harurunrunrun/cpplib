---
title: 2D Convex Polygon Cut (二次元凸多角形の切断)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_line/convex_cut.hpp
---

## API

- `std::vector<Point> convex_cut(polygon, line)`: 凸多角形のうち、有向直線 `line.a -> line.b` の左側にある部分を頂点列で返す。

## API別の時間計算量・空間計算量

- `convex_cut`: 頂点数を $N$ として、時間 $O(N)$、出力・追加領域 $O(N)$。

## 注意点

- 直線上の点も残す。
- 入力は頂点順に並んだ凸多角形でなければならない。
- `line` の方向ベクトルの長さが `GEOMETRY_EPS` 以下なら退化直線とみなし、`std::invalid_argument` を送出する。
- 方向ベクトルを単位化してから左右を判定するため、退化判定に該当しない範囲では `line.b - line.a` を正の定数倍しても切断結果は変わらない。
