---
title: Tetrahedron Volume (四面体の体積)
documentation_of: ../src/algorithm/geometry/3d/scalar/volume.hpp
---

## API

- `volume(tetrahedron)`: 四面体の非負体積を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化四面体では0を返す。
