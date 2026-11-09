---
title: Line-Point Reflection (直線に関する点の鏡映)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/line_point/line_point_reflection.hpp
---

## API

- `reflection(line, point)`: `point` を直線の周りに $180$ 度回転した点（直線に関する鏡映）を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 直線の 2 点は相異なり、入力座標は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- 内部で `projection(line, point)` を用いる。
