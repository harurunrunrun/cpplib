---
title: Point on 3D Circle (三次元円周上判定)
documentation_of: ../src/algorithm/geometry/3d/on_circle.hpp
---

## API

- `on_circle(circle, point)`: 点が円の平面上かつ中心から半径距離にあるか判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

Circle3 は中心・法線・半径が有限、法線が非零、半径が非負でなければならず、違反時は std::invalid_argument を送出する。

零法線または負半径には `std::invalid_argument` を送出する。半径0の円は中心1点を表す。
