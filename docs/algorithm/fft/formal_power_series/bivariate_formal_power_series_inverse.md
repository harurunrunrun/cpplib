---
title: Fast Bivariate Formal Power Series Inverse (高速二変数形式的冪級数逆数) [inv_of_formal_power_series_2d]
documentation_of: ../../../../src/algorithm/fft/formal_power_series/bivariate_formal_power_series_inverse.hpp
---

矩形領域で打ち切った二変数形式的冪級数の乗法逆元を、
2次元Number Theoretic TransformとNewton反復で計算する。
Newton反復は列方向、行方向の順に一軸ずつ打ち切り次数を倍化する。

## `bivariate_formal_power_series_inverse`

```cpp
template<int MOD>
vector<vector<Modint<MOD>>>
bivariate_formal_power_series_inverse(
    const vector<vector<Modint<MOD>>>& series
);
```

`series[i][j]` を $x^iy^j$ の係数とする。
入力と同じ $N\times M$ の係数行列 `inverse` を返し、

$$
\mathtt{series}\cdot\mathtt{inverse}\equiv1\pmod{(x^N,y^M)}
$$

を満たす。

- 時間計算量: $O(NM(\log N+\log M))$
- 空間計算量: $O(NM)$

## 注意点

入力は空でない矩形行列で、定数項 `series[0][0]` は可逆でなければならない。
各段階の行数・列数を2冪へzero paddingするため、その長さは `MOD - 1` を
割り切る必要がある。998244353では各軸について最大 $2^{23}$ のtransformを
利用できる。空入力、非矩形入力、零定数項、transform長または容量の違反では
例外を送出する。
