---
title: 3D Linear Coincidence Umbrella (三次元線形図形の一致判定集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/linear_coincident.hpp
---

## API

- `coincident(Line3, Line3)`: 2 本の無限直線が点集合として一致するかを返す。
- `coincident(Ray3, Ray3)`: 始点と向きを含めて 2 本の半直線が一致するかを返す。
- `coincident(Segment3, Segment3)`: 端点の順序を無視して 2 本の閉線分が一致するかを返す。

このヘッダは同じ型同士の `*_coincident.hpp` をまとめて include する、互換用の集約ヘッダである。必要な型だけを使う場合は、対応する個別ヘッダを直接 include すればよい。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 点の一致は `Point3::operator==`（絶対誤差付き）で判定する。
- 退化した直線・半直線には `std::invalid_argument` を送出する。線分版は退化した線分も受け付ける。
