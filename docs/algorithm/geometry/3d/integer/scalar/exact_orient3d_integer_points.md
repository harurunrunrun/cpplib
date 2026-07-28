---
title: Exact Orient3D for Integer Points (整数点の正確な三次元向き判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/scalar/exact_orient3d_integer_points.hpp
---

## API

- `exact_orient3d(first, second, third, fourth)`: `det(second-first, third-first, fourth-first)` の正確な符号を `-1,0,1` で返す。

## API別の時間計算量・空間計算量

全座標差が $2^{40}$ 以下なら固定幅整数で時間・追加領域 $O(1)$。それ以外は値のbit長を $B$ として、内部多倍長整数の乗加算時間と追加領域 $O(B)$ を要する。

## 注意点

`IntegerPoint3` の全座標域で正確であり、退化入力は0を返す。固定幅fast pathの範囲外ではライブラリ内の `ExactInteger` を使い、外部多倍長ライブラリには依存しない。
