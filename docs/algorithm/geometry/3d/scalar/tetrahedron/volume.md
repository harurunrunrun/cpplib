---
title: Tetrahedron Volume (四面体の体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/tetrahedron/volume.hpp
---

## API

- `volume(tetrahedron)`: 四面体の体積（非負）を返す。`signed_volume(tetrahedron)` の絶対値である。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 4 点が同一平面上にある退化四面体では $0$ を返す（浮動小数点誤差により厳密に $0$ になるとは限らない）。
- 入力の検証やオーバーフロー対策は行わない。
