---
title: 3D Point Set Diameter Result (三次元点集合直径結果型)
documentation_of: ../../../../src/algorithm/geometry/3d/point_set_diameter_3d_result.hpp
---

## API

- `PointSetDiameter3DResult::first`: 直径を与える点対の小さい側のindex。
- `PointSetDiameter3DResult::second`: 直径を与える点対の大きい側のindex。
- `PointSetDiameter3DResult::distance`: 2点間のEuclidean距離。

## API別の時間計算量・空間計算量

- 構築・各fieldへのアクセス: 時間・追加領域 $O(1)$。

## 注意点

型自体はindexや距離を検査しない。直径計算APIが返す値では `first < second` が成り立つ。
