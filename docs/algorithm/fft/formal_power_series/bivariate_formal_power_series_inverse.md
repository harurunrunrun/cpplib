---
title: Fast Bivariate Formal Power Series Inverse (高速二変数形式的冪級数逆数) [inv_of_formal_power_series_2d]
documentation_of: ../../../../src/algorithm/fft/formal_power_series/bivariate_formal_power_series_inverse.hpp
---

長方形領域で打ち切った二変数形式的冪級数の乗法逆元を、二次元 NTT と Newton 法で計算する。Newton 法では、まず列方向（$y$ の次数）、次に行方向（$x$ の次数）の順に、一軸ずつ打ち切り次数を倍にしていく。

## `bivariate_formal_power_series_inverse`

```cpp
template<int MOD>
std::vector<std::vector<Modint<MOD>>>
bivariate_formal_power_series_inverse(
    const std::vector<std::vector<Modint<MOD>>>& series
);
```

`series[i][j]` を $x^i y^j$ の係数とする。入力が $N \times M$ の行列のとき、同じ $N \times M$ の係数行列 `inverse` を返す。これは

$$
\mathtt{series} \cdot \mathtt{inverse} \equiv 1 \pmod{(x^N, y^M)}
$$

を満たす。

- 時間計算量: $O(NM (\log N + \log M))$
- 空間計算量: $O(NM)$

## 注意点

- 入力は空でない長方形の行列でなければならない。空なら、または行の長さが揃っていなければ `std::invalid_argument` を送出する。
- 定数項 `series[0][0]` は可逆でなければならない。$0$ なら `std::domain_error` を送出する。
- 各段階で行数・列数を 2 の冪へゼロ埋めして変換する。その変換長は `MOD - 1` を割り切る必要があり、満たさなければ `std::runtime_error` を送出する。$998244353$ では各軸について長さ $2^{23}$ までの変換を利用できる。
- 変換用配列の大きさが `std::size_t` に収まらない場合は `std::length_error` を送出する。
