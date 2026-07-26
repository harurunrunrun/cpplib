---
title: Line-Point Reflection (直線に関する点の鏡映)
documentation_of: ../../../../src/algorithm/geometry/3d/line_point_reflection.hpp
---

## API

- `reflection(line, point)`: 点を直線の周りに180度回転した点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

直線の二点は相異なり、入力座標は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
