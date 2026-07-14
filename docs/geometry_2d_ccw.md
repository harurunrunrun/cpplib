---
title: 2D Counterclockwise Test (二次元反時計回り判定) [CGL_1_C]
documentation_of: ../src/algorithm/geometry/2d/ccw.hpp
---

3点の位置関係を分類する。

## API

- `ccw(first, second, third)`: `COUNTER_CLOCKWISE(1)`, `CLOCKWISE(-1)`, `ONLINE_BACK(2)`, `ONLINE_FRONT(-2)`, `ON_SEGMENT(0)` のいずれかを返す。
- 上記5定数: 判定結果を表す列挙値。

## API別の時間計算量・空間計算量

- `ccw` と定数参照: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

共線判定には `GEOMETRY_EPS` を用いる。`first` から `second` への有向線分を基準とする。
