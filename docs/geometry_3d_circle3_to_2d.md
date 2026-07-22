---
title: Circle3 to 2D (三次元円の二次元化)
documentation_of: ../src/algorithm/geometry/3d/shape/circle3_to_2d.hpp
---

## API

- `circle3_to_2d(circle, system)`: 円を指定平面座標系の`Circle`へ変換する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Circle3 は中心・法線・半径が有限、法線が非零、半径が非負でなければならず、違反時は std::invalid_argument を送出する。

負半径、零法線、座標平面と円平面が一致しない場合は`std::invalid_argument`を送出する。
