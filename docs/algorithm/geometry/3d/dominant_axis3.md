---
title: Dominant Axis 3D Result Type (三次元支配軸結果型)
documentation_of: ../../../../src/algorithm/geometry/3d/dominant_axis3.hpp
---

## API

- `DominantAxis3::x`: $x$ 軸を表す。
- `DominantAxis3::y`: $y$ 軸を表す。
- `DominantAxis3::z`: $z$ 軸を表す。

## API別の時間計算量・空間計算量

- 構築・比較: 時間・追加領域 $O(1)$。

## 注意点

`dominant_axis` の結果や `dominant_axis_projection` で除去する軸を表す列挙型である。
