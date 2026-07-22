---
title: 3D Coplanarity (3次元共面判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/coplanar.hpp
---

## API

- `coplanar(first, second, third, fourth)`: 4 点の向き付き体積を適応的精度で評価し、厳密に同一平面上なら `true` を返す。

## API別の時間計算量・空間計算量

- `coplanar(first, second, third, fourth)`: 通常は時間 $O(1)$、追加空間 $O(1)$。誤差境界内では入力浮動小数点数の二進表現に対する多倍長整数の厳密計算へフォールバックする。

## 注意点

- 一致点や一直線上の点を含む退化配置も共面と判定する。
- NaN または無限大を含む入力には `std::invalid_argument` を送出する。
- 許容誤差による近似判定ではなく、渡された `long double` 値に対する厳密な符号判定である。
