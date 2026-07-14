---
title: Half-Plane Intersection Area (半平面共通部分の面積)
documentation_of: ../src/algorithm/geometry/2d/half_plane_intersection_area.hpp
---

## API

`long double half_plane_intersection_area(half_planes)`

## 引数

各 `Line{a,b}` の左閉半平面を表す境界直線列を渡す。

## 戻り値

共通部分が空なら `0`、有界なら非負面積、非空かつ無界なら正の無限大を返す。

## API別の時間計算量・空間計算量

- 半平面数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。

## 注意点

退化境界・非有限座標では `invalid_argument`、内部bounding計算が表現範囲を超える場合は `overflow_error`。
