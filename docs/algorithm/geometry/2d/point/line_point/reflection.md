---
title: 2D Reflection (二次元鏡映) [CGL_1_B]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/line_point/reflection.hpp
---

## API

- `Point reflection(const Line& line, const Point& point)`: `point` を無限直線 `line` に関して鏡映した点を返す。

## API別の時間計算量・空間計算量

- `reflection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線では、内部で呼ぶ `projection` が `std::invalid_argument` を送出する。
