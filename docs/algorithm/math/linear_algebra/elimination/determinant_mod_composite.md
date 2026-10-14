---
title: Determinant Mod Composite (合成数を法とする行列式) [matrix_det_arbitrary_mod]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/determinant_mod_composite.hpp
---

`determinant_mod_composite(matrix, modulus)` は正方行列の行列式を法 `modulus` で返す。
逆元を使わず、Euclid の互除法による行操作を行う。負の行列要素も受け付ける。
空行列の行列式は $1\bmod m$。

## 時間計算量

$O(N^3\log m)$、空間 $O(N^2)$。入力行列を値渡しするので、元の行列を残す場合はコピーを使う。

## 注意点

- $1\le m\le\mathrm{INT\_MAX}$。積と行差を `long long` で計算する。
- 非正方行列、範囲外の法は `invalid_argument`。
- 素数を法とする場合も使えるが、体上の通常の掃き出し法の方が速い。
