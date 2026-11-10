---
title: Point on 3D Circle (三次元円周上判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/circle_point/on_circle.hpp
---

## API

- `on_circle(circle, point)`: 点が円を含む平面上にあり、かつ中心からの距離が半径に等しいかを、許容誤差 `GEOMETRY3D_EPS` の相対誤差の範囲で判定する。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時、および非有限な点には `std::invalid_argument` を送出する。
- 半径 $0$ の円は中心の 1 点を表す。
