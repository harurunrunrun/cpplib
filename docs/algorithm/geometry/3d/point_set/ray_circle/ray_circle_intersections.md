---
title: 3D Ray and Circle Intersections (三次元半直線と円の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/ray_circle/ray_circle_intersections.hpp
---

## API

- `ray_circle_intersections(ray, circle)`: 閉半直線（始点を含む）と円周の相異なる交点を $0$ 個から $2$ 個返す。

## API 別の時間計算量・空間計算量

時間・出力以外の追加領域ともに $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 退化した半直線、非有限な半直線にも `std::invalid_argument` を送出する。
- 交点計算は共通の平行移動量を保つ正規化を行い、中間計算のオーバーフローを避ける。交点が `long double` で表現できない場合は `std::overflow_error` を送出する。
