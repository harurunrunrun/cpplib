---
title: 3D Tetrahedron Inradius (三次元四面体の内接球半径)
documentation_of: ../src/algorithm/geometry/3d/tetrahedron_inradius.hpp
---

## API

- `tetrahedron_inradius(tetrahedron)`: 内接球半径を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化四面体には `std::invalid_argument` を送出する。退化判定には正確な `adaptive_orient3d` の符号を用いる。
