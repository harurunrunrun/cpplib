---
title: Fast Dynamic Matrix Power Modulo (高速動的行列の法付き累乗)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/matrix_operations/fast_matrix_power_mod.hpp
---

実行時にサイズが決まる正方整数行列の冪を、Strassen 法の行列積を使って法 `modulus` の下で求める。通常の $O(D^3)$ の行列積を使う `matrix_power_mod` とは別の、大きい密行列向けの API である。

## API

```cpp
DynamicModMatrix fast_matrix_power_mod(
    DynamicModMatrix matrix,
    uint64_t exponent,
    int64_t modulus = 1'000'000'007
);
```

$\mathrm{matrix}^{\mathrm{exponent}} \bmod \mathrm{modulus}$ を返す。負の入力要素も `0..modulus-1` に正規化する。指数が 0 なら同じサイズの単位行列を返す。

## 時間計算量・空間計算量

行列の一辺を $D$、$S$ を $D$ 以上で最小の 2 の冪、$\omega=\log_2 7$、$e$ を `exponent` とする。

- 時間計算量: $O(S^\omega\log(e+1)+D^2)$
- 追加領域: $O(S^2)$

$S<2D$ なので、時間 $O(D^\omega\log(e+1))$、領域 $O(D^2)$ とも書ける。再帰の小さいブロックでは通常の三重ループに切り替える。指数が 0 なら行列積を行わず $O(D^2)$ である。

## 注意点

- 行列は正方、`modulus > 0` でなければならない。違反時は `std::invalid_argument` を送出する。
- `modulus` には正の `int64_t` 全域を使える。積は `unsigned __int128` で計算してから剰余を取り、加減算も各演算の後に正規化する。
- Strassen 法は通常の行列積より定数倍と一時領域が大きい。小さい行列では `matrix_power_mod` の方が速いことがある。
- 空の $0\times0$ 行列にも対応し、結果は空行列である。
