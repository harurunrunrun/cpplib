---
title: Circle3 to 2D (三次元円の二次元化)
documentation_of: ../../../../../../src/algorithm/geometry/3d/circle/circle/circle3_to_2d.hpp
---

## API

- `circle3_to_2d(circle, system)`: 三次元の円 `Circle3` を、平面座標系 `system` における二次元の `Circle` へ変換する。中心は `system.first_axis`, `system.second_axis` 方向の座標に変換し、半径はそのまま保つ。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 座標系の成分が非有限な場合、円の法線と座標系の法線が平行でない場合、円の中心が座標平面上にない場合（局所スケールに対する相対許容誤差で判定）にも `std::invalid_argument` を送出する。
- 中心と原点の差は共通の平行移動量を保つ正規化で計算し、中間計算のオーバーフローを避ける。変換後の中心座標を `long double` で表現できない場合は `std::overflow_error` を送出する。
