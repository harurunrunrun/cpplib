---
title: Exact Orient3D for Integer Points (整数点の正確な三次元向き判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/scalar/exact_orient3d_integer_points.hpp
---

## API

- `exact_orient3d(first, second, third, fourth)`: `det(second - first, third - first, fourth - first)` の正確な符号を `-1`, `0`, `1` で返す。

## API別の時間計算量・空間計算量

座標差の絶対値がすべて $2^{40}$ 以下なら、固定幅整数（`__int128`）で時間・追加領域ともに $O(1)$。それ以外は値のビット長を $B$ として、内部の多倍長整数の乗算・加算の時間と追加領域 $O(B)$ を要する。

## 注意点

`IntegerPoint3`（`std::int64_t` の 3 つ組）の全座標範囲で正確であり、退化した入力には 0 を返す。固定幅の高速経路の範囲外ではライブラリ内の `ExactInteger` を使い、外部の多倍長整数ライブラリには依存しない。
