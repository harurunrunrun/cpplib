---
title: 2D Point in Polygon (二次元多角形包含判定) [CGL_3_C]
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/point_set_point/contains.hpp
---

## API

- `int contains(const std::vector<Point>& polygon, const Point& point)`: 外部なら 0、境界上なら 1、内部なら 2 を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

単純多角形を頂点順に与える。時計回り・反時計回りのどちらでもよい。
境界上かどうかは `on_segment` と同じ絶対距離 `GEOMETRY_EPS` で判定する。半直線との交差判定では、$y$ 方向の比較を半開区間の規則に従い、許容誤差なしの厳密な比較で行う。
