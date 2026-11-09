---
title: 3D Orthonormal Basis (三次元正規直交基底)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/point/orthonormal_basis_3d.hpp
---

指定した方向を第 1 軸とする、右手系の三次元正規直交基底を構築する。

## API

- `orthonormal_basis_3d(first_axis)`: `std::array<Point3, 3>` として $(u, v, w)$ を返す。$u$ は `first_axis` と同じ向きの単位ベクトルで、$w = u \times v$ である。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 第 1 軸の成分の絶対値が最小となる座標軸（第 1 軸と最も平行でない座標軸）を補助方向に選び、外積の桁落ちを避ける。
- 入力は最大絶対成分で割ってから正規化するため、極端に大きい・小さい成分でもオーバーフロー・アンダーフローしにくい。
- ゼロベクトルまたは非有限な入力には `std::invalid_argument` を送出する。
