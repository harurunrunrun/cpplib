---
title: 3D Triangle Inradius (三次元三角形の内接円半径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/triangle/triangle_inradius.hpp
---

## API

- `triangle_inradius(triangle)`: 三角形の内接円の半径を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 退化した三角形や非有限な入力には `std::invalid_argument` を送出する。退化判定は厳密な外積計算で行う。
- 辺ベクトルを共通のスケールで正規化し、正規化後の面積と周長の比から半径を求める。正規化後の形状または半径を `long double` で表現できない場合は `std::overflow_error` を送出する。
