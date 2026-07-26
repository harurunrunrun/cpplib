---
title: Point-Point Distance (点と点の距離)
documentation_of: ../../../../src/algorithm/geometry/3d/point_point_distance.hpp
---

## API

- `distance(left, right)`: 二点間のユークリッド距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

座標は有限値でなければならない。結果を表現できない場合は例外を送出する。
