---
title: 3D Triangle Normal (三次元三角形の法線)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/triangle/triangle_normal.hpp
---

## API

- `triangle_normal(triangle)`: 頂点順 `a`, `b`, `c` に対応する単位法線（`(b - a)` $\times$ `(c - a)` の向き）を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 内部で `unit` を用いる。外積が厳密に零ベクトルになる退化三角形（3 頂点が共線）や、外積が非有限になる場合は `std::invalid_argument` を送出する。
- 許容誤差による退化判定は行わないため、ほぼ共線な三角形でも外積が非零なら法線を返す。
