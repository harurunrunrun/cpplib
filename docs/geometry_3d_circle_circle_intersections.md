---
title: 3D Circle and Circle Intersections (三次元円同士の交点)
documentation_of: ../src/algorithm/geometry/3d/circle_circle_intersections.hpp
---

## API

- `circle_circle_intersections(first, second)`: 空間内の2円周の相異なる交点を0個から2個返す。

## API別の時間計算量・空間計算量

- 時間・出力以外の追加領域 $O(1)$。

## 注意点

正半径の2円が一致すると交点が無限個なので `std::domain_error` を送出する。一致する半径0の円は中心1点を返す。負半径または零法線には `std::invalid_argument` を送出する。
