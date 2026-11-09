---
title: Line-Point Incidence Test (直線と点の所属判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_point/intersect_line_point.hpp
---

点が無限直線上にあるか判定する。

## API

- `bool intersect(const Line& line, const Point& point)`: `point` が `line` 上にあれば `true` を返す。

## API別の時間計算量・空間計算量

- `intersect`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 方向ベクトル `line.b - line.a` の長さが `GEOMETRY_EPS` 以下の退化直線には `std::invalid_argument` を送出する。
- 判定には、方向ベクトルの長さを考慮した許容誤差を用いる。
