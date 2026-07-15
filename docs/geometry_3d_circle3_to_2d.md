---
title: Circle3 to 2D (三次元円の二次元化)
documentation_of: ../src/algorithm/geometry/3d/circle3_to_2d.hpp
---

## API

- `circle3_to_2d(circle, system)`: 円を指定平面座標系の`Circle`へ変換する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

負半径、零法線、座標平面と円平面が一致しない場合は`std::invalid_argument`を送出する。
