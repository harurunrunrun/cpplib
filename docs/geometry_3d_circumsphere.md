---
title: 3D Tetrahedron Circumsphere (三次元四面体の外接球)
documentation_of: ../src/algorithm/geometry/3d/circumsphere.hpp
---

## API

- `circumsphere(tetrahedron)`: 4頂点を通る外接球を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

4頂点が共面な退化四面体には `std::invalid_argument` を送出する。
