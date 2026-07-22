---
title: 3D Tetrahedron Orientation (三次元四面体の向き)
documentation_of: ../src/algorithm/geometry/3d/predicate/tetrahedron_orientation.hpp
---

## API

- `tetrahedron_orientation(tetrahedron)`: 向き付き6倍体積の符号を -1、0、1 で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

有限 long double 座標をdyadic数として正確に判定する。0は4点が共面であることを表し、頂点2個を交換すると非零の符号が反転する。非有限座標には std::invalid_argument を送出する。
