---
title: 3D Tetrahedron Inradius (三次元四面体の内接球半径)
documentation_of: ../src/algorithm/geometry/3d/scalar/tetrahedron_inradius.hpp
---

## API

- `tetrahedron_inradius(tetrahedron)`: 内接球半径を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化四面体・非有限入力には `std::invalid_argument` を送出する。退化判定には正確な
`adaptive_orient3d` の符号を用いる。anchorからの辺差分を共通scaleへ正規化し、
局所体積と表面積の比から半径を求める。局所形状または半径を表現できない場合は
`std::overflow_error` を送出する。
