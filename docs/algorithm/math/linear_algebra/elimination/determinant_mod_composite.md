---
title: Determinant Mod Composite (合成数を法とする行列式) [matrix_det_arbitrary_mod]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/determinant_mod_composite.hpp
---

```cpp
long long determinant_mod_composite(std::vector<std::vector<long long>> matrix, long long modulus);
```

正方行列の行列式を法 `modulus`（以下 $m$）で返す。逆元を使わず、Euclid の互除法による行操作で消去するため、$m$ は合成数でもよい。負の行列要素も受け付ける。空行列の行列式は $1 \bmod m$ である。

## 時間計算量

$O(N^3\log m)$ 時間、$O(N^2)$ 空間。入力行列は値渡しなので、呼び出し元の行列は変更されない。

## 注意点

- $1\le m\le 2^{31}-1$（`INT_MAX`）でなければならない。積と行の差は `long long` で計算する。
- 非正方行列や範囲外の法では `invalid_argument` を送出する。
- 素数を法とする場合にも使えるが、体上の通常の掃き出し法の方が速い。
