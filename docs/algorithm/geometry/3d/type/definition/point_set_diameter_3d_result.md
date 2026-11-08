---
title: 3D Point Set Diameter Result (三次元点集合直径結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/point_set_diameter_3d_result.hpp
---

## API

- `PointSetDiameter3DResult::first`: 直径を与える点対のうち、小さい方のインデックス（`std::size_t`）。
- `PointSetDiameter3DResult::second`: 直径を与える点対のうち、大きい方のインデックス（`std::size_t`）。
- `PointSetDiameter3DResult::distance`: 2 点間のユークリッド距離（`long double`）。

## API別の時間計算量・空間計算量

- 構築・各メンバへのアクセス: 時間・追加領域ともに $O(1)$。

## 注意点

型自体はインデックスや距離を検査しない。直径計算 API が返す値では `first < second` が成り立つ。
