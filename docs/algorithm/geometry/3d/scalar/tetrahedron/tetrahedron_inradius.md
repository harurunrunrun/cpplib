---
title: 3D Tetrahedron Inradius (三次元四面体の内接球半径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/tetrahedron/tetrahedron_inradius.hpp
---

## API

- `tetrahedron_inradius(tetrahedron)`: 四面体の内接球の半径を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 退化した四面体や非有限な入力には `std::invalid_argument` を送出する。退化判定には厳密な `adaptive_orient3d` の符号を用いる。
- 基準頂点からの辺ベクトルを共通スケールで正規化し、正規化後の体積と表面積の比から半径を求める。正規化後の形状または半径を `long double` で表現できない場合は `std::overflow_error` を送出する。
