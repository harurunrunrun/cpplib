---
title: Closest Point on 3D Circle (三次元円周上の最近点)
documentation_of: ../src/algorithm/geometry/3d/circle_closest_point.hpp
---

## API

- `closest_point(circle, point)`: 円周上で点に最も近い点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点
計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Circle3 は中心・法線・半径が有限、法線が非零、半径が非負でなければならず、違反時は std::invalid_argument を送出する。

負半径または零法線には `std::invalid_argument` を送出する。正半径かつ点が円の法線軸上にある場合、最近点が一意でないため `std::domain_error` を送出する。半径0では中心を返す。
