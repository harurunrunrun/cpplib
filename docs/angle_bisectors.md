---
title: Angle Bisectors of Two Lines (二直線の角の二等分線)
documentation_of: ../src/algorithm/geometry/2d/angle_bisectors.hpp
---

## API

`array<Line, 2> angle_bisectors(first, second)`

## 引数

交差する2本の非退化直線を渡す。

## 戻り値

交点を通る互いに直交した2本の角二等分線を返す。入力方向を辞書方向へ正規化した単位方向の和、差の順である。

## API別の時間計算量・空間計算量

- `angle_bisectors`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線では `invalid_argument`、平行または一致する直線では `domain_error`。
平行判定は正規化方向同士の角度に対する `GEOMETRY_EPS` を用いる。
