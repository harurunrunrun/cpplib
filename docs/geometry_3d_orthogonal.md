---
title: 3D Orthogonality (三次元直交判定)
documentation_of: ../src/algorithm/geometry/geometry_3d/orthogonal.hpp
---

## API

- `orthogonal(left, right)`: 2ベクトルが直交するか許容誤差付きで判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

判定には内積と `GEOMETRY3D_EPS` を用いる。
