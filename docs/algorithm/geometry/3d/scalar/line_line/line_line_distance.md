---
title: Line-Line Distance (直線間距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_line/line_line_distance.hpp
---

## API

- `distance(first, second)`: 二直線間の最短距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

両直線は退化しておらず、座標は有限値でなければならない。
