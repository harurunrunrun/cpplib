---
title: 3D Finite Value Check (三次元幾何の有限値判定)
documentation_of: ../src/algorithm/geometry/3d/is_finite.hpp
---

## API

- `geometry3d_is_finite(value)`: `long double` が有限なら `true`。
- `geometry3d_is_finite(point)`: `Point3` の全座標が有限なら `true`。
- `geometry3d_is_finite(primitive)`: `Line3`、`Ray3`、`Segment3`、`Plane3`、`Circle3`、`Sphere3`、`Triangle3`、`Tetrahedron3` の全数値要素が有限なら `true`。

## API別の時間計算量・空間計算量

すべて時間・追加領域 $O(1)$。

## 注意点

半径の非負性や法線・方向ベクトルが零でないことは検査しない。`NaN` と正負の無限大は `false` になる。このAPIを構成計算へ入る前の入力検査に使用し、述語計算と座標構成を分離する。
