---
title: GF(2) Determinant (二元体上の行列式) [matrix_det_mod_2]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/gf2/gf2_determinant.hpp
---

文字列で与えた 0/1 の正方行列の行列式を $\mathrm{GF}(2)$ 上で求める。

## `gf2_determinant`

```cpp
int gf2_determinant(const vector<string>& matrix)
```

各文字を $\mathrm{GF}(2)$ の要素とみなした行列式を `0` または `1` で返す。$0\times0$ 行列の行列式は `1` である。入力行列は変更しない。

## 時間計算量

行列を $N\times N$、機械語の bit 数を $w$ として $O(N^3/w)$。入力を bit 列に詰める処理に $O(N^2)$ かかる。

## 空間計算量

$O(N^2/w)$ 語。

## 注意点

- 全行の長さが `matrix.size()` と一致し、各文字が `0` または `1` である必要がある。違反時は `invalid_argument` を送出する。
- 語数の計算は、`matrix.size()` が `size_t` の最大値に近い場合にも加算でオーバーフローしない。
