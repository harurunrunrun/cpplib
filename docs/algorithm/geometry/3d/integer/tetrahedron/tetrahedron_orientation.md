---
title: 3D Tetrahedron Orientation (三次元四面体の向き)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/tetrahedron/tetrahedron_orientation.hpp
---

## API

- `tetrahedron_orientation(tetrahedron)`: 頂点 `a`, `b`, `c`, `d` の向き付き 6 倍体積の符号を `-1`, `0`, `1` で返す（`adaptive_orient3d(a, b, c, d)` と同じ）。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

有限な `long double` 座標を 2 進有理数（dyadic）として正確に判定する。0 は 4 点が共面であることを表し、頂点を 2 つ入れ替えると非零の符号は反転する。非有限な座標には `std::invalid_argument` を送出する。
