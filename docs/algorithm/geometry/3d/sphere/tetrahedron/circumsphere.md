---
title: 3D Tetrahedron Circumsphere (三次元四面体の外接球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/tetrahedron/circumsphere.hpp
---

## API

- `circumsphere(tetrahedron)`: 四面体の 4 頂点を通る外接球を `Sphere3` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 4 頂点が同一平面上にある退化四面体や非有限な入力には `std::invalid_argument` を送出する。
- 退化判定とクラメルの公式は、入力の `long double` 値を二進有理数として厳密に計算し、最後に中心座標だけを丸める。そのため、巨大な平行移動や一様に巨大・微小なスケールの入力でも中間計算でオーバーフロー・アンダーフローは起きない。
- 中心または半径を `long double` で表現できない場合は `std::overflow_error` を送出する。
