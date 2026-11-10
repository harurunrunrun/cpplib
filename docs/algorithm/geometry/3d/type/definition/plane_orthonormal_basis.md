---
title: 3D Plane Orthonormal Basis (三次元平面の正規直交基底)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/plane_orthonormal_basis.hpp
---

## API

- `plane_orthonormal_basis(plane)`: 平面に平行で互いに直交する単位ベクトル $u, v$ を `std::pair<Point3, Point3>` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 法線が零ベクトルまたは非有限の場合は `std::invalid_argument` を送出する。
- 単位法線を $n$ とすると $v = n \times u$ であり、$(u, v, n)$ は右手系をなす。$u$ は $n$ と最も直交に近い座標軸との外積から作る。
