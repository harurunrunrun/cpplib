---
title: GF(2) Determinant (二元体上の行列式) [matrix_det_mod_2]
documentation_of: ../src/algorithm/math/linear_algebra/gf2_determinant.hpp
---

文字列で与えたbinary正方行列の行列式を $\mathrm{GF}(2)$ 上で求める。

## `gf2_determinant`

```cpp
int gf2_determinant(const vector<string>& matrix)
```

各文字を $\mathrm{GF}(2)$ の要素とみなした行列式を `0` または `1` で返す。
$0\times0$ 行列の行列式は `1`。入力行列は変更しない。

## 時間計算量

行列を $N\times N$、機械語のbit数を $w$ として $O(N^3/w)$。
入力のpackingに $O(N^2)$ を要する。

## 空間計算量

$O(N^2/w)$ machine words。

## 注意点

全行の長さが `matrix.size()` と一致し、文字が `0` または `1` である必要がある。
違反時は `invalid_argument`。word数の計算は `matrix.size()` が
`size_t` の最大値に近い場合にも加算overflowしない。
