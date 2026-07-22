---
title: Fast Dynamic Matrix Power Modulo (高速動的行列の法付き累乗)
documentation_of: ../src/algorithm/math/fast_matrix_power_mod.hpp
---

実行時サイズの正方整数行列を、Strassen行列積を使って法付き累乗する。
通常の三乗行列積を使う `matrix_power_mod` とは別の、大きい密行列向けAPIである。

## API

```cpp
DynamicModMatrix fast_matrix_power_mod(
    DynamicModMatrix matrix,
    uint64_t exponent,
    int64_t modulus = 1'000'000'007
);
```

`matrix^exponent mod modulus` を返す。負の入力要素も
`0..modulus-1` へ正規化する。指数0では同じサイズの単位行列を返す。

## API別の時間計算量・空間計算量

行列の一辺を $D$、$S$ を $D$ 以上で最小の2の冪、
$\alpha=\log_2 7$ とする。

- 時間計算量: $O(S^\alpha\log(exponent+1)+D^2)$。
- 追加領域: $O(S^2)$。

$S<2D$ なので、大文字Oでは時間 $O(D^\alpha\log(exponent+1))$、
領域 $O(D^2)$ とも書ける。小さい再帰blockは通常の三重loopへ切り替える。
指数0では行列積を行わず $O(D^2)$ である。

## 注意点

- 行列は正方形、`modulus > 0` でなければならない。違反時は
  `std::invalid_argument`。
- `modulus` は正の `int64_t` 全域を使用できる。積は
  `unsigned __int128` で計算してから剰余を取り、加減算も各演算後に正規化する。
- Strassen法は通常の加減乗算より定数倍と一時領域が大きい。
  小さい行列では `matrix_power_mod` の方が速いことがある。
- 空の $0\times0$ 行列にも対応し、結果は空行列である。
