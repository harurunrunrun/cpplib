---
title: Signed Tetrahedron Volume (四面体の符号付き体積)
documentation_of: ../src/algorithm/geometry/3d/signed_volume.hpp
---

## API

- `signed_volume(tetrahedron)`: 頂点順に対応する符号付き体積を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化四面体では0を返す。符号は右手系と頂点順に依存する。
